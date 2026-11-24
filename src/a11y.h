// Acessibilidade (leitor de tela: VoiceView no Fire TV e TalkBack no Android TV).
// Custo zero quando nenhum leitor de tela falado esta ativo no sistema.
#ifndef NV_A11Y_H
#define NV_A11Y_H
#include <stddef.h>

// 1 quando VoiceView (Fire TV) ou TalkBack (Android TV) esta ligado.
int  a11y_ativo(void);

// Chamado pela ponte nativa (JNI) quando o leitor de tela liga ou desliga.
void a11y_definir_ativo(int ativo);

// Anuncia o elemento atualmente focado (com debounce curto e deduplicacao:
// seguro chamar a cada quadro ou mudanca de estado; so fala quando o texto muda).
void a11y_foco(const char *texto);

// Anuncia imediatamente (troca de tela, codigo de login, confirmacao de acao),
// interrompendo fala anterior.
void a11y_falar(const char *texto);

// Anuncia aviso/toast sem interromper imediatamente a navegacao (fila educada).
void a11y_aviso(const char *texto);

// Anuncia entrada em nova tela e limpa o foco memorizado para que o primeiro
// elemento focado da tela seja lido logo em seguida.
void a11y_tela(const char *titulo);

// Limpa o ultimo foco memorizado (ex.: ao fechar modal/overlay para reler o
// item que ficou focado por baixo).
void a11y_reset_foco(void);

// Converte um codigo curto ("AB39K2") em forma soletrada ("A, B, 3, 9, K, 2")
// para leitura clara pelo VoiceView/TalkBack.
void a11y_soletrar(const char *codigo, char *dst, size_t n);

// Descritores de foco por tela/camada (implementados nos respectivos modulos).
void login_a11y(char *dst, size_t n);
void perfilsel_a11y(char *dst, size_t n);
void menu_a11y(char *dst, size_t n);
void ilha_modal_a11y(char *dst, size_t n);
void teclado_a11y(char *dst, size_t n);
void atualizacao_a11y(char *dst, size_t n);
void home_a11y(char *dst, size_t n);
void detail_a11y(char *dst, size_t n);
void guia_a11y(char *dst, size_t n);
void busca_a11y(char *dst, size_t n);
void biblioteca_a11y(char *dst, size_t n);
void explorar_a11y(char *dst, size_t n);
void ajustes_a11y(char *dst, size_t n);
void streams_a11y(char *dst, size_t n);
void episodios_a11y(char *dst, size_t n);
void faixas_a11y(char *dst, size_t n);
void player_a11y(char *dst, size_t n);

#endif
