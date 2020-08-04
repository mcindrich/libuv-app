#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

int counter = 0;
uv_timer_t timer_req;
uv_idle_t idle_req;

int someCondition(uv_timer_t *handle)
{
    return 1;
}

void timer_cb(uv_timer_t *handle)
{
    if (counter == 100)
    {
        uv_timer_stop(handle);
        uv_idle_stop(&idle_req);
    }
    if (someCondition(handle))
    {
        ++counter;
    }
}

void idle_cb(uv_idle_t *handle)
{
    int percent = counter / 10;
    system("clear");
    printf("[");
    for (int i = 1; i <= 9; i++)
    {
        if (i <= percent)
            printf("-");
        else
            printf(" ");
    }
    printf("] -> [%d]\n", counter);
}

int main()
{
    uv_loop_t *loop = uv_default_loop();
    uv_loop_init(loop);

    uv_timer_init(loop, &timer_req);
    uv_idle_init(loop, &idle_req);

    uv_idle_start(&idle_req, idle_cb);
    uv_timer_start(&timer_req, timer_cb, 1000, 20);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}