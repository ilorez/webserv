
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

//

#define MAX_CONX_QUEUE 10
#define BUF_SIZE 8192
#define BODY_SIZE_LIMIT 104857600 // 100 megabyte
#define CHUNK_SIZE 4096
#define MAX_EVENTS 1000
#define TIMEOUT_SECONDS 15		// 15s
#define EPOLL_WAIT_TIMEOUT 5000 // 5s
#define MAX_HEADER_SIZE 16384	// 16KB
#define USE_TMP_SIZE 50000		// 50kb, if the request body is more then USE_TMP_SIZE i will use tmp file to store it
#define SESSION_TTL 3600
#define BUFFER_SIZE_LIMIT 65536

#define DEF_UPLOAD_STORE  "./uploads"
#define DEF_CGI_STORE     "./cgi-bin/"
#define DEF_ROOT          "./www/"
#define DEF_ERROR_PAGE    "./www/error/error.html"
#define DEF_SERVER_NAME   "MyServer"
#define DEF_CODE_TAG      "{{CODE}}"
#define DEF_MESSAGE_TAG   "{{MESSAGE}}"
#define DEF_HOST   "127.0.0.1"
#define DEF_PORT  8080 

#endif
