#!/usr/bin/env bash
# ============================================================
#  SunnyLand Dependency Downloader (Linux/macOS)
#  Pure ASCII - no colors, no special chars
# ============================================================

set -e

# --- Config ---------------------------------------------------
USE_PROXY=0
EXTERNAL_DIR="$(dirname "$0")/../external"
mkdir -p "$EXTERNAL_DIR"

SDL3_VERSION="release-3.2.0"
SDL_IMAGE_VERSION="release-3.2.0"
SDL_MIXER_VERSION="release-3.2.0"
SDL_NET_VERSION="release-3.2.0"
SDL_TTF_VERSION="release-3.2.0"
GLM_VERSION="1.1.0"
JSON_VERSION="v3.11.3"
SPDLOG_VERSION="v1.17.0"

# --- Parse args -----------------------------------------------
for arg in "$@"; do
    case "$arg" in
        --proxy)   USE_PROXY=1 ;;
        --no-proxy) USE_PROXY=0 ;;
        -h|--help)
            echo "Usage: $0 [--proxy] [--no-proxy]"
            echo ""
            echo "  --proxy    Use gh-proxy.com for GitHub downloads"
            echo "  --no-proxy Direct download from GitHub (default)"
            exit 0
            ;;
    esac
done

# --- Proxy setup ----------------------------------------------
if [ "$USE_PROXY" -eq 1 ]; then
    BASE_URL="https://gh-proxy.com/https://github.com"
    echo "[INFO] Proxy ENABLED (gh-proxy.com)"
else
    BASE_URL="https://github.com"
    echo "[INFO] Proxy DISABLED, direct download"
fi

echo ""
echo "+------------------------------------------------+"
echo "|       SunnyLand Dependency Downloader          |"
echo "+------------------------------------------------+"
echo ""

# --- Progress function (pure ASCII) --------------------------
TOTAL=8
CURRENT=0

print_progress() {
    CURRENT=$((CURRENT + 1))
    PERCENT=$((CURRENT * 100 / TOTAL))
    FILLED=$((PERCENT * 20 / 100))
    EMPTY=$((20 - FILLED))

    BAR=""
    for ((i=0; i<FILLED; i++)); do BAR="${BAR}#"; done
    for ((i=0; i<EMPTY;  i++)); do BAR="${BAR}."; done

    echo "[${CURRENT}/${TOTAL}] [${BAR}] ${PERCENT}% - $1"
}

# --- Download function ---------------------------------------
# Usage: download <dir_name> <github_path> <tag_or_branch>
download() {
    local DIR="$EXTERNAL_DIR/$1"
    local URL="${BASE_URL}/$2.git"
    local TAG="$3"

    if [ -d "$DIR/.git" ] || [ -f "$DIR/CMakeLists.txt" ]; then
        echo "  [SKIP] $1 already exists"
        return 0
    fi

    echo "  [DL]   $1 ($TAG)"
    echo "         URL: ${URL}"

    if git clone --progress --depth 1 --branch "$TAG" "$URL" "$DIR" 2>&1 | \
       while IFS= read -r line; do
           # Parse git clone progress
           if echo "$line" | grep -q "Receiving objects"; then
               PCT=$(echo "$line" | grep -o '[0-9]*%' | head -1)
               SIZE=$(echo "$line" | grep -o '[0-9.]* [KMGiB]*' | head -1)
               echo -ne "\r         Progress: ${PCT:-?} ${SIZE:-}"
           fi
       done; then
        echo ""
        echo "  [OK]   $1 : done"
    else
        echo ""
        echo "  [ERR]  $1 : FAILED"
        echo "         Try: rm -rf $DIR && $0 $*"
        return 1
    fi
}

# ============================================================
#  Download all dependencies
# ============================================================

print_progress "SDL3"
download "SDL3" "libsdl-org/SDL" "$SDL3_VERSION" || true

print_progress "SDL_image"
download "SDL3_image" "libsdl-org/SDL_image" "$SDL_IMAGE_VERSION" || true

print_progress "SDL_mixer"
download "SDL3_mixer" "libsdl-org/SDL_mixer" "$SDL_MIXER_VERSION" || true

print_progress "SDL_net"
download "SDL3_net" "libsdl-org/SDL_net" "$SDL_NET_VERSION" || true

print_progress "SDL_ttf"
download "SDL3_ttf" "libsdl-org/SDL_ttf" "$SDL_TTF_VERSION" || true

print_progress "glm"
download "glm" "g-truc/glm" "$GLM_VERSION" || true

print_progress "json"
download "json" "nlohmann/json" "$JSON_VERSION" || true

print_progress "spdlog"
download "spdlog" "gabime/spdlog" "$SPDLOG_VERSION" || true

# ============================================================
#  Summary
# ============================================================
echo ""
echo "+------------------------------------------------+"
echo "|  [DONE] All dependencies processed             |"
echo "+------------------------------------------------+"
echo "  Directory : $EXTERNAL_DIR"

# Calculate total size
if command -v du >/dev/null 2>&1; then
    TOTAL_SIZE=$(du -sh "$EXTERNAL_DIR" 2>/dev/null | cut -f1)
    echo "  Total size: ${TOTAL_SIZE:-unknown}"
fi

echo ""
echo "Next steps:"
echo "  cmake -B build"
echo "  cmake --build build -j\$(nproc)"
echo ""
