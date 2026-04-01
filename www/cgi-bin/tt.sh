#!/bin/bash
echo "Content-Type: text/plain"
echo

# Diretório real onde o script está
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Apagar diretório "remove" dentro do mesmo diretório do script
rm -rf "$SCRIPT_DIR/remove"

echo "Diretório 'remove' apagado com sucesso."
