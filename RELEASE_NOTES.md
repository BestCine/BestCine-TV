## 📺 BestCine TV v1.1.14 — Smart TVs (LG webOS, Samsung Tizen e Android TV / Google TV)

Aplicativo cliente de mídia open-source em **C / SDL2 / OpenGL ES 2** para Smart TVs (60 FPS), configurável pelo usuário e baseado no projeto **[iqui27/nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy)** sob licença **GPL-3.0**.

### 🌐 Portal Oficial de Downloads
👉 **https://bestcine.github.io**

### ✨ Novidades, Melhorias e Correções desta versão (v1.1.14)
- **Correção crítica de marcação de assistidos (`✓`):** corrigida a colisão de identificadores nos títulos TMDB que fazia todos os filmes ou séries de catálogos TMDB aparecerem marcados como assistidos após assistir a apenas um título.
- **Trailers restaurados em todo o aplicativo:**
  - Extras, ficha técnica, recomendações e trailers do TMDB/YouTube desbloqueados para títulos identificados por TMDB;
  - Trailers IMDb (MP4) agora resolvem automaticamente o identificador externo via TMDB em segundo plano;
  - Trailers Apple TV (HLS) agora buscam prioritariamente na loja brasileira (`pt-BR`) antes da loja americana, encontrando títulos com nome em português.
- **Áudio em Português por padrão na escolha automática:**
  - O idioma de áudio padrão agora nasce em **Português** com a preferência de idioma ativada na escolha automática, priorizando fontes HD/4K em português sobre outros idiomas (mantendo a ordem 4K > 1080p dentro do idioma e respeitando quem alterar para inglês ou escolher fontes manualmente);
  - Migração automática única para instalações existentes que ainda estavam no padrão antigo (*Original*), preservando qualquer escolha manual já feita pelo usuário.
- **Suporte P2P nativo ativado por padrão no Android TV / Mi Box / Google TV / Fire TV:**
  - O APK de Android TV agora inclui o motor P2P nativo (`nuvio-engine` + `libtorrent`) ativado por padrão, sem exigir serviço Debrid nesses aparelhos, mantendo compatibilidade total com atualização automática e código Downloader;
  - Migração automática única para quem atualizar da versão anterior, respeitando alterações manuais futuras.
- **Correção de estabilidade visual em Samsung Tizen 4.0–5.5 (`NuvioTpk40`):**
  - Perfil visual estável aplicado automaticamente nessas TVs (tema fixo, animações reduzidas, efeitos de GPU mínimos e sem desfoque dinâmico) para eliminar cintilação/tela piscando em GPUs Mali mais antigas, com migração única para instalações existentes.
- **Verificação de atualizações à prova de erro 403:** o verificador interno agora consulta primeiro o manifesto estático em `bestcine.github.io`, usando a API do GitHub apenas como reserva.
- **Otimizações de desempenho e memória:**
  - Menor uso de RAM nos catálogos TMDB (teto reduzido para TVs de baixa memória, alocação exata por fileira e limpeza automática sob pressão de memória);
  - Rolagem na Tela Inicial mais leve a 60 FPS com cache das flags de selos por fileira;
  - Busca de trailers mais rápida com prioridade `pt-BR` e controle de concorrência (máximo de 2 threads simultâneas por provedor).

### 📦 Como instalar
- **LG webOS (Homebrew Channel):** adicione `https://github.com/BestCine/BestCine-TV/releases/latest/download/repo.json` ou instale o `.ipk` abaixo *(use a versão `highcache` apenas em TVs com 2 GB+ de RAM)*.
- **Samsung Tizen (Apps2Samsung):** escolha o `.tpk` da geração da sua TV (`Tpk40` 2018–2020 / Tizen 4.0–5.5, `Tpk60` 2021 / Tizen 6.0, `Tpk65` 2022–2023 / Tizen 6.5–7.0, `Tpk` 2024+ / Tizen 8.0–9.0) ou o `.wgt` (Tizen 5.5+).
- **Android TV / Google TV / Fire TV / Mi Box:** instale o APK universal (`arm64-v8a` + `armeabi-v7a`, com motor P2P nativo incluso).

> **Créditos (GPL-3.0):** Baseado nos projetos open-source [iqui27/nuvio-native-legacy](https://github.com/iqui27/nuvio-native-legacy) e [NuvioMedia](https://github.com/Tapframe).
