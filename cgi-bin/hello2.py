#!/usr/bin/env python3
import os
import sys

def main():
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print()
    print("<html><body><h1>Hello, World!, I'm CGI output</h1></body></html>")


if __name__ == "__main__":
    main()
