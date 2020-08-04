#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define FILENAME "CMakeLists.txt"
#define BUFFER_MAX_SIZE 64

uv_fs_t fs_writeHandle, fs_readHandle, fs_openHandle, fs_closeHandle;
uv_buf_t buf_ioHandle;
uv_loop_t *loop;

char buffer[BUFFER_MAX_SIZE];
char printBuffer[BUFFER_MAX_SIZE];

void fs_write_cb(uv_fs_t *req)
{
}

void fs_read_cb(uv_fs_t *req)
{
    if (req->result < 0)
    {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    }
    else if (req->result == 0)
    {
        uv_fs_close(loop, &fs_closeHandle, fs_openHandle.result, NULL);
    }
    else
    {
        memset(printBuffer, 0, sizeof(printBuffer));
        memcpy(printBuffer, buffer, sizeof(buffer));
        fprintf(stdout, "%s", printBuffer);
        memset(buffer, 0, sizeof(buffer));
        uv_fs_read(loop, &fs_readHandle, fs_openHandle.result, &buf_ioHandle, 1, -1, fs_read_cb);
    }
}

void fs_open_cb(uv_fs_t *req)
{
    assert(req == &fs_openHandle);

    if (req->result >= 0)
    {
        buf_ioHandle = uv_buf_init(buffer, sizeof(buffer));
        uv_fs_read(loop, &fs_readHandle, fs_openHandle.result, &buf_ioHandle, 1, -1, fs_read_cb);
    }
    else
    {
        fprintf(stderr, "Error reading from a file : %s\n", uv_strerror(req->result));
    }
}

int main()
{
    loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    uv_loop_init(loop);
    // place to setup handles and events that need to be processed

    uv_fs_open(loop, &fs_openHandle, FILENAME, O_RDONLY, 0, fs_open_cb);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    free(loop);
    return 0;
}