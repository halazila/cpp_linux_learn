#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <uv.h>

uv_loop_t loop;
uv_tcp_t tcpServer;
struct sockaddr_in addr;

void on_connect(uv_stream_t *server, int status);
void on_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf);
void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

int main(int argc, char *argv[])
{
    uv_loop_init(&loop);
    uv_tcp_init(&loop, &tcpServer);
    uv_ip4_addr("0.0.0.0", 8008, &addr);
    uv_tcp_bind(&tcpServer, (const struct sockaddr *)&addr, 0);
    uv_listen((uv_stream_t *)&tcpServer, 128, on_connect);

    uv_run(&loop, UV_RUN_DEFAULT);

    return 0;
}

void on_connect(uv_stream_t *server, int status)
{
    uv_tcp_t *client = new uv_tcp_t;
    if (status < 0)
    {
        fprintf(stderr, "Error on Listening: %s.\n", uv_strerror(status));
    }

    uv_tcp_init(&loop, client);
    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        int r = uv_read_start((uv_stream_t *)client, on_alloc, on_read);
        if (r < 0)
        {
            fprintf(stderr, "Error on reading client stream: %s.\n", uv_strerror(r));
        }
    }
    else
    {
        uv_close((uv_handle_t *)client, nullptr);
    }
}

void on_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    buf->len = size;
    buf->base = (char *)malloc(size);
}

void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0)
    {
        fprintf(stderr, "Error on reading client stream: %s.\n", uv_strerror(nread));
        uv_close((uv_handle_t *)stream, nullptr);
    }
    else
    {
        std::cout << buf->base << std::endl;
    }
}