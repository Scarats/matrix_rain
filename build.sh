#!/bin/sh

# AI MADE SCRIPT

# Build and install the 'matrix' program in a simple, portable way.
# - Compiles matrix.c into ./matrix
# - Installs it to ~/.local/bin by default (or a dir you pass as $1)
# - Ensures that directory is on PATH for future shells

set -e

# Always run relative to this script's directory
cd -- "$(dirname "$0")"

# Compile (use $CC if set, otherwise 'cc')
# You can override flags via CFLAGS env var when invoking the script.
: "${CC:=cc}"
: "${CFLAGS:=-O2 -std=c11 -Wall -Wextra}"
"$CC" $CFLAGS -o matrix matrix.c

# Choose install directory (override by passing one, e.g. /usr/local/bin)
PREFIX="${1:-$HOME/.local/bin}"
mkdir -p "$PREFIX"

# Install the binary (prefer 'install', fallback to cp + chmod)
if command -v install >/dev/null 2>&1; then
  install -m 755 matrix "$PREFIX/matrix"
else
  cp matrix "$PREFIX/matrix"
  chmod 755 "$PREFIX/matrix"
fi
echo "Installed to: $PREFIX/matrix"

# If not already on PATH, append to ~/.profile so future shells can find it
case ":$PATH:" in
  *":$PREFIX:"*) ;;  # already present
  *)
    printf '\n# Added by matrix build.sh\nexport PATH="%s:$PATH"\n' "$PREFIX" >> "$HOME/.profile"
    echo "Added $PREFIX to PATH in ~/.profile"
    echo "Open a new terminal or run: . ~/.profile"
    ;;
esac

echo "Done. You can now run: matrix"