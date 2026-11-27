#!/bin/bash
# Otimiza apenas as artes de demonstracao/preview no estagio Android
# (build/android/assets/art) para reduzir o tamanho do APK e o espaco ocupado
# em filesDir/res/art nas TV Boxes com pouco armazenamento interno (ex.: 8 GB).
#
# Mantem 100% intactos em resolucao/qualidade original:
#   marcas/, icones/, icones-app/, badges/, selos/, prov/, texturas/
# E preserva exatamente os mesmos nomes e extensoes de arquivo para que toda
# referencia no codigo C (Ajustes, Novidades, Colecoes) continue funcionando.
set -euo pipefail

DEST="${1:-}"
[ -n "$DEST" ] && [ -d "$DEST" ] || {
  echo "otimizar-arte.sh: uso: $0 <pasta-assets-art>" >&2
  exit 1
}

if ! command -v ffmpeg >/dev/null 2>&1; then
  echo "otimizar-arte.sh: ffmpeg ausente — mantendo artes sem redimensionamento" >&2
  exit 0
fi

ANTES_KB=$(du -sk "$DEST" | awk '{print $1}')

otimizar_jpg() {
  local f="$1" maxw="$2" q="$3"
  local tmp="${f}.opt.jpg"
  if ffmpeg -nostdin -loglevel error -i "$f" \
      -vf "scale='min(${maxw},iw)':-2:flags=lanczos" \
      -frames:v 1 -q:v "$q" -y "$tmp" 2>/dev/null; then
    if [ -s "$tmp" ]; then
      local orig novo
      orig=$(wc -c < "$f")
      novo=$(wc -c < "$tmp")
      if [ "$novo" -lt "$orig" ]; then
        mv -f "$tmp" "$f"
        return 0
      fi
    fi
  fi
  rm -f "$tmp"
}

otimizar_png() {
  local f="$1" maxw="$2"
  local tmp="${f}.opt.png"
  if ffmpeg -nostdin -loglevel error -i "$f" \
      -vf "scale='min(${maxw},iw)':-1:flags=lanczos" \
      -frames:v 1 -compression_level 9 -y "$tmp" 2>/dev/null; then
    if [ -s "$tmp" ]; then
      local orig novo
      orig=$(wc -c < "$f")
      novo=$(wc -c < "$tmp")
      if [ "$novo" -lt "$orig" ]; then
        mv -f "$tmp" "$f"
        return 0
      fi
    fi
  fi
  rm -f "$tmp"
}

export -f otimizar_jpg otimizar_png

JOBS="${NV_JOBS:-$(nproc 2>/dev/null || echo 4)}"

# 1. Backdrops de demonstracao na raiz (usados apenas em previews/fallback)
find "$DEST" -maxdepth 1 -type f -name '*.jpg' -print0 \
  | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_jpg "$1" 720 6' _ {}

# 2. Logos de demonstracao (exibidos pequenos nos previews de Ajustes/Novidades)
if [ -d "$DEST/logo" ]; then
  find "$DEST/logo" -maxdepth 1 -type f -name '*.png' -print0 \
    | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_png "$1" 320' _ {}
fi

# 3. Stills de episodios de demonstracao (miniaturas nos previews de Ajustes)
if [ -d "$DEST/ep" ]; then
  find "$DEST/ep" -maxdepth 1 -type f -name '*.jpg' -print0 \
    | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_jpg "$1" 360 7' _ {}
fi

# 4. Posters de demonstracao
if [ -d "$DEST/poster" ]; then
  find "$DEST/poster" -maxdepth 1 -type f -name '*.jpg' -print0 \
    | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_jpg "$1" 260 6' _ {}
fi

# 5. Arte editorial de demonstracao
if [ -d "$DEST/editorial" ]; then
  find "$DEST/editorial" -maxdepth 1 -type f -name '*.jpg' -print0 \
    | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_jpg "$1" 640 6' _ {}
fi

# 6. Fotos de elenco de demonstracao (miniaturas circulares nos previews)
if [ -d "$DEST/elenco" ]; then
  find "$DEST/elenco" -maxdepth 1 -type f -name '*.jpg' -print0 \
    | xargs -0 -r -P "$JOBS" -I {} bash -c 'otimizar_jpg "$1" 180 6' _ {}
fi

# Garante que nenhum temporario ficou para tras.
find "$DEST" -name '*.opt.*' -delete 2>/dev/null || true

DEPOIS_KB=$(du -sk "$DEST" | awk '{print $1}')
echo "otimizar-arte.sh: $((ANTES_KB / 1024)) MB -> $((DEPOIS_KB / 1024)) MB em $DEST" >&2
