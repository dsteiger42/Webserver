#!/usr/bin/env python3

import os

print("Content-Type: text/plain\n")
print("Hello from CGI!\n")

print("Method:", os.environ.get("REQUEST_METHOD"))
print("Query:", os.environ.get("QUERY_STRING"))
#raise Exception("boom")