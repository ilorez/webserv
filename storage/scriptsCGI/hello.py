#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html\r")
print("\r")
print("<h1>Hello from CGI</h1>")
print("<p>REQUEST_METHOD: " + os.environ.get("REQUEST_METHOD", "NOT SET") + "</p>")
print("<p>QUERY_STRING: " + os.environ.get("QUERY_STRING", "NOT SET") + "</p>")
