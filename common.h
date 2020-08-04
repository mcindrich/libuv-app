#ifndef _COMMON_H
#define _COMMON_H

#define SERVER_PORT 5001
#define MAX_BUFFER_SIZE 1024
#define DEFAULT_BACKLOG 128
#define TEST_FILE_NAME "Example.txt"
#define MAX_FILE_SIZE 4096

#define LOG_ERR(msg, status) fprintf(stderr, ">> %s --> %s\n", msg, uv_strerror(status))
#define LOG(msg) fprintf(stdout, ">>> %s\n", msg)

#endif