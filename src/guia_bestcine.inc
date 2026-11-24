// BestCine TV ao Vivo — grade estilo BestCine Windows (sidebar fixa de categorias
// a esquerda + grade de 5 colunas de cards grandes com logo centralizado, estrela
// de favoritos e programacao ao vivo real). Incluido por src/guia.c.

#define BC_SIDE_X      44.0f
#define BC_SIDE_Y      34.0f
#define BC_SIDE_W     344.0f
#define BC_SIDE_H    1012.0f
#define BC_CAT_TOP    124.0f
#define BC_CAT_BOT   1024.0f
#define BC_CAT_ROW     56.0f
#define BC_CAT_GAP      6.0f

#define BC_MAIN_X     416.0f
#define BC_MAIN_W    1460.0f
#define BC_TOP_Y       34.0f
#define BC_GRID_TOP   128.0f
#define BC_GRID_BOT  1004.0f
#define BC_COLS         5
#define BC_CARD_W     276.0f
#define BC_CARD_H     212.0f
#define BC_GAP_X       20.0f
#define BC_GAP_Y       18.0f

static int    bcCatSel;      // 0 = Todos, 1 = Favoritos, 2.. = cats[k]
static int    bcZona = 1;    // 0 = sidebar categorias, 1 = grade canais, 2 = botoes topo
static int    bcGridFoco;
static int    bcTopoFoco;    // 0 = Buscar, 1 = Atualizar
static float  bcRolCat, bcVelCat;
static float  bcRolGrid, bcVelGrid;
static Uint32 bcOkDesde;
static int    bcOkLongo;

static int bcGradeAtiva(void) {
  return aberta && !overlay && !gRetrato();
}

// Programacao real enviada pelo servidor BestCine no campo description
// ("🔴 Agora (12:53–13:45): ..."), ignorando o eco "Canal: <nome>".
static const char *bcDescProgramacao(const GCanal *c) {
  char eco[200];
  if (!c || !c->desc || !c->desc[0]) return NULL;
  snprintf(eco, sizeof eco, "Canal: %s", c->nome ? c->nome : "");
  if (!strcmp(c->desc, eco)) return NULL;
  return c->desc;
}

static int bcLerHora2(const char *p, int *hh, int *mm) {
  if (!p[0] || !p[1] || p[2] != ':' || !p[3] || !p[4]) return 0;
  if (p[0] < '0' || p[0] > '9' || p[1] < '0' || p[1] > '9' ||
      p[3] < '0' || p[3] > '9' || p[4] < '0' || p[4] > '9') return 0;
  *hh = (p[0] - '0') * 10 + (p[1] - '0');
  *mm = (p[3] - '0') * 10 + (p[4] - '0');
  return (*hh >= 0 && *hh <= 23 && *mm >= 0 && *mm <= 59);
}

// Extrai titulo limpo, horarios (HH:MM) e progresso (0.02..1.0) de
// "Agora (HH:MM–HH:MM): Titulo" usando horario de Brasilia (UTC-3).
static int bcExtrairEpgBarra(const char *desc, time_t agoraUtc,
                             char *titDst, size_t titCap,
                             char *iniDst, size_t iniCap,
                             char *fimDst, size_t fimCap,
                             float *progDst) {
  const char *p, *fimLinha;
  int sh = 0, sm = 0, eh = 0, em = 0;
  int iniMin, fimMin, dur, brtMin, dec;
  long long brtSec;
  size_t len;
  float prog;
  if (!desc || !desc[0] || !titDst || !titCap || !iniDst || !iniCap || !fimDst || !fimCap || !progDst)
    return 0;

  fimLinha = desc;
  while (*fimLinha && *fimLinha != '\n' && *fimLinha != '\r') fimLinha++;

  // Procura "agora" (case-insensitive) na primeira linha
  p = desc;
  while (p + 5 <= fimLinha) {
    if ((p[0] == 'a' || p[0] == 'A') &&
        (p[1] == 'g' || p[1] == 'G') &&
        (p[2] == 'o' || p[2] == 'O') &&
        (p[3] == 'r' || p[3] == 'R') &&
        (p[4] == 'a' || p[4] == 'A')) {
      p += 5;
      break;
    }
    p++;
  }
  if (p + 5 > fimLinha) return 0;

  while (p < fimLinha && *p == ' ') p++;
  if (p >= fimLinha || *p != '(') return 0;
  p++;
  while (p < fimLinha && *p == ' ') p++;
  if (p + 5 > fimLinha || !bcLerHora2(p, &sh, &sm)) return 0;
  p += 5;
  while (p < fimLinha && *p == ' ') p++;

  // Separador: '-' ASCII ou en-dash (E2 80 93) / em-dash (E2 80 94)
  if (p < fimLinha && *p == '-') {
    p++;
  } else if (p + 3 <= fimLinha &&
             (unsigned char)p[0] == 0xE2 && (unsigned char)p[1] == 0x80 &&
             ((unsigned char)p[2] == 0x93 || (unsigned char)p[2] == 0x94)) {
    p += 3;
  } else {
    return 0;
  }

  while (p < fimLinha && *p == ' ') p++;
  if (p + 5 > fimLinha || !bcLerHora2(p, &eh, &em)) return 0;
  p += 5;
  while (p < fimLinha && *p == ' ') p++;
  if (p >= fimLinha || *p != ')') return 0;
  p++;
  while (p < fimLinha && *p == ' ') p++;
  if (p >= fimLinha || *p != ':') return 0;
  p++;
  while (p < fimLinha && *p == ' ') p++;
  if (p >= fimLinha) return 0;

  len = (size_t)(fimLinha - p);
  while (len > 0 && (p[len - 1] == ' ' || p[len - 1] == '\t')) len--;
  if (!len) return 0;
  if (len >= titCap) len = titCap - 1;
  memcpy(titDst, p, len);
  titDst[len] = 0;

  iniMin = sh * 60 + sm;
  fimMin = eh * 60 + em;
  dur = (fimMin > iniMin) ? (fimMin - iniMin) : ((1440 - iniMin) + fimMin);
  if (dur <= 0 || dur > 12 * 60) return 0;

  brtSec = (long long)agoraUtc - 3LL * 3600LL;
  brtMin = (int)(((brtSec / 60LL) % 1440LL + 1440LL) % 1440LL);
  dec = (fimMin > iniMin)
        ? (brtMin - iniMin)
        : (brtMin >= iniMin ? (brtMin - iniMin) : ((1440 - iniMin) + brtMin));
  prog = (float)dec / (float)dur;
  if (prog < 0.02f) prog = 0.02f;
  if (prog > 1.0f) prog = 1.0f;

  snprintf(iniDst, iniCap, "%02d:%02d", sh, sm);
  snprintf(fimDst, fimCap, "%02d:%02d", eh, em);
  *progDst = prog;
  return 1;
}

static int bcTotalCats(void) { return 2 + nCats; }

static int bcCatContagem(int idx) {
  if (idx == 0) return nCanais;
  if (idx == 1) return nFavOrd;
  if (idx >= 2 && idx - 2 < nCats) return catN[idx - 2];
  return 0;
}

static const char *bcCatNome(int idx) {
  if (idx == 0) return i18n("Todos");
  if (idx == 1) return "\xe2\x98\x85 Favoritos";
  if (idx >= 2 && idx - 2 < nCats) return cats[idx - 2].nome;
  return "";
}

static int bcListaN(void) { return bcCatContagem(bcCatSel); }

static GCanal *bcListaCanal(int idx) {
  if (idx < 0) return NULL;
  if (bcCatSel == 0) return idx < nCanais ? &canais[idx] : NULL;
  if (bcCatSel == 1) return idx < nFavOrd ? &canais[favOrd[idx]] : NULL;
  if (bcCatSel >= 2 && bcCatSel - 2 < nCats) {
    int k = bcCatSel - 2;
    return idx < catN[k] ? &canais[catIni[k] + idx] : NULL;
  }
  return NULL;
}

static void bcAjustarFoco(void) {
  int tot = bcTotalCats(), n;
  if (bcCatSel >= tot) bcCatSel = 0;
  if (bcCatSel < 0) bcCatSel = 0;
  n = bcListaN();
  if (n <= 0) bcGridFoco = 0;
  else if (bcGridFoco >= n) bcGridFoco = n - 1;
  if (bcGridFoco < 0) bcGridFoco = 0;
}

static void bcSelecionarCat(int idx) {
  int tot = bcTotalCats();
  if (idx < 0) idx = 0;
  if (idx >= tot) idx = tot - 1;
  if (bcCatSel != idx) {
    bcCatSel = idx;
    bcGridFoco = 0;
    bcRolGrid = 0.0f;
    bcVelGrid = 0.0f;
  }
}

static void bcAlternarFavoritoAtual(void) {
  GCanal *c = bcListaCanal(bcGridFoco);
  if (!c) return;
  favAlternar(c);
  glem_aviso_curto(c->fav ? "\xe2\x98\x85 Adicionado aos Favoritos" : "Removido dos Favoritos");
  bcAjustarFoco();
}

static void bcPonteiroCatFocar(int i, int b) {
  (void)b;
  bcSelecionarCat(i);
  bcZona = 0;
}
static void bcPonteiroCatAtivar(int i, int b) {
  (void)b;
  bcSelecionarCat(i);
  bcZona = bcListaN() > 0 ? 1 : 0;
}

static void bcPonteiroTopoFocar(int i, int b) {
  (void)b;
  bcZona = 2;
  bcTopoFoco = i ? 1 : 0;
}
static void bcPonteiroTopoAtivar(int i, int b) {
  (void)b;
  bcZona = 2;
  bcTopoFoco = i ? 1 : 0;
  if (bcTopoFoco == 0) buscaAbrir();
  else { guia_recarregar(0); glem_aviso_curto("Atualizando canais BestCine…"); }
}

static void bcPonteiroCardFocar(int i, int b) {
  int n = bcListaN();
  (void)b;
  if (i < 0 || i >= n) return;
  bcZona = 1;
  bcGridFoco = i;
}
static void bcPonteiroCardAtivar(int i, int b) {
  int n = bcListaN();
  GCanal *c;
  (void)b;
  if (i < 0 || i >= n) return;
  bcZona = 1;
  bcGridFoco = i;
  c = bcListaCanal(i);
  if (c) {
    if (ponteiro_ok_longo()) bcAlternarFavoritoAtual();
    else pedirCanal(c);
  }
}

static void bcGradeAbrir(void) {
  bcOkDesde = 0;
  bcOkLongo = 0;
  bcAjustarFoco();
  bcZona = bcListaN() > 0 ? 1 : 0;
}

static void bcGradeAtualizar(float dt, Uint32 agora) {
  int tot = bcTotalCats();
  int n = bcListaN();
  bcAjustarFoco();

  if (bcOkDesde && !bcOkLongo && segurar_venceu(agora, bcOkDesde, "guia-bc")) {
    bcOkLongo = 1;
    if (bcZona == 1) bcAlternarFavoritoAtual();
  }

  // Rolagem suave da sidebar de categorias
  {
    float areaH = BC_CAT_BOT - BC_CAT_TOP;
    float passo = BC_CAT_ROW + BC_CAT_GAP;
    float totalH = (float)tot * passo;
    float alvo = (float)bcCatSel * passo - areaH * 0.42f + BC_CAT_ROW * 0.5f;
    float maxR = totalH - areaH;
    if (maxR < 0.0f) maxR = 0.0f;
    if (alvo < 0.0f) alvo = 0.0f;
    if (alvo > maxR) alvo = maxR;
    bcRolCat = anim_mola2(&bcVelCat, bcRolCat, alvo, dt, NV_MOLA_SCROLL);
  }

  // Rolagem suave da grade de 5 colunas
  {
    int linhas = (n + BC_COLS - 1) / BC_COLS;
    int lin = bcGridFoco / BC_COLS;
    float areaH = BC_GRID_BOT - BC_GRID_TOP;
    float passo = BC_CARD_H + BC_GAP_Y;
    float totalH = (float)linhas * passo;
    float alvo = (float)lin * passo - areaH * 0.30f;
    float maxR = totalH - areaH + 24.0f;
    if (maxR < 0.0f) maxR = 0.0f;
    if (alvo < 0.0f) alvo = 0.0f;
    if (alvo > maxR) alvo = maxR;
    bcRolGrid = anim_mola2(&bcVelGrid, bcRolGrid, alvo, dt, NV_MOLA_SCROLL);
  }
}

static int bcGradeEvento(const SDL_Event *e) {
  if (!bcGradeAtiva()) return 0;
  if (e->type == SDL_KEYUP) {
    SDL_Keycode k = e->key.keysym.sym;
    if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
      if (bcOkDesde && !bcOkLongo) {
        if (bcZona == 1) {
          GCanal *c = bcListaCanal(bcGridFoco);
          if (c) pedirCanal(c);
        } else if (bcZona == 0) {
          if (bcListaN() > 0) bcZona = 1;
        } else if (bcZona == 2) {
          if (bcTopoFoco == 0) buscaAbrir();
          else { guia_recarregar(0); glem_aviso_curto("Atualizando canais BestCine…"); }
        }
      }
      bcOkDesde = 0;
      bcOkLongo = 0;
    }
    return 1;
  }
  if (e->type != SDL_KEYDOWN) return 1;

  SDL_Keycode k = e->key.keysym.sym;
  if (k == SDLK_AC_BACK || k == SDLK_ESCAPE || k == SDLK_BACKSPACE ||
      k == SDLK_DELETE || e->key.keysym.scancode == NV_SCANCODE_BACK) {
    bcOkDesde = 0; bcOkLongo = 0;
    sair();
    return 1;
  }

  if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
    if (!e->key.repeat) {
      bcOkDesde = SDL_GetTicks();
      if (!bcOkDesde) bcOkDesde = 1;
      bcOkLongo = 0;
    }
    return 1;
  }

  bcAjustarFoco();
  if (bcZona == 0) {
    int tot = bcTotalCats();
    if (k == SDLK_UP && bcCatSel > 0) bcSelecionarCat(bcCatSel - 1);
    else if (k == SDLK_DOWN && bcCatSel + 1 < tot) bcSelecionarCat(bcCatSel + 1);
    else if (k == SDLK_PAGEUP) bcSelecionarCat(bcCatSel - 8);
    else if (k == SDLK_PAGEDOWN) bcSelecionarCat(bcCatSel + 8);
    else if (k == SDLK_RIGHT) {
      if (bcListaN() > 0) bcZona = 1;
      else { bcZona = 2; bcTopoFoco = 0; }
    }
    return 1;
  }

  if (bcZona == 2) {
    if (k == SDLK_LEFT) {
      if (bcTopoFoco > 0) bcTopoFoco--;
      else bcZona = 0;
    } else if (k == SDLK_RIGHT) {
      if (bcTopoFoco < 1) bcTopoFoco++;
    } else if (k == SDLK_DOWN) {
      if (bcListaN() > 0) bcZona = 1;
      else bcZona = 0;
    }
    return 1;
  }

  // bcZona == 1 (grade de canais em 5 colunas)
  {
    int n = bcListaN();
    if (n <= 0) { bcZona = 0; return 1; }
    int col = bcGridFoco % BC_COLS;
    if (k == SDLK_LEFT) {
      if (col > 0) bcGridFoco--;
      else bcZona = 0;
    } else if (k == SDLK_RIGHT) {
      if (bcGridFoco + 1 < n && col + 1 < BC_COLS) bcGridFoco++;
    } else if (k == SDLK_UP) {
      if (bcGridFoco >= BC_COLS) bcGridFoco -= BC_COLS;
      else { bcZona = 2; bcTopoFoco = 0; }
    } else if (k == SDLK_DOWN) {
      if (bcGridFoco + BC_COLS < n) bcGridFoco += BC_COLS;
      else if (bcGridFoco / BC_COLS < (n - 1) / BC_COLS) bcGridFoco = n - 1;
    } else if (k == SDLK_PAGEUP) {
      bcGridFoco -= BC_COLS * 3;
      if (bcGridFoco < 0) bcGridFoco = 0;
    } else if (k == SDLK_PAGEDOWN) {
      bcGridFoco += BC_COLS * 3;
      if (bcGridFoco >= n) bcGridFoco = n - 1;
    }
  }
  return 1;
}

static void bcGradeDesenhar(float a, Uint32 agora) {
  time_t agoraT = time(NULL);
  int tot = bcTotalCats();
  int n = bcListaN();
  int i;
  bcAjustarFoco();

  // Fundo escuro BestCine (#090C14)
  gfx_cor((GfxRect){ 0.0f, 0.0f, NV_TELA_W, NV_TELA_H }, 0.0f, 0.035f, 0.047f, 0.078f, 1.0f);

  // ---------------- SIDEBAR ESQUERDA DE CATEGORIAS ----------------
  {
    GfxRect side = { BC_SIDE_X, BC_SIDE_Y, BC_SIDE_W, BC_SIDE_H };
    gfx_cor(side, 18.0f / side.h, 0.055f, 0.075f, 0.122f, 0.96f * a);
    gfx_anel(side, 18.0f / side.h, 1.5f, 1.0f, 1.0f, 1.0f, 0.08f * a);

    // Cabecalho da sidebar
    {
      GfxRect icoBox = { side.x + 20.0f, side.y + 20.0f, 44.0f, 44.0f };
      TxtLinha tit = txt_linha(TXT_HEADLINE, "TV ao Vivo", 244, 246, 250, 255);
      char sub[64];
      TxtLinha st;
      gfx_cor(icoBox, 12.0f / icoBox.h, 0.118f, 0.420f, 1.0f, 0.22f * a);
      gfx_icone((GfxRect){ icoBox.x + 9.0f, icoBox.y + 9.0f, 26.0f, 26.0f },
                "aj_tv-minimal-play", 0.35f, 0.65f, 1.0f, a);
      txt_desenhar_alpha(tit, icoBox.x + 56.0f, icoBox.y - 2.0f, a);
      if (estado == G_BAIXANDO && nCanais == 0) snprintf(sub, sizeof sub, "Carregando canais…");
      else snprintf(sub, sizeof sub, "%d canais", nCanais);
      st = txt_linha(TXT_MINI, sub, 148, 163, 184, 255);
      txt_desenhar_alpha(st, icoBox.x + 56.0f, icoBox.y + 30.0f, a);
    }

    // Lista rolavel de categorias
    gfx_recorte(side.x + 8.0f, BC_CAT_TOP, side.w - 16.0f, BC_CAT_BOT - BC_CAT_TOP);
    for (i = 0; i < tot; i++) {
      float passo = BC_CAT_ROW + BC_CAT_GAP;
      float y = BC_CAT_TOP + (float)i * passo - bcRolCat;
      GfxRect r = { side.x + 14.0f, y, side.w - 28.0f, BC_CAT_ROW };
      int sel = (i == bcCatSel);
      int foc = (sel && bcZona == 0);
      char cnt[16];
      TxtLinha tc, tn;
      if (y + BC_CAT_ROW < BC_CAT_TOP - 8.0f) continue;
      if (y > BC_CAT_BOT + 8.0f) break;
      if (a > 0.99f)
        ponteiro_alvo_faixa(r.x, r.y, r.w, r.h, BC_CAT_TOP, BC_CAT_BOT,
                            bcPonteiroCatFocar, bcPonteiroCatAtivar, i, 0);
      if (sel) {
        gfx_cor(r, 12.0f / r.h, 0.118f, 0.420f, 1.0f, (foc ? 1.0f : 0.80f) * a);
        if (foc) gfx_anel(r, 12.0f / r.h, 2.5f, 1.0f, 1.0f, 1.0f, 0.90f * a);
      } else {
        gfx_cor(r, 12.0f / r.h, 1.0f, 1.0f, 1.0f, 0.03f * a);
      }
      snprintf(cnt, sizeof cnt, "%d", bcCatContagem(i));
      tc = txt_linha(TXT_MINI, cnt, sel ? 235 : 138, sel ? 242 : 152, sel ? 255 : 172, 255);
      txt_desenhar_alpha(tc, r.x + r.w - 16.0f - (float)tc.w, r.y + (r.h - (float)tc.h) * 0.5f, a);
      tn = txt_linha_corta(TXT_CAPTION, bcCatNome(i),
                           sel ? 255 : (i == 1 ? 250 : 210),
                           sel ? 255 : (i == 1 ? 204 : 218),
                           sel ? 255 : (i == 1 ? 21  : 230),
                           255, r.w - 38.0f - (float)tc.w);
      txt_desenhar_alpha(tn, r.x + 16.0f, r.y + (r.h - (float)tn.h) * 0.5f, a);
    }
    gfx_sem_recorte();
  }

  // ---------------- CABECALHO DA AREA DIREITA ----------------
  {
    char sub[120];
    TxtLinha tit = txt_linha_corta(TXT_ILHA_TITULO, bcCatNome(bcCatSel), 245, 247, 250, 255, 860.0f);
    TxtLinha st;
    txt_desenhar_alpha(tit, BC_MAIN_X, BC_TOP_Y + 2.0f, a);
    if (estado == G_BAIXANDO && nCanais > 0)
      snprintf(sub, sizeof sub, "%d canais disponíveis · atualizando…", n);
    else if (estado == G_BAIXANDO)
      snprintf(sub, sizeof sub, "Carregando canais BestCine…");
    else
      snprintf(sub, sizeof sub, "%d canais disponíveis", n);
    st = txt_linha(TXT_CAPTION, sub, 148, 163, 184, 255);
    txt_desenhar_alpha(st, BC_MAIN_X, BC_TOP_Y + 46.0f, a);

    // Botoes Buscar e Atualizar no canto superior direito
    {
      float bx2 = BC_MAIN_X + BC_MAIN_W - 188.0f;
      float bx1 = bx2 - 196.0f;
      GfxRect b1 = { bx1, BC_TOP_Y + 10.0f, 180.0f, 52.0f };
      GfxRect b2 = { bx2, BC_TOP_Y + 10.0f, 188.0f, 52.0f };
      int f1 = (bcZona == 2 && bcTopoFoco == 0);
      int f2 = (bcZona == 2 && bcTopoFoco == 1);
      if (a > 0.99f) {
        ponteiro_alvo_faixa(b1.x, b1.y, b1.w, b1.h, 0.0f, NV_TELA_H,
                            bcPonteiroTopoFocar, bcPonteiroTopoAtivar, 0, 0);
        ponteiro_alvo_faixa(b2.x, b2.y, b2.w, b2.h, 0.0f, NV_TELA_H,
                            bcPonteiroTopoFocar, bcPonteiroTopoAtivar, 1, 0);
      }
      gfx_cor(b1, 14.0f / b1.h, f1 ? 0.118f : 0.082f, f1 ? 0.420f : 0.106f, f1 ? 1.0f : 0.165f, a);
      gfx_anel(b1, 14.0f / b1.h, f1 ? 2.5f : 1.2f, 1.0f, 1.0f, 1.0f, (f1 ? 0.90f : 0.12f) * a);
      gfx_icone((GfxRect){ b1.x + 20.0f, b1.y + 14.0f, 24.0f, 24.0f }, "aj_search", 0.95f, 0.96f, 1.0f, a);
      { TxtLinha l = txt_linha(TXT_CAPTION, "Buscar", 245, 247, 250, 255);
        txt_desenhar_alpha(l, b1.x + 54.0f, b1.y + (b1.h - (float)l.h) * 0.5f, a); }

      gfx_cor(b2, 14.0f / b2.h, f2 ? 0.118f : 0.082f, f2 ? 0.420f : 0.106f, f2 ? 1.0f : 0.165f, a);
      gfx_anel(b2, 14.0f / b2.h, f2 ? 2.5f : 1.2f, 1.0f, 1.0f, 1.0f, (f2 ? 0.90f : 0.12f) * a);
      gfx_icone((GfxRect){ b2.x + 20.0f, b2.y + 14.0f, 24.0f, 24.0f }, "aj_rotate-cw", 0.95f, 0.96f, 1.0f, a);
      { TxtLinha l = txt_linha(TXT_CAPTION, "Atualizar", 245, 247, 250, 255);
        txt_desenhar_alpha(l, b2.x + 54.0f, b2.y + (b2.h - (float)l.h) * 0.5f, a); }
    }
  }

  // ---------------- GRADE 5 COLUNAS DE CANAIS ----------------
  gfx_recorte(BC_MAIN_X - 8.0f, BC_GRID_TOP - 8.0f, BC_MAIN_W + 16.0f, BC_GRID_BOT - BC_GRID_TOP + 16.0f);
  if (n == 0) {
    const char *msg = (estado == G_BAIXANDO)
      ? "Carregando canais BestCine…"
      : (bcCatSel == 1)
        ? "Nenhum canal favorito ainda. Segure OK sobre qualquer canal para adicionar aos Favoritos."
        : "Nenhum canal disponível nesta categoria.";
    TxtLinha l = txt_linha_corta(TXT_BODY, msg, 160, 174, 192, 255, BC_MAIN_W - 80.0f);
    txt_desenhar_alpha(l, BC_MAIN_X + (BC_MAIN_W - (float)l.w) * 0.5f, BC_GRID_TOP + 220.0f, a);
  } else {
    float passoY = BC_CARD_H + BC_GAP_Y;
    int linIni = (int)((bcRolGrid - passoY) / passoY);
    int linFim = (int)((bcRolGrid + (BC_GRID_BOT - BC_GRID_TOP) + passoY) / passoY);
    int totalLin = (n + BC_COLS - 1) / BC_COLS;
    if (linIni < 0) linIni = 0;
    if (linFim >= totalLin) linFim = totalLin - 1;

    for (int lin = linIni; lin <= linFim; lin++) {
      for (int col = 0; col < BC_COLS; col++) {
        int idx = lin * BC_COLS + col;
        GCanal *c;
        float x, y;
        GfxRect card, logoBox;
        int foc;
        const char *prog = NULL;
        EpgProg ep;
        int epgIdx;

        if (idx >= n) break;
        c = bcListaCanal(idx);
        if (!c) continue;
        x = BC_MAIN_X + (float)col * (BC_CARD_W + BC_GAP_X);
        y = BC_GRID_TOP + (float)lin * passoY - bcRolGrid;
        if (y + BC_CARD_H < BC_GRID_TOP - 10.0f || y > BC_GRID_BOT + 10.0f) continue;

        card = (GfxRect){ x, y, BC_CARD_W, BC_CARD_H };
        foc = (bcZona == 1 && idx == bcGridFoco);
        if (a > 0.99f) {
          ponteiro_alvo_faixa(card.x, card.y, card.w, card.h, BC_GRID_TOP, BC_GRID_BOT,
                              bcPonteiroCardFocar, bcPonteiroCardAtivar, idx, 0);
          ponteiro_alvo_segurar(bcPonteiroCardFocar);
        }

        // Superficie do card (azul marinho escuro BestCine)
        gfx_cor(card, 16.0f / card.h,
                foc ? 0.098f : 0.067f,
                foc ? 0.145f : 0.094f,
                foc ? 0.235f : 0.153f, a);
        // Area superior mais sutil para destaque do logo
        logoBox = (GfxRect){ x + 12.0f, y + 10.0f, BC_CARD_W - 24.0f, 102.0f };
        gfx_cor(logoBox, 12.0f / logoBox.h, 0.043f, 0.063f, 0.106f, 0.72f * a);

        // Borda de foco azul vibrante BestCine
        if (foc)
          gfx_anel(card, 16.0f / card.h, 3.5f, 0.231f, 0.510f, 0.965f, a);
        else
          gfx_anel(card, 16.0f / card.h, 1.0f, 1.0f, 1.0f, 1.0f, 0.07f * a);

        // Logo grande do canal centralizado
        guia_logo_desenhar(logoDe(c), c->nome, logoBox, 196.0f, 80.0f, 1.0f, a);

        // Estrela de favorito no canto superior direito
        {
          TxtLinha st = txt_linha(TXT_CAPTION, c->fav ? "\xe2\x98\x85" : "\xe2\x98\x86",
                                  c->fav ? 250 : 120, c->fav ? 204 : 132, c->fav ? 21 : 154, 255);
          txt_desenhar_alpha(st, x + BC_CARD_W - 30.0f, y + 10.0f, (c->fav ? 1.0f : 0.55f) * a);
        }

        // Nome do canal
        {
          TxtLinha nm = txt_linha_corta(TXT_CAPTION, c->nome, 245, 247, 252, 255, BC_CARD_W - 28.0f);
          txt_desenhar_alpha(nm, x + 14.0f, y + 119.0f, a);
        }

        // Programacao ao vivo + barra de progresso EPG (XMLTV ou servidor BestCine)
        {
          char titBuf[180], hIni[12], hFim[12];
          float epgProg = 0.0f;
          int temBarra = 0;

          epgIdx = epgDo(c);
          if (epgIdx >= 0 && epg_agora(epgIdx, agoraT - fusoDe(c), &ep) && ep.titulo && ep.titulo[0]) {
            prog = ep.titulo;
            if (ep.fim > ep.ini) {
              fmtHora(ep.ini, hIni, sizeof hIni);
              fmtHora(ep.fim, hFim, sizeof hFim);
              epgProg = anim_clamp((float)((agoraT - fusoDe(c)) - ep.ini) / (float)(ep.fim - ep.ini), 0.02f, 1.0f);
              temBarra = 1;
            }
          } else {
            const char *descBc = bcDescProgramacao(c);
            if (descBc && bcExtrairEpgBarra(descBc, agoraT, titBuf, sizeof titBuf,
                                            hIni, sizeof hIni, hFim, sizeof hFim, &epgProg)) {
              prog = titBuf;
              temBarra = 1;
            } else if (descBc) {
              const char *nl = strpbrk(descBc, "\r\n");
              if (nl) {
                size_t k = (size_t)(nl - descBc);
                if (k >= sizeof titBuf) k = sizeof titBuf - 1;
                memcpy(titBuf, descBc, k);
                titBuf[k] = 0;
                prog = titBuf;
              } else {
                prog = descBc;
              }
            } else {
              prog = NULL;
            }
          }
          if (!prog || !prog[0])
            prog = (c->cat >= 0 && c->cat < nCats) ? cats[c->cat].nome : "Ao vivo";

          if (temBarra) {
            TxtLinha pr = txt_linha_corta(TXT_MINI, prog, 186, 198, 218, 255, BC_CARD_W - 28.0f);
            TxtLinha tIni = txt_linha(TXT_MINI, hIni, 148, 163, 184, 255);
            TxtLinha tFim = txt_linha(TXT_MINI, hFim, 148, 163, 184, 255);
            float barY = y + 180.0f;
            float bx = x + 14.0f + (float)tIni.w + 8.0f;
            float rx = x + BC_CARD_W - 14.0f - (float)tFim.w;
            float bw = (rx - 8.0f) - bx;
            float bh = 5.0f;
            float by = barY + ((float)tIni.h - bh) * 0.5f;

            txt_desenhar_alpha(pr, x + 14.0f, y + 151.0f, a);
            txt_desenhar_alpha(tIni, x + 14.0f, barY, a);
            txt_desenhar_alpha(tFim, rx, barY, a);
            if (bw > 14.0f) {
              float fw = bw * epgProg;
              if (fw < bh) fw = bh;
              if (fw > bw) fw = bw;
              gfx_cor((GfxRect){ bx, by, bw, bh }, 0.5f, 1.0f, 1.0f, 1.0f, 0.16f * a);
              gfx_cor((GfxRect){ bx, by, fw, bh }, 0.5f, 0.937f, 0.267f, 0.267f, a);
            }
          } else {
            // Sem horario de EPG: pontinho vermelho AO VIVO + texto limpo
            gfx_cor((GfxRect){ x + 14.0f, y + 165.0f, 9.0f, 9.0f }, 0.5f, 0.937f, 0.267f, 0.267f, a);
            {
              TxtLinha pr = txt_linha_corta(TXT_MINI, prog, 165, 180, 202, 255, BC_CARD_W - 42.0f);
              txt_desenhar_alpha(pr, x + 30.0f, y + 158.0f, a);
            }
          }
        }
      }
    }
  }
  gfx_sem_recorte();

  // ---------------- RODAPE DE DICAS ----------------
  {
    TxtLinha dica = txt_linha(TXT_MINI,
      "OK Assistir  \xc2\xb7  Segurar OK \xe2\x98\x85 Favorito  \xc2\xb7  \xe2\x97\x80 Categorias  \xc2\xb7  \xe2\x96\xb2 Buscar / Atualizar  \xc2\xb7  Voltar Sair",
      148, 163, 184, 255);
    txt_desenhar_alpha(dica, BC_MAIN_X, 1028.0f, 0.88f * a);
  }

  (void)agora;
  buscaDesenhar(agora);
}
