#!/usr/bin/env python3
import socket
import sys
import time

HOST = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8080


def send_raw_request(name, raw_request):
    print(f"\n=== TEST: {name} ===")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        s.sendall(raw_request.encode())

        response = b""
        s.settimeout(2)

        try:
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                response += chunk
        except socket.timeout:
            pass

        print(response.decode(errors="ignore"))
        s.close()

    except Exception as e:
        print(f"[ERROR] {e}")


# ✅ 1. Chunked válido
valid_chunked = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nHello\r\n"
    "6\r\n World\r\n"
    "0\r\n\r\n"
)

# ❌ 2. Chunk size inválido (não hex)
invalid_hex = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "Z\r\nHello\r\n"
    "0\r\n\r\n"
)

# ❌ 3. Falta CRLF após chunk
missing_crlf = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nHello"
    "0\r\n\r\n"
)

# ❌ 4. Chunk incompleto
incomplete_chunk = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "A\r\nHello"
)

# ❌ 5. Sem chunk final
no_terminator = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nHello\r\n"
)

# ❌ 6. Chunk size maior que dados
wrong_size = (
    "POST / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "A\r\nHello\r\n"
    "0\r\n\r\n"
)


# ⚠️ 7. Chunk com delay (simula rede lenta)
def slow_chunked():
    print("\n=== TEST: slow chunked ===")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    s.sendall(b"POST / HTTP/1.1\r\n")
    time.sleep(0.5)
    s.sendall(b"Host: localhost\r\n")
    time.sleep(0.5)
    s.sendall(b"Transfer-Encoding: chunked\r\n\r\n")
    time.sleep(0.5)

    s.sendall(b"5\r\nHe")
    time.sleep(1)
    s.sendall(b"llo\r\n")
    time.sleep(1)

    s.sendall(b"0\r\n\r\n")

    response = s.recv(4096)
    print(response.decode(errors="ignore"))
    s.close()


# 🚀 Executar testes
if __name__ == "__main__":
    send_raw_request("VALID CHUNKED", valid_chunked)
    send_raw_request("INVALID HEX", invalid_hex)
    send_raw_request("MISSING CRLF", missing_crlf)
    send_raw_request("INCOMPLETE CHUNK", incomplete_chunk)
    send_raw_request("NO TERMINATOR", no_terminator)
    send_raw_request("WRONG SIZE", wrong_size)

    slow_chunked()