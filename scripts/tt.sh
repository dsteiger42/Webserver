#!/bin/bash

HOST="localhost"
PORT="8080"

pass() { echo -e "\e[32m[PASS]\e[0m $1"; }
fail() { echo -e "\e[31m[FAIL]\e[0m $1"; }
info() { echo -e "\e[34m[INFO]\e[0m $1"; }

check() {
    name=$1
    expected=$2
    cmd=$3

    info "Running: $name"
    # Executa o comando, pega a primeira linha da resposta HTTP
    result=$(eval "$cmd" 2>/dev/null | head -n 1)

    if echo "$result" | grep -q -E "$expected"; then
        pass "$name"
    else
        fail "$name -> got: $result"
    fi
    echo
}

echo "🔥 ULTIMATE WEBSERV TESTER (NC VERSION) 🔥"
echo "Target: $HOST:$PORT"
echo

info "=== BODY LIMIT TEST ==="

# 🚨 Teste de corpo grande usando printf + nc
check "Large body attack" "400|413" \
"(
    printf 'POST / HTTP/1.1\r\nHost: $HOST:$PORT\r\nContent-Length: 204800\r\n\r\n'
    dd if=/dev/zero bs=1k count=200 2>/dev/null
) | nc $HOST $PORT"
