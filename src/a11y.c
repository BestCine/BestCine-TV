// Ponte de acessibilidade (VoiceView / TalkBack) para o nucleo C do BestCine TV.
#include "a11y.h"
#ifdef NV_ANDROID
#include "android.h"
#endif
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

static volatile int a11yAtivo;
static unsigned a11yGeracao = 1;
static char ultimoFoco[512];
static char ultimaFala[512];
static Uint32 ultimaFalaMs;
static Uint32 telaFalaMs;

int a11y_ativo(void) {
#ifdef NV_ANDROID
  return __atomic_load_n(&a11yAtivo, __ATOMIC_ACQUIRE) != 0;
#else
  return 0;
#endif
}

unsigned a11y_geracao(void) {
  return a11yGeracao;
}

void a11y_definir_ativo(int ativo) {
  int prev = __atomic_exchange_n(&a11yAtivo, ativo ? 1 : 0, __ATOMIC_ACQ_REL);
  if (ativo && !prev) {
    ultimoFoco[0] = 0;
    ultimaFala[0] = 0;
    a11yGeracao++;
  }
}

// Limpa caracteres decorativos ("·", "•", "›", "★", emojis 4 bytes, multiplos espacos)
// para que o VoiceView/TalkBack fale frases naturais em portugues.
static void a11yLimpar(const char *src, char *dst, size_t n) {
  size_t w = 0;
  int esp = 0;
  if (!n) return;
  dst[0] = 0;
  if (!src) return;
  while (*src) {
    const unsigned char *p = (const unsigned char *)src;
    // Ignora emojis/simbolos suplementares UTF-8 de 4 bytes (ex.: 🔴, 📺, ⭐)
    if (p[0] >= 0xF0 && p[0] <= 0xF4 && p[1] && p[2] && p[3]) {
      esp = 1;
      src += 4;
      continue;
    }
    // Ignora estrelas/setas/simbolos decorativos comuns de 3 bytes (★ E2 98 85, ▶ E2 96 B6, … E2 80 A6)
    if (p[0] == 0xE2 && p[1] == 0x98 && (p[2] == 0x85 || p[2] == 0x86)) {
      esp = 1;
      src += 3;
      continue;
    }
    if (p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0xA6) {
      esp = 1;
      src += 3;
      continue;
    }
    // Separadores UTF-8 comuns na UI: "·" (C2 B7), "•" (E2 80 A2), "›" (E2 80 BA),
    // "—" (E2 80 94), "–" (E2 80 93) -> viram ", "
    if ((p[0] == 0xC2 && p[1] == 0xB7) ||
        (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0xA2 || p[2] == 0xBA || p[2] == 0x94 || p[2] == 0x93))) {
      size_t step = (p[0] == 0xC2) ? 2 : 3;
      if (w > 0 && dst[w - 1] != ',' && w + 2 < n) {
        dst[w++] = ',';
        esp = 1;
      }
      src += step;
      continue;
    }
    if (*p < 0x20) {
      esp = 1;
      src++;
      continue;
    }
    if (*p == ' ') {
      esp = 1;
      src++;
      continue;
    }
    if (esp && w > 0) {
      if (w + 2 >= n) break;
      dst[w++] = ' ';
    }
    esp = 0;
    if (w + 1 >= n) break;
    dst[w++] = (char)*p;
    src++;
  }
  while (w > 0 && (dst[w - 1] == ' ' || dst[w - 1] == ',')) w--;
  dst[w] = 0;
}

void a11y_reset_foco(void) {
  ultimoFoco[0] = 0;
  a11yGeracao++;
}

void a11y_foco(const char *texto) {
  char limpo[512];
  if (!a11y_ativo() || !texto || !texto[0]) return;
  a11yLimpar(texto, limpo, sizeof limpo);
  if (!limpo[0]) return;
  if (!strcmp(ultimoFoco, limpo)) return;
  snprintf(ultimoFoco, sizeof ultimoFoco, "%s", limpo);
#ifdef NV_ANDROID
  {
    Uint32 agora = SDL_GetTicks();
    int interromper = (telaFalaMs && (agora - telaFalaMs) < 900u) ? 0 : 1;
    int atraso = interromper ? 90 : 180;
    android_a11y_anunciar(limpo, interromper, atraso);
  }
#endif
}

void a11y_falar(const char *texto) {
  char limpo[512];
  Uint32 agora;
  if (!a11y_ativo() || !texto || !texto[0]) return;
  a11yLimpar(texto, limpo, sizeof limpo);
  if (!limpo[0]) return;
  agora = SDL_GetTicks();
  if (!strcmp(ultimaFala, limpo) && (agora - ultimaFalaMs) < 500u) return;
  snprintf(ultimaFala, sizeof ultimaFala, "%s", limpo);
  ultimaFalaMs = agora;
#ifdef NV_ANDROID
  android_a11y_anunciar(limpo, 1, 0);
#endif
}

void a11y_aviso(const char *texto) {
  char limpo[512];
  Uint32 agora;
  if (!a11y_ativo() || !texto || !texto[0]) return;
  a11yLimpar(texto, limpo, sizeof limpo);
  if (!limpo[0]) return;
  agora = SDL_GetTicks();
  if (!strcmp(ultimaFala, limpo) && (agora - ultimaFalaMs) < 800u) return;
  snprintf(ultimaFala, sizeof ultimaFala, "%s", limpo);
  ultimaFalaMs = agora;
#ifdef NV_ANDROID
  android_a11y_anunciar(limpo, 0, 0);
#endif
}

void a11y_tela(const char *titulo) {
  if (!a11y_ativo() || !titulo || !titulo[0]) return;
  ultimoFoco[0] = 0;
  a11yGeracao++;
  telaFalaMs = SDL_GetTicks();
  a11y_falar(titulo);
}

void a11y_soletrar(const char *codigo, char *dst, size_t n) {
  size_t w = 0;
  int primeiro = 1;
  if (!n) return;
  dst[0] = 0;
  if (!codigo) return;
  for (; *codigo; codigo++) {
    unsigned char c = (unsigned char)*codigo;
    if (c <= ' ' || c == '-') continue;
    if (!primeiro) {
      if (w + 3 >= n) break;
      dst[w++] = ',';
      dst[w++] = ' ';
    }
    if (w + 1 >= n) break;
    dst[w++] = (char)c;
    primeiro = 0;
  }
  dst[w] = 0;
}
