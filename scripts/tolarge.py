#!/usr/bin/env python3

import socket
import sys

# ─── Config ─────────────────────────────────────────────
HOST = "127.0.0.1"
PORT = 8080
TIMEOUT = 5.0

# ─── Cores ──────────────────────────────────────────────
R  = "\033[0;31m"
G  = "\033[0;32m"
Y  = "\033[0;33m"
W  = "\033[1;37m"
DIM = "\033[2m"
RST = "\033[0m"
BOLD = "\033[1m"

# ─── Helpers ────────────────────────────────────────────

def send_raw(request_bytes):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(TIMEOUT)
        s.connect((HOST, PORT))
        s.sendall(request_bytes)

        data = b""
        try:
            while True:
                chunk = s.recv(8192)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            pass

        s.close()
        return data
    except Exception:
        return None

def send_request(method, path, headers=None, body=b""):
    h = {"Host": f"{HOST}:{PORT}", "Connection": "close"}
    if headers:
        h.update(headers)
    if body:
        h["Content-Length"] = str(len(body))

    req = f"{method} {path} HTTP/1.1\r\n"
    for k, v in h.items():
        req += f"{k}: {v}\r\n"
    req += "\r\n"

    raw = req.encode() + body
    resp = send_raw(raw)

    if resp is None:
        return None

    try:
        return int(resp.split(b" ")[1])
    except Exception:
        return 0

# ─── TESTE ──────────────────────────────────────────────

def test_max_body_size():
    print(f"\n{W}{BOLD}Teste: Body > MAX_BODY_SIZE{RST}")

    big_body = b"A" * 819255555

    code = send_request(
        "POST",
        "/upload/test_large.txt",
        headers={"Content-Type": "application/octet-stream"},
        body=big_body
    )

    if code in (413, 400, 403):
        print(f"  {G}✓ PASS{RST} → status={code}")
    else:
        print(f"  {R}✗ FAIL{RST}")
        print(f"    {DIM}body=8192 bytes → status={code} (esperado 413){RST}")
        print(f"    {Y}Possível problema:{RST}")
        print(f"      - servidor não valida Content-Length cedo")
        print(f"      - ou bloqueia ao ler o body")
        print(f"      - ou não envia resposta antes de fechar socket")

# ─── MAIN ───────────────────────────────────────────────

if __name__ == "__main__":
    if len(sys.argv) >= 2:
        HOST = sys.argv[1]
    if len(sys.argv) >= 3:
        PORT = int(sys.argv[2])

    print(f"{DIM}A testar {HOST}:{PORT}...{RST}")

    test_max_body_size()
