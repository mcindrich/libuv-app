#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

// monitor file byte size and when drastically changed output a warning

void on_file_change_cb(uv_fs_poll_t *handle, int status, const uv_stat_t *prev, const uv_stat_t *curr)
{
    if (status < 0)
    {
        LOG_ERR("unable to read file change", status);
        return;
    }
    int diff = curr->st_size - prev->st_size;
    char *msg;
    if (diff > 0)
        msg = "increased";
    else
        msg = "lowered";
    printf("File size %s: %d B\n", msg, abs(diff));

    if (diff > MAX_FILE_SIZE)
    {
        printf("File size drastically increased --> Example.txt!\n");
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s _FileName_\n", argv[0]);
        return -1;
    }

    uv_loop_t *loop = uv_default_loop();
    uv_fs_poll_t file_poll;

    uv_loop_init(loop);

    uv_fs_poll_init(loop, &file_poll);
    uv_fs_poll_start(&file_poll, on_file_change_cb, argv[1], 30);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}