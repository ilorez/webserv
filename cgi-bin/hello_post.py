#!/usr/bin/env python3
import sys
import os

# Required CGI header
print("Content-Type: text/plain\r\n")

# Read Content-Length (important in CGI)
content_length = os.environ.get("CONTENT_LENGTH")
print("ENV:CONTENT_LEN IS: ", content_length);

if content_length:
    length = int(content_length)
    body = sys.stdin.read(length)
else:
    body = sys.stdin.read()  # fallback

print("Reading from stdin:\n")
print(body)
