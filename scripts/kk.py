#!/usr/bin/env python3


import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 8080))

s.sendall(b"POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nHELLO\r\n")
s.shutdown(socket.SHUT_WR)

s.settimeout(5)
try:
    response = s.recv(4096)
    if response == b"":
        print("BUG CONFIRMADO: servidor fechou a ligação sem responder")
        print("Esperado: HTTP/1.1 400 Bad Request")
    else:
        print("Resposta:", response.decode())
except socket.timeout:
    print("TIMEOUT — servidor não respondeu")
finally:
    s.close()
