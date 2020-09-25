#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "uv.h"

uv_loop_t loop;
uv_buf_t wrBuf;
char chBuf[256];
uv_connect_t conn;
uv_timer_t timer;

void on_conn(uv_connect_t *req, int status);
void on_write(uv_write_t *req, int status);
void on_timer(uv_timer_t *timer);

int main(int argc, char *argv[])
{
    uv_loop_init(&loop);
    //timer
    uv_timer_init(&loop, &timer);
    //socket
    uv_tcp_t socket;
    uv_tcp_init(&loop, &socket);
    //buffer
    wrBuf = uv_buf_init(chBuf, sizeof chBuf);
    struct sockaddr_in dest;
    uv_ip4_addr("127.0.0.1", 8008, &dest);
    //connection
    int r = uv_tcp_connect(&conn, &socket, (const struct sockaddr *)&dest, on_conn);
    if (r < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(r));
        return -1;
    }
    uv_timer_start(&timer, on_timer, 5000, 2000);
    uv_run(&loop, UV_RUN_DEFAULT);
    return 0;
}

void on_conn(uv_connect_t *req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(status));
    }
    else
    {
        sprintf(wrBuf.base, "%s", "Hellowoy");
        uv_write_t *wreq = new uv_write_t;
        uv_write(wreq, req->handle, &wrBuf, 1, on_write);
    }
}

void on_write(uv_write_t *req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(status));
    }
    else
    {
        /* code */
    }

    free(req);
}

void on_timer(uv_timer_t *timer)
{
    static int nCount = 0;
    sprintf(wrBuf.base, "%s%d", "Hellowoy", ++nCount);
    uv_write_t *wreq = new uv_write_t;
    uv_write(wreq, conn.handle, &wrBuf, 1, on_write);
    if (nCount > 20)
    {
        uv_timer_stop(timer);
    }
}