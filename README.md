<p align="center">
  <img src="deploy/app/icon-large.png" alt="BestCine TV" width="120">
</p>

<h1 align="center">📺 BestCine TV (v1.1.7) — Smart TVs, TV Box, Mi Box &amp; Fire TV</h1>

<p align="center">
  <strong>Versão feita especialmente para CONTROLE REMOTO (60 FPS nativo)</strong><br>
  LG webOS &nbsp;·&nbsp; Samsung Tizen &nbsp;·&nbsp; Android TV / Google TV / Mi Box / Fire TV Stick / TV Box
</p>

<p align="center">
  <a href="https://github.com/BestCine/BestCine-TV/releases/latest">
    <img src="https://img.shields.io/badge/Vers%C3%A3o_Atual-v1.1.7-1E6BFF?style=for-the-badge" alt="Versão Atual" />
  </a>
  <a href="https://github.com/BestCine/BestCine">
    <img src="https://img.shields.io/badge/Windows_PC_%26_Celular-BestCine-0078D6?style=for-the-badge" alt="App PC e Celular" />
  </a>
  <a href="https://dsc.gg/bestcine">
    <img src="https://img.shields.io/badge/Discord-Comunidade_Oficial-5865F2?style=for-the-badge&logo=discord&logoColor=white" alt="Discord BestCine" />
  </a>
</p>

---

## 🎯 QUAL É O SEU APARELHO? (CLIQUE E BAIXE DIRETO — NÃO PRECISA ABRIR "ASSETS")

> ⚠️ **NÃO SABE QUAL BAIXAR? SÓ ENCONTRE SEU APARELHO NA LISTA ABAIXO E CLIQUE NO LINK AZUL!**

### 1️⃣ Android TV / Google TV / Mi Box / Fire TV Stick / TV Box (TCL, Philips, Sony, Philco, Xiaomi, Chromecast…)
Se o seu aparelho usa **Android TV, Google TV ou Fire TV**, baixe pela Central Oficial (exige **Android 7 ou superior**):

👉 **[⬇️ BAIXAR BESTCINE TV PARA ANDROID TV / GOOGLE TV / MI BOX / FIRE TV (bestcine.github.io)](https://bestcine.github.io)**

- **APK único universal (`v1.1.6+`):** `BestCineTV-android-lite-universal.apk` (ARM 32 e 64 bits juntos, assinatura Release oficial) acompanhado de `.sha256` (`sha256sum -c BestCineTV-android-lite-universal.apk.sha256`).
- **Como instalar:** baixe pelo app **Downloader** (colando o link acima), **Send Files to TV** ou pendrive USB e abra o arquivo.
- **Como entrar na sua conta:** abra o BestCine TV, aponte a câmera do celular para o **QR Code** na TV e toque em **Autorizar TV** (não precisa digitar senha no controle!).

---

### 2️⃣ TV Samsung (Sistema Tizen — instalar via Apps2Samsung)
> ⚠️ **ATENÇÃO: NÃO olhe o ano em que você comprou a TV!**
> Olhe a **etiqueta atrás da TV** (ou em *Configurações › Suporte › Sobre esta TV*) e veja as **LETRAS DO MODELO** (ex.: `UN55AU8000` → letras **`AU`**):

| Letras do modelo da sua Samsung | Versão do Tizen | Clique para baixar |
|---|---|---|
| **`NU` / `RU` / `TU`** *(ex.: NU7100, RU7100, TU7000, TU8000)* | Tizen 4.0 – 5.5 | 👉 **[⬇️ Baixar para NU / RU / TU (.tpk)](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/BestCineTV-1.1.7-NuvioTpk40.tpk)** |
| **`AU` / `Q60A` / `Q70A` / `Q80A`** *(ex.: AU7700, AU8000)* | Tizen 6.0 | 👉 **[⬇️ Baixar para AU / linha A (.tpk)](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/BestCineTV-1.1.7-NuvioTpk60.tpk)** |
| **`BU` / `CU` / `Q60B` / `Q60C`** *(ex.: BU8000, CU7700, CU8000)* | Tizen 6.5 – 7.0 | 👉 **[⬇️ Baixar para BU / CU (.tpk)](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/BestCineTV-1.1.7-NuvioTpk65.tpk)** |
| **`DU` / `U` / `Q60D` / `S90D+`** *(ex.: DU7700, DU8000, U8500F, modelos 2024+)* | Tizen 8.0 – 9.0 | 👉 **[⬇️ Baixar para DU / U / 2024+ (.tpk)](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/BestCineTV-1.1.7-NuvioTpk.tpk)** |
| *Alternativa WebAssembly (se o `.tpk` acima der erro)* | Tizen 5.5+ | 👉 **[⬇️ Baixar versão .wgt](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/BestCineTV-1.1.7-tizen.wgt)** |

- **Como instalar:** envie o arquivo `.tpk` escolhido para a TV usando o **[Apps2Samsung](https://github.com/Apps2Samsung/Apps2Samsung)** (ele assina automaticamente para a sua TV).
- Se der erro `TypeLoadException` ao abrir, significa apenas que você baixou o `.tpk` de outra letra — basta instalar o da linha certa do `tizen=X.X` que aparece na tela.

---

### 3️⃣ TV LG (Sistema webOS 3, 4, 5, 6, 22, 23, 24+)
- **Jeito mais fácil (Homebrew Channel — atualiza sozinho):**
  1. Abra o **Homebrew Channel** na TV LG.
  2. Vá em **Settings › Repositories** e cole exatamente este link:
     ```
     https://github.com/BestCine/BestCine-TV/releases/latest/download/repo.json
     ```
  3. Procure por **BestCine TV** e clique em **Install**.
- **Ou baixe o arquivo `.ipk` direto:**
  👉 **[⬇️ BAIXAR BESTCINE TV PARA LG webOS (.ipk)](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/com.bestcine.tv_1.1.7_arm.ipk)** *(ou [versão High-Cache para TVs com 2 GB+ RAM](https://github.com/BestCine/BestCine-TV/releases/download/v1.1.7/com.bestcine.tv_1.1.7_arm-highcache.ipk))*.

---

### 4️⃣ Quer instalar no WINDOWS PC ou no CELULAR / TABLET Android?
> ⚠️ **NÃO use os arquivos desta página no PC ou no Celular!** Baixe a versão de PC e Celular aqui:
- 💻 **[⬇️ BAIXAR PARA WINDOWS PC (.msi)](https://github.com/BestCine/BestCine/releases/download/v1.1.6/BestCine-Windows-x64-1.1.6.msi)**
- 📱 **[⬇️ BAIXAR PARA CELULAR / TABLET ANDROID (.apk)](https://github.com/BestCine/BestCine/releases/download/v1.1.6/BestCine-Android-arm64-v8a-1.1.6.apk)**
- Página completa do app de PC & Celular: **[BestCine/BestCine](https://github.com/BestCine/BestCine)**

---

## ✨ O que o BestCine TV tem?
- 🔐 **Login por QR Code (sem digitar senha na TV):** escaneie com o celular e sincronize suas preferências automaticamente.
- 🎨 **Interface nativa 60 FPS para controle remoto:** navegação em grade por categorias, **★ Favoritos**, suporte a legendas e desempenho otimizado para TVs antigas e novas.
- 👨‍👩‍👧‍👦 **Até 6 perfis na mesma TV:** clique em **Trocar de perfil › `+ Novo perfil`** para cada pessoa da casa ter seu próprio *Continuar assistindo*, *Salvos*, *Favoritos* e *Trakt/Simkl* localmente.
- 🌐 **Suporte a DNS criptografado (DoH):** configurável diretamente em *Ajustes* (com fallback automático para o DNS do sistema).
- 🇧🇷 **100% Português (BR)** + **Trakt, Simkl, TMDB e Discord** integrados + **Atualização automática** direto pela TV.

---

## 🛠️ Créditos e Licença (GPL-3.0)
O **BestCine TV** é um fork independente e gratuito distribuído sob a licença **[GNU General Public License v3.0 (GPL-3.0)](LICENSE)**, baseado no excelente projeto open-source **[iqui27/nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy)** (C / SDL2 / OpenGL ES 2) e nos conceitos visuais de **[NuvioMedia / Tapframe](https://github.com/Tapframe)**.

- **Ícones:** [Lucide Icons](https://lucide.dev) (ISC).
- **Áudio DTS/AC3:** [FFmpeg](https://ffmpeg.org/) (LGPL 2.1+).
- **Aviso:** este repositório contém exclusivamente o código-fonte do aplicativo cliente / reprodutor multimídia open-source e não hospeda ou distribui conteúdo de mídia. Projeto independente, sem afiliação com `iqui27`, `NuvioMedia`, LG, Samsung ou Google.
