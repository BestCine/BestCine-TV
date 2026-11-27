## 📺 BestCine TV v1.1.15 — Smart TVs (LG webOS, Samsung Tizen e Android TV / Google TV)

Aplicativo cliente de mídia open-source em **C / SDL2 / OpenGL ES 2** para Smart TVs (60 FPS), configurável pelo usuário e baseado no projeto **[iqui27/nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy)** sob licença **GPL-3.0**.

### 🌐 Portal Oficial de Downloads
👉 **https://bestcine.github.io**

### ✨ Novidades, Melhorias e Correções desta versão (v1.1.15)
- **Conta Nuvio opcional em *Ajustes → Rastreamento* e addons externos liberados:**
  - Nova opção **Conta Nuvio** com pareamento rápido por QR Code para sincronizar **Continuar assistindo**, **Assistidos**, **Biblioteca** e receber automaticamente seus **addons externos** (catálogos, legendas e metadados) configurados no BestCine de PC ou celular;
  - As preferências locais e a ordem personalizada das fileiras continuam salvas individualmente por TV.
- **Correção do falso aviso "Sem internet" (Samsung Tizen e Smart TVs):**
  - Adicionada sonda leve de conectividade em segundo plano antes de exibir o aviso de ausência de rede, com recuperação automática quando a conexão retorna;
  - Corrigido o tratamento de erros TLS 1.3 (código 35) em TVs mais antigas e adicionado fallback automático do DNS criptografado (DoH) para o DNS da TV quando o provedor bloqueia DNS alternativo.
- **Correção do falso aviso na primeira abertura do Samsung Tizen (`.tpk`):**
  - Eliminada a faixa preta *"Previous launch stopped at: app-context"* que podia aparecer na primeira abertura após instalar ou atualizar quando o sistema Tizen pré-carregava o aplicativo em segundo plano invisível.
- **Ordem das fileiras e TV ao Vivo na Tela Inicial:**
  - Corrigida a proteção dos catálogos embutidos (*Filmes Populares* e *Séries Populares*) para preservar sempre a ordem salva pelo usuário ao reiniciar a TV;
  - Canais de TV ao Vivo agora podem ser adicionados como fileira na Tela Inicial quando o usuário optar por isso em *Ordenar fileiras*.
- **Melhorias no Android TV / Google TV / Fire TV:**
  - Corrigido o loop de permissão de *"Instalar apps desconhecidos"* durante a auto-atualização interna do APK;
  - Otimização de artes empacotadas para reduzir o tamanho final do instalador Android TV.
- **Segurança e arquitetura:**
  - Configuração de endpoints movida para injeção exclusiva em tempo de build via variáveis de ambiente / GitHub Secrets.

### 📦 Como instalar
- **LG webOS (Homebrew Channel):** adicione `https://github.com/BestCine/BestCine-TV/releases/latest/download/repo.json` ou instale o `.ipk` abaixo *(use a versão `highcache` apenas em TVs com 2 GB+ de RAM)*.
- **Samsung Tizen (Apps2Samsung):** escolha o `.tpk` da geração da sua TV (`Tpk40` 2018–2020 / Tizen 4.0–5.5, `Tpk60` 2021 / Tizen 6.0, `Tpk65` 2022–2023 / Tizen 6.5–7.0, `Tpk` 2024+ / Tizen 8.0–9.0) ou o `.wgt` (Tizen 5.5+).
- **Android TV / Google TV / Fire TV / Mi Box:** instale o APK universal (`arm64-v8a` + `armeabi-v7a`, com motor P2P nativo incluso).

> **Créditos (GPL-3.0):** Baseado nos projetos open-source [iqui27/nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy) e [NuvioMedia](https://github.com/Tapframe).
