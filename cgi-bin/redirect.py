#!/usr/bin/env python3
import os

def main():
    # You can change this target to test internal or external redirects
    target = "/"

    # OR test external redirect:
    #target = "https://google.com"

    print(f"Location: {target}")
    print()  # IMPORTANT: end of headers

    # Optional body (some servers ignore it on redirect)
    print("<html><body>")
    print("<h1>Redirecting...</h1>")
    print("</body></html>")

if __name__ == "__main__":
    main()
