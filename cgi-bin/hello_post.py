#!/usr/bin/env python3
import os
import sys

def main():
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = ""
    if content_length > 0:
        body = sys.stdin.read(content_length)

    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>POST received</h1>")
    print("<p><b>body:</b> " + body + "</p>")
    print("<p><b>content-length:</b> " + str(content_length) + "</p>")
    print("</body></html>")

if __name__ == "__main__":
    main()
