#!/usr/bin/env python3
import os
import sys

def main():
    method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
    query = os.environ.get("QUERY_STRING", "")
    path = os.environ.get("PATH_INFO", "")
    user_agent = os.environ.get("HTTP_USER_AGENT", "UNKNOWN")

    print("Content-Type: text/html")
    print()
    # print("\r\n\r\n")

    print("<html><body>")
    print("<h1>CGI Test OK</h1>")
    print(f"<p>Method: {method}</p>")
    print(f"<p>Query String: {query}</p>")
    print(f"<p>Path Info: {path}</p>")
    print(f"<p>User-Agent: {user_agent}</p>")
    print("</body></html>")

if __name__ == "__main__":
    main()
