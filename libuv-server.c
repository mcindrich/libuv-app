#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;
struct sockaddr_in addr;

char message[256];

typedef struct
{
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

uv_fs_t open_req, write_req, close_req;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void on_read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0)
    {
        write_req_t *req = (write_req_t *)malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);

        uv_fs_write(loop, &write_req, open_req.result, &req->buf, 1, -1, NULL);
        return;
    }
    if (nread < 0)
    {
        if (nread != UV_EOF)
        {
            LOG_ERR(">> error with reading from client", nread);
        }
        else
        {
        }

        uv_close((uv_handle_t *)client, NULL);
    }

    free(buf->base);
}

void on_new_connection_cb(uv_stream_t *server, int status)
{
    if (status < 0)
    {
        LOG_ERR(">> connection error", status);
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        uv_read_start((uv_stream_t *)client, alloc_buffer, on_read_cb);
    }
    else
    {
        uv_close((uv_handle_t *)client, NULL);
    }
}

int main()
{
    loop = uv_default_loop();

    uv_tcp_t tcp_server;
    uv_tcp_init(loop, &tcp_server);

    uv_fs_open(loop, &open_req, TEST_FILE_NAME, O_WRONLY, 0, NULL);

    memset(message, 0, sizeof(message));

    uv_ip4_addr("0.0.0.0", SERVER_PORT, &addr);

    uv_tcp_bind(&tcp_server, (const struct sockaddr *)&addr, 0);
    int ret = uv_listen((uv_stream_t *)&tcp_server, DEFAULT_BACKLOG, on_new_connection_cb);
    if (ret)
    {
        LOG_ERR(">> listening error", ret);
        return 1;
    }
    int finalRet = uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return finalRet;
}
