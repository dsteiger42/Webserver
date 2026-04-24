#!/usr/bin/env python3

print("Content-Type: text/plain")

# tentativa de injection (inofensiva)

print("X-Test: normal\r\nX-Injected: yes")

print()
print("Hello from CGI")
