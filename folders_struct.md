webserv/
├── Makefile
├── webserv.conf                  # default config file
├── include/                      # all .hpp headers
├── src/                          # all .cpp implementations
│
├── www/                          # static website root
│   ├── index.html
│   ├── 404.html
│   ├── 405.html
│   ├── 500.html
│   └── images/
│
├── uploads/                      # POST uploaded files land here
│
└── cgi-bin/                      # CGI scripts
    ├── hello.py
    └── form.php
