#include "common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;
struct sockaddr_in addr;

char buffer[MAX_BUFFER_SIZE];

typedef struct
{
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

uv_fs_t open_req, close_req, read_req;
uv_buf_t read_buffer;

uv_tcp_t client;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void on_client_write_cb(uv_write_t *req, int status)
{
    uv_handle_t *client_handle = (uv_handle_t *)req->data;
    if (status < 0)
    {
        LOG_ERR("unable to write to client", status);
    }
    else
    {
        LOG("client disconnected");
        uv_close(client_handle, NULL);
    }
}

void on_file_read_cb(uv_fs_t *req)
{
    uv_stream_t *client_stream = (uv_stream_t *)open_req.data;
    uv_buf_t *fr_buffer = (uv_buf_t *)read_req.data;

    if (req->result < 0)
    {
        LOG_ERR("error reading file on server", req->result);
    }
    else if (req->result == UV_EOF)
    {
        // reached EOF
        // send read file_contents
        uv_close((uv_handle_t *)client_stream, NULL);
        // free the malloc'd client from on_new_connection_cb
        free(client_stream);
        free(fr_buffer->base);
        free(fr_buffer);
    }
    else if (req->result >= 0)
    {
        uv_write_t write_handle;
        write_handle.data = client_stream;

        uv_write(&write_handle, client_stream, fr_buffer, 1, on_client_write_cb);
        // uv_fs_read(loop, &read_req, open_req.result, &read_buffer, 1, -1, on_file_read_cb);
    }
}

void on_file_open_cb(uv_fs_t *req)
{
    assert(req == &open_req);
    if (req->result < 0)
    {
        LOG_ERR(" unable to open file", req->result);
    }
    else
    {
        uv_buf_t *file_read_buffer = (uv_buf_t *)malloc(sizeof(uv_buf_t));
        char *str_buffer = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);

        read_req.data = file_read_buffer;

        *file_read_buffer = uv_buf_init(str_buffer, sizeof(char) * MAX_BUFFER_SIZE);
        uv_fs_read(loop, &read_req, open_req.result, file_read_buffer, 1, -1, on_file_read_cb);
    }
}

void on_read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0)
    {
        // assume that the filename is short for now => just an example
        char *filename = buf->base;
        open_req.data = client;

        uv_fs_open(loop, &open_req, (const char *)filename, O_RDONLY, 0, on_file_open_cb);
        return;
    }
    if (nread < 0)
    {
        if (nread != UV_EOF)
        {
            LOG_ERR("error with reading from client", nread);
        }
        uv_close((uv_handle_t *)client, NULL);
    }
    free(buf->base);
}

void on_new_connection_cb(uv_stream_t *server, int status)
{
    if (status < 0)
    {
        LOG_ERR("connection error", status);
        return;
    }

    uv_tcp_t *client_handle = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client_handle);

    if (uv_accept(server, (uv_stream_t *)client_handle) == 0)
    {
        LOG("client connected");
        uv_read_start((uv_stream_t *)client_handle, alloc_buffer, on_read_cb);
    }
    else
    {
        printf("ERROR\n");
        uv_close((uv_handle_t *)client_handle, NULL);
    }
}

int main()
{
    loop = uv_default_loop();

    uv_tcp_t tcp_server;
    uv_tcp_init(loop, &tcp_server);

    uv_ip4_addr("0.0.0.0", SERVER_PORT, &addr);

    uv_tcp_bind(&tcp_server, (const struct sockaddr *)&addr, 0);
    int ret = uv_listen((uv_stream_t *)&tcp_server, DEFAULT_BACKLOG, on_new_connection_cb);
    if (ret)
    {
        LOG_ERR("listening error", ret);
        return 1;
    }
    int finalRet = uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return finalRet;
}
