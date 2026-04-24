#!/bin/bash

HOST=127.0.0.1
PORT=8080

GREEN="\033[0;32m"
RED="\033[0;31m"
NCOLOR="\033[0m"

function ok() {
    echo -e "${GREEN}[OK] $1${NCOLOR}"
}

function fail() {
    echo -e "${RED}[FAIL] $1${NCOLOR}"
}

function test_title() {
    echo -e "\n===== $1 ====="
}



########################################
# MULTIPLE REQUESTS SAME CONNECTION
########################################
test_title "PIPELINING TEST"

(
echo -ne "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
echo -ne "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
) | nc $HOST $PORT > response.txt

COUNT=$(grep -c "HTTP/" response.txt)
[ "$COUNT" -ge 2 ] && ok "Pipelining works" || fail "Pipelining failed"
