<p align="center">
  <img src="deploy/app/icon-large.png" alt="BestCine TV" width="120">
</p>

<h1 align="center">BestCine TV</h1>

<p align="center">
  <strong>Aplicativo nativo em C / SDL2 / OpenGL ES 2 para Smart TVs</strong><br>
  LG webOS 3+ &nbsp;·&nbsp; Samsung Tizen 4.0–9 &nbsp;·&nbsp; Android TV / Google TV 7+
</p>

<p align="center">
  <a href="https://github.com/BestCine/BestCine-TV/releases/latest"><strong>Baixar última versão</strong></a>
  &nbsp;·&nbsp; <a href="#instalação">Guia de instalação</a>
  &nbsp;·&nbsp; <a href="#recursos-do-bestcine-tv">Recursos</a>
  &nbsp;·&nbsp; <a href="https://github.com/BestCine/BestCine">App PC &amp; Celular</a>
  &nbsp;·&nbsp; <a href="#créditos-e-licença">Créditos (GPL-3.0)</a>
</p>

---

O **BestCine TV** é o cliente nativo para Smart TVs do ecossistema **BestCine**, baseado no excelente projeto open-source [**nuvio-native-legacy**](https://github.com/iqui27/nuvio-native-legacy) criado por [**iqui27**](https://github.com/iqui27).

Escrito em **C nativo sobre SDL2 e OpenGL ES 2**, ele conversa diretamente com o pipeline de vídeo da TV, entregando navegação fluida a **60 FPS** mesmo em TVs antigas (como LG webOS 3/4 e Samsung Tizen 4.0 de 2018) sem o peso de um navegador web por baixo.

> **Procurando a versão para Windows PC ou Android Celular?**
> Acesse o repositório complementar: **[BestCine/BestCine](https://github.com/BestCine/BestCine)**.

---

## Plataformas suportadas

| Plataforma | Modelos / Versões | Formato do pacote | Status |
|---|---|---|---|
| **LG webOS** | webOS 3.x, 4.x, 5.x, 6.x, 22, 23, 24+ | `.ipk` (Homebrew Channel) | Nativo (C / SDL2 / GLES2) |
| **Samsung Tizen (2018–2020)** | Tizen 4.0 – 5.5 (`NU / RU / TU`) | `BestCineTV-1.0.1-NuvioTpk40.tpk` | Nativo (C / OpenGL ES) |
| **Samsung Tizen (2021)** | Tizen 6.0 (`AU / Q60A / Q70A / Q80A`) | `BestCineTV-1.0.1-NuvioTpk60.tpk` | Nativo (C / OpenGL ES) |
| **Samsung Tizen (2022–2023)** | Tizen 6.5 – 7.0 (`BU / CU / Q60B / Q60C`) | `BestCineTV-1.0.1-NuvioTpk65.tpk` | Nativo (C / OpenGL ES) |
| **Samsung Tizen (2024+)** | Tizen 8.0 – 9.0 (`DU / Q60D / S90D+`) | `BestCineTV-1.0.1-NuvioTpk.tpk` | Nativo (C / OpenGL ES) |
| **Samsung Tizen (WebAssembly)** | Tizen 5.5+ (alternativo) | `BestCineTV-1.0.1-tizen.wgt` | Alternativo (AVPlay) |
| **Android TV / Google TV / Fire TV / TV Box** | Android 7.0+ (`armv7` / `arm64`) | `BestCineTV-1.0.1-android-lite-*.apk` | Nativo (C / SDL2 / MediaCodec) |

---

## Recursos do BestCine TV

### Conta BestCine & Login por QR Code
- **Login rápido por QR Code** direto na tela da TV (sem digitar e-mail ou senha no controle remoto): basta escanear o QR Code com o celular ou acessar `https://example.invalid/tv` e digitar o código de 6 dígitos exibido na tela.
- **Configuração 100% automática**: ao aprovar no celular, sua conta, seu plano e todos os catálogos (filmes, séries e canais ao vivo) do BestCine são sincronizados automaticamente.
- **Seção Conta BestCine nos Ajustes**: exibe usuário, e-mail, plano atual/validade, botão de atualização imediata dos catálogos e saída segura da conta.
- **Sincronização opcional com Trakt e Simkl** por perfil (histórico, *Continuar assistindo* e lista de salvos entre dispositivos).

### DNS Próprio Anti-Bloqueio (DoH)
- Em **Ajustes › Conta e serviços › DNS Anti-Bloqueio (DoH)**, o app resolve domínios diretamente via DNS criptografado:
  - **Cloudflare DoH (`1.1.1.1`)** — ativado por padrão contra bloqueios de operadoras.
  - **Google DoH (`8.8.8.8`)** — alternativa criptografada.
  - **DNS do sistema** — usa o DNS configurado no roteador/TV.
- Compatível tanto com **libcurl moderno (7.62+)** quanto com **TVs antigas (LG webOS 3/4/5 com libcurl 7.53.1)** através de resolução DoH JSON em cache + `CURLOPT_RESOLVE`.

### Reprodução & Interface
- **60 FPS nativo** com suporte a **HDR10**, **Dolby Vision** (MP4 / webOS), **Dolby Atmos** passthrough e conversão de **DTS/TrueHD para AC3 5.1 / AAC** em TVs LG e Samsung.
- **TV ao Vivo integrada** com guia de programação (EPG), categorias, troca rápida de canais e proxy local de TS/HLS.
- **Perfis com PIN**, controle infantil, busca unificada, trailers automáticos, suporte a controle Magic Remote (LG) e D-Pad completo.
- **Legendas avançadas** com renderização ASS/SSA via `libass`, legenda dupla, sincronia automática e personalização completa de fonte/cor/tamanho.

---

## Instalação

### LG webOS (Recomendado: Homebrew Channel)

A forma mais fácil é instalar pelo **Homebrew Channel** (com atualizações automáticas direto na TV):

1. Abra o **Homebrew Channel** na sua TV LG.
2. Vá em **Settings › Repositories** e adicione a URL abaixo:
   ```
   https://github.com/BestCine/BestCine-TV/releases/latest/download/repo.json
   ```
3. Procure por **BestCine TV** na lista e clique em **Install**.

Também é possível baixar o arquivo `.ipk` diretamente na página de [Releases](https://github.com/BestCine/BestCine-TV/releases/latest):
- `com.bestcine.tv_X.Y.Z_arm.ipk` — pacote padrão (ajusta o cache de texturas conforme a RAM da TV).
- `com.bestcine.tv_X.Y.Z_arm-highcache.ipk` — cache de 300 MB para TVs com 2 GB de RAM ou mais.

Para instalação via Modo Desenvolvedor (`ares-install`), consulte [INSTALL.md](INSTALL.md).

---

### Samsung Tizen (`.tpk` nativo ou `.wgt`)

Os pacotes Samsung são distribuídos sem assinatura de distribuidor para funcionarem em qualquer TV. Instale usando o [**Apps2Samsung**](https://github.com/Apps2Samsung/Apps2Samsung), que assina o pacote automaticamente para o DUID da sua TV.

> **ATENÇÃO — Não olhe o ano em que você comprou a TV!**
> No Brasil, muitos modelos Samsung (como a **AU8000**, linha 2021) continuaram sendo vendidos em 2022 e 2023. Escolha sempre pela **versão do Tizen** que aparece no Apps2Samsung ou pela **letra do modelo** da sua TV:

| Versão do Tizen | Letras do modelo (Exemplos) | Linha | Arquivo recomendado |
|---|---|---|---|
| **Tizen 4.0 – 5.5** | `NU` / `RU` / `TU` *(ex.: TU7000, TU8000)* | 2018 – 2020 | `BestCineTV-1.0.1-NuvioTpk40.tpk` |
| **Tizen 6.0** | `AU` / `Q60A` / `Q70A` / `Q80A` *(ex.: AU7700, AU8000)* | 2021 | `BestCineTV-1.0.1-NuvioTpk60.tpk` |
| **Tizen 6.5 – 7.0** | `BU` / `CU` / `Q60B` / `Q60C` *(ex.: BU8000, CU7700)* | 2022 – 2023 | `BestCineTV-1.0.1-NuvioTpk65.tpk` |
| **Tizen 8.0 – 9.0** | `DU` / `Q60D` / `S90D+` *(ex.: DU7700, DU8000)* | 2024+ | `BestCineTV-1.0.1-NuvioTpk.tpk` |

> Se aparecer o erro `TypeLoadException` ao abrir, significa apenas que o `.tpk` instalado é de outra versão do Tizen — basta instalar o `.tpk` correspondente ao `tizen=X.X` exibido na tela.
> Em TVs Tizen 5.5+, o pacote WebAssembly (`BestCineTV-1.0.1-tizen.wgt`) também pode ser usado como alternativa.

---

### Android TV / Google TV / Fire TV Stick / TV Box

1. Baixe o `.apk` na página de [Releases](https://github.com/BestCine/BestCine-TV/releases/latest):
   - **`BestCineTV-1.0.1-android-lite-armv7.apk`** — Recomendado para **Fire TV Stick (Lite, HD, 4K)**, **Mi TV Stick** e **TV Boxes 32-bit**.
   - **`BestCineTV-1.0.1-android-lite-arm64.apk`** — Recomendado para **Android TV / Google TV modernas (64-bit)**, Chromecast 4K, Shield e Fire TV 4K Max 2ª geração.
2. Instale na TV usando **Downloader**, **Send Files to TV** ou um pendrive USB.
3. Abra o **BestCine TV** e escaneie o QR Code com o celular para entrar na sua conta.

---

## Compilação (Desenvolvedores)

Este repositório não exige credenciais privadas para compilar — toda configuração de autenticação acontece em tempo de execução pelo login QR Code do usuário.

- **LG webOS (`.ipk`)**: `bash tools/arm.sh --ipk --build` (requer Docker + imagem `tools/Dockerfile`).
- **Samsung Tizen nativo (`.tpk`)**: `bash tools/tpk.sh` ou pelo GitHub Actions (`Actions › Build BestCine TV (Samsung Tizen TPK)`).
- **Samsung Tizen WASM (`.wgt`)**: `bash tools/tizen.sh && bash tools/tizen-wgt.sh` (requer Emscripten SDK).
- **Android TV (`.apk`)**: `bash tools/android.sh` (requer JDK 17 + Android SDK/NDK 27).

---

## Créditos e Licença

Este projeto é um fork independente distribuído sob a licença **[GNU General Public License v3.0 (GPL-3.0)](LICENSE)**.

### Créditos principais
- **[iqui27 / nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy)** — Todo o mérito pela engenharia do motor nativo em C/SDL2/GLES2, pipelines de vídeo para LG webOS, Samsung Tizen (`.tpk` e `.wgt`) e Android TV, conversão DTS/Dolby Vision, interface 60 FPS e documentação técnica pertence ao autor original **iqui27** e aos contribuidores do `nuvio-native-legacy`.
- **[NuvioMedia / Tapframe](https://github.com/Tapframe)** — Pelos conceitos originais de interface, ícones GPLv3 e arquitetura de ecossistema de onde o projeto upstream se originou.
- **[Lucide Icons](https://lucide.dev)** — Ícones de ajustes sob licença ISC (`deploy/app/art/icones/lucide/`).
- **[FFmpeg](https://ffmpeg.org/)** — Bibliotecas utilizadas nas builds com suporte a conversão DTS sob licença **LGPL 2.1 ou posterior**.

### Aviso de não afiliação
O **BestCine TV** é um fork independente mantido pela equipe BestCine. **Não é afiliado, endossado ou mantido oficialmente por `iqui27`, `NuvioMedia`, `Tapframe`, LG, Samsung ou Google.** Todas as marcas registradas e artes de metadados pertencem aos seus respectivos proprietários.
