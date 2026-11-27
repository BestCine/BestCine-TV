#include "trailerimdb.h"
#include "idbase.h"
#include "rede.h"
#include "js.h"
#include "dados.h"
#include "ajustes.h"
#include "perfiltv.h"
#include "tex_cache.h"
#include "memlog.h"
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

extern const char *desc_chave_tmdb(void) __attribute__((weak));
extern const char *desc_chave_tmdb_reserva(void) __attribute__((weak));

#ifndef NV_REC_URL
#define NV_REC_URL ""
#endif

#define TR_MAX 32
// Ate quatro definicoes por titulo (o IMDb serve 1080p, 720p, 480p e SD);
// quem le escolhe pelo teto dos Ajustes.
#define DEF_MAX 4
typedef struct {
  char imdb[32];
  int  n;
  int  alt[DEF_MAX];
  char url[DEF_MAX][1024];
  char nome[64];
  long expira;        // epoch; 0 = sem trailer (resposta negativa)
  int  emVoo, respondeu;
} Entrada;
static Entrada tab[TR_MAX];
static int nTab, prox, emVooTotal;
static SDL_mutex *mtx;
#define TR_MAX_CONCORRENTES 2

static void trancar(void)   { if (!mtx) mtx = SDL_CreateMutex(); SDL_LockMutex(mtx); }
static void destrancar(void){ SDL_UnlockMutex(mtx); }

static Entrada *achar(const char *imdb) {
  int i;
  for (i = 0; i < nTab; i++) if (!strcmp(tab[i].imdb, imdb)) return &tab[i];
  return NULL;
}
static Entrada *reservar(const char *imdb) {
  Entrada *e = achar(imdb);
  if (e) return e;
  if (nTab < TR_MAX) e = &tab[nTab++];
  else { e = &tab[prox]; prox = (prox + 1) % TR_MAX; }
  memset(e, 0, sizeof *e);
  snprintf(e->imdb, sizeof e->imdb, "%s", imdb);
  return e;
}

// "Expires=1790035589" dentro da URL assinada.
static long expiraDe(const char *url) {
  const char *p = strstr(url, "Expires=");
  return p ? atol(p + 8) : 0;
}

static void caminhoDisco(const char *imdb, char *dst, unsigned tam) {
  char pasta[512], limpo[32];
  size_t i;
  dst[0] = 0;
  if (!dados_caminho(pasta, sizeof pasta, "trailer")) return;
  mkdir(pasta, 0755);
  snprintf(limpo, sizeof limpo, "%s", imdb ? imdb : "");
  for (i = 0; limpo[i]; i++) if (limpo[i] == ':' || limpo[i] == '/') limpo[i] = '_';
  snprintf(dst, tam, "%s/%s", pasta, limpo);
}

// Formato: linha 1 "expira nome", depois uma linha "altura url" por definicao.
static int lerDisco(Entrada *e) {
  char c[600], linha[1200];
  FILE *f;
  caminhoDisco(e->imdb, c, sizeof c);
  if (!c[0] || !(f = fopen(c, "r"))) return 0;
  if (!fgets(linha, sizeof linha, f)) { fclose(f); return 0; }
  linha[strcspn(linha, "\r\n")] = 0;
  e->expira = atol(linha);
  { const char *sp = strchr(linha, ' ');
    snprintf(e->nome, sizeof e->nome, "%s", sp ? sp + 1 : ""); }
  e->n = 0;
  while (e->n < DEF_MAX && fgets(linha, sizeof linha, f)) {
    const char *sp;
    linha[strcspn(linha, "\r\n")] = 0;
    sp = strchr(linha, ' ');
    if (!sp || atoi(linha) <= 0) continue;
    e->alt[e->n] = atoi(linha);
    snprintf(e->url[e->n], sizeof e->url[e->n], "%s", sp + 1);
    e->n++;
  }
  fclose(f);
  return 1;
}
static void gravarDisco(const Entrada *e) {
  char c[600];
  FILE *f;
  int i;
  caminhoDisco(e->imdb, c, sizeof c);
  if (!c[0] || !(f = fopen(c, "w"))) return;
  fprintf(f, "%ld %s\n", e->expira, e->nome);
  for (i = 0; i < e->n; i++) fprintf(f, "%d %s\n", e->alt[i], e->url[i]);
  fclose(f);
  dados_marcar_sujo(1);   // IDBFS (Samsung): cache re-obtivel, descarga leve
}

static int valido(const Entrada *e) {
  // Uma hora de folga: a URL nao pode vencer no meio do trailer.
  return e->respondeu && e->expira > (long)time(NULL) + 3600;
}

// Recolhe todos os MP4 de playbackURLs (uma URL por definicao) em `e`, em
// ordem decrescente de altura. Devolve a maior altura, 0 sem MP4.
static int escolher(const char *json, Entrada *e) {
  const char *no = strstr(json, "\"node\"");
  const char *fim = json + strlen(json);
  const char *p;
  e->n = 0;
  if (!no) return 0;
  { const char *n2 = strstr(no, "\"name\"");
    if (n2 && n2 < fim) js_texto(n2, fim, "value", e->nome, sizeof e->nome); }
  p = js_array(no, fim, "playbackURLs");
  for (; p; p = js_prox(js_fim(p))) {
    const char *pe = js_fim(p);
    char mime[16], def[16], u[1024];
    int alt, i, j;
    js_texto(p, pe, "videoMimeType", mime, sizeof mime);
    js_texto(p, pe, "videoDefinition", def, sizeof def);
    if (strcmp(mime, "MP4")) continue;
    // "DEF_1080p", "DEF_720p", "DEF_480p"; "DEF_SD" conta como 360.
    alt = !strcmp(def, "DEF_SD") ? 360 : atoi(def + (strncmp(def, "DEF_", 4) ? 0 : 4));
    if (alt <= 0) continue;
    if (!js_texto(p, pe, "url", u, sizeof u)) continue;
    for (i = 0; i < e->n && e->alt[i] > alt; i++) ;
    if (i < e->n && e->alt[i] == alt) continue;
    if (e->n >= DEF_MAX) { if (i >= DEF_MAX) continue; e->n = DEF_MAX - 1; }
    for (j = e->n; j > i; j--) { e->alt[j] = e->alt[j - 1]; memcpy(e->url[j], e->url[j - 1], sizeof e->url[j]); }
    e->alt[i] = alt;
    snprintf(e->url[i], sizeof e->url[i], "%s", u);
    e->n++;
  }
  return e->n ? e->alt[0] : 0;
}

typedef struct { char chave[32]; int serie; } PedidoImdb;

static void resolverImdbDeTmdb(long tmdbId, int serie, char *dst, size_t cap) {
  const char *ck = NULL;
  char url[320], *resp;
  dst[0] = 0;
  if (tmdbId <= 0) return;
  if (desc_chave_tmdb) ck = desc_chave_tmdb();
  if ((!ck || !ck[0]) && desc_chave_tmdb_reserva) ck = desc_chave_tmdb_reserva();
  if (!ck || !ck[0]) return;
  snprintf(url, sizeof url, "https://api.themoviedb.org/3/%s/%ld/external_ids?api_key=%s",
           serie ? "tv" : "movie", tmdbId, ck);
  resp = rede_baixar(url, 8);
  if (resp) {
    js_texto(resp, NULL, "imdb_id", dst, cap);
    free(resp);
  }
  if (strncmp(dst, "tt", 2)) dst[0] = 0;
}

static void *buscar(void *arg) {
  PedidoImdb *ped = (PedidoImdb *)arg;
  char chave[32], imdb[24] = "";
  int serie = ped ? ped->serie : 0;
  char consulta[512];
  char *corpo = NULL;
  Entrada lida;
  static const char *const cabs[] = {
    "Accept: application/json",
    "Content-Type: application/json",
    "Referer: https://www.imdb.com/",
    NULL };
  Entrada *e;
  snprintf(chave, sizeof chave, "%s", ped ? ped->chave : "");
  free(ped);
  if (!strncmp(chave, "tt", 2)) {
    snprintf(imdb, sizeof imdb, "%s", chave);
  } else if (!strncmp(chave, "tmdb:", 5)) {
    resolverImdbDeTmdb(atol(chave + 5), serie, imdb, sizeof imdb);
  }
  if (imdb[0]) {
    // %7B = {  %22 = "  %28 = (  %29 = )  %3A = :  %7D = }
    snprintf(consulta, sizeof consulta,
      "https://api.graphql.imdb.com/?query=%%7Btitle%%28id%%3A%%22%s%%22%%29%%7BprimaryVideos%%28first%%3A1%%29%%7Bedges%%7Bnode%%7Bname%%7Bvalue%%7DplaybackURLs%%7Burl%%20videoMimeType%%20videoDefinition%%7D%%7D%%7D%%7D%%7D%%7D",
      imdb);
#ifdef __EMSCRIPTEN__
    // SAMSUNG (#136): o navegador nao deixa por o Referer e a API nao manda
    // CORS a um wgt — a mesma consulta vai pelo servico de recomendacoes, que
    // devolve a resposta crua. Sem ele na build nao ha por onde (e
    // trailerfonte_imdb_tizen ja tirou o IMDb da ordem).
    (void)cabs;
    if (NV_REC_URL[0]) snprintf(consulta, sizeof consulta, "%s/v1/trailer/imdb?id=%s", NV_REC_URL, imdb);
    else consulta[0] = 0;
    corpo = consulta[0] ? rede_baixar(consulta, 15) : NULL;
#else
    corpo = rede_baixar_com(consulta, 15, cabs);
#endif
  }
  memset(&lida, 0, sizeof lida);
  snprintf(lida.nome, sizeof lida.nome, "Trailer");
  { int alt = corpo ? escolher(corpo, &lida) : 0;
    printf("[trailer] imdb %s (%s): %s%dp (%d definicoes)\n", chave, imdb[0] ? imdb : "-",
           !corpo ? "sem resposta " : alt ? "MP4 ate " : "sem trailer ", alt, lida.n);
    fflush(stdout); }
  trancar();
  e = reservar(chave);
  e->emVoo = 0; e->respondeu = 1;
  if (emVooTotal > 0) emVooTotal--;
  e->n = lida.n;
  memcpy(e->alt, lida.alt, sizeof e->alt);
  memcpy(e->url, lida.url, sizeof e->url);
  snprintf(e->nome, sizeof e->nome, "%s", lida.nome);
  // Sem resposta: nao grava (tenta de novo na proxima abertura). Sem trailer:
  // vale um dia. Com trailer: vale ate a assinatura vencer.
  e->expira = !corpo ? 0 : e->n ? expiraDe(e->url[0]) : (long)time(NULL) + 86400;
  if (corpo) gravarDisco(e);
  destrancar();
  free(corpo);
  return NULL;
}

void trailerimdb_pedir_tipo(const char *imdb, int serie) {
  Entrada *e;
  pthread_t f;
  PedidoImdb *ped;
  char chave[32];
  if (!imdb || (strncmp(imdb, "tt", 2) && strncmp(imdb, "tmdb:", 5))) return;
  idbase_copiar(imdb, chave, sizeof chave);
  if (!chave[0]) return;
  trancar();
  e = reservar(chave);
  if (!e->respondeu && !e->emVoo && lerDisco(e)) e->respondeu = 1;
  if (e->emVoo || valido(e)) { destrancar(); return; }
  if (emVooTotal >= TR_MAX_CONCORRENTES) { destrancar(); return; }
  e->emVoo = 1; e->respondeu = 0;
  emVooTotal++;
  destrancar();
  ped = calloc(1, sizeof *ped);
  if (!ped) { trancar(); e->emVoo = 0; if (emVooTotal > 0) emVooTotal--; destrancar(); return; }
  snprintf(ped->chave, sizeof ped->chave, "%s", chave);
  ped->serie = serie ? 1 : 0;
  if (pthread_create(&f, NULL, buscar, ped) == 0) pthread_detach(f);
  else { free(ped); trancar(); e->emVoo = 0; if (emVooTotal > 0) emVooTotal--; destrancar(); }
}

void trailerimdb_pedir(const char *imdb) {
  trailerimdb_pedir_tipo(imdb, 0);
}

const char *trailerimdb_url(const char *imdb, const char **nome) {
  Entrada *e;
  const char *r = NULL;
  char chave[32];
  if (!imdb || !imdb[0]) return NULL;
  idbase_copiar(imdb, chave, sizeof chave);
  trancar();
  e = achar(chave);
  if (e && valido(e) && e->n) {
    // O teto dos Ajustes: a maior definicao que nao passa dele; sem nenhuma
    // abaixo do teto, a menor que ha. Automatico: ver ptv_trailer_indice.
    long mem = 0;
    int k;
    tex_orcamento_info(NULL, &mem, NULL, NULL);
    // Android fraco: o teto de 720p e duro, 1080p unico nao toca.
    k = (memlog_android_fraco() ? ptv_trailer_indice_duro : ptv_trailer_indice)(
        ptv_plataforma(), mem, ajustes_trailer_qualidade(), e->alt, e->n);
    if (k >= 0) {
      r = e->url[k];
      if (nome) *nome = e->nome;
    }
  }
  destrancar();
  return r;
}

int trailerimdb_respondeu(const char *imdb) {
  Entrada *e;
  int r;
  char chave[32];
  if (!imdb || !imdb[0]) return 1;
  idbase_copiar(imdb, chave, sizeof chave);
  trancar();
  e = achar(chave);
  r = !e || e->respondeu;
  destrancar();
  return r;
}
