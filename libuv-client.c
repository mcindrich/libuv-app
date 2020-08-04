#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;

uv_write_t write_req;
uv_buf_t buf_temp;

char buffer[MAX_BUFFER_SIZE];

void on_write_cb(uv_write_t *req, int status)
{
    if (status < 0)
    {
        LOG_ERR(">> writing error occured", status);
    }
}

void on_connect_cb(uv_connect_t *req, int status)
{
    if (status < 0)
    {
        LOG_ERR(">> error connecting to the server", status);
    }
    else
    {
        char *messageForServer = (char *)req->data;
        int len = strlen(messageForServer);
        buf_temp = uv_buf_init(buffer, sizeof(buffer) * sizeof(char));

        memcpy(buf_temp.base, messageForServer, len);
        buf_temp.len = len;
        uv_write(&write_req, req->handle, &buf_temp, 1, on_write_cb);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s _message_", argv[0]);
        return -1;
    }

    uv_tcp_t tcp_clientHandle;
    uv_connect_t connect_handle;

    connect_handle.data = argv[1];

    struct sockaddr_in addr;

    loop = uv_default_loop();

    // setup handles and all that
    uv_tcp_init(loop, &tcp_clientHandle);
    uv_ip4_addr("127.0.0.1", SERVER_PORT, &addr);

    uv_tcp_connect(&connect_handle, &tcp_clientHandle, (const struct sockaddr *)&addr, on_connect_cb);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}