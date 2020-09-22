#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <map>
#include <set>
#include <string.h>
#include <uv.h>

void on_open(uv_fs_t *req);
void on_close(uv_fs_t *req);
void on_read(uv_fs_t *req);
void on_write(uv_fs_t *req);

uv_loop_t loop = *uv_default_loop();
uv_fs_t reqOpen, reqRead, reqClose, reqWrite;
uv_buf_t rdBuf, wrBuf;
char rdBuffer[256], wrBuffer[256];
int nTotalRead = 0;

int main(int argc, char *argv[])
{
    uv_loop_init(&loop);
    rdBuf = uv_buf_init(rdBuffer, sizeof rdBuffer);
    wrBuf = uv_buf_init(wrBuffer, sizeof wrBuf);

    uv_fs_open(&loop, &reqRead, "./tt.txt", O_RDWR, 0, on_open);
    uv_run(&loop, UV_RUN_DEFAULT);
    uv_fs_req_cleanup(&reqRead);
    uv_fs_req_cleanup(&reqOpen);
    uv_fs_req_cleanup(&reqClose);
    uv_fs_req_cleanup(&reqWrite);
    return 0;
}

void on_open(uv_fs_t *req)
{
    std::cout << "on file open: " << req->result << ", " << uv_strerror(req->result) << ", file: " << req->file << std::endl;
    if (req->result >= 0)
    {
        uv_fs_read(&loop, &reqRead, req->result, &rdBuf, 1, -1, on_read); //req->result is the file descriptor
    }
}

void on_close(uv_fs_t *req)
{
    std::cout << "on file close: " << req->result << ", " << uv_strerror(req->result) << ", file: " << req->file << std::endl;
}

void on_read(uv_fs_t *req)
{
    std::cout << "on file read: " << req->result << ", " << uv_strerror(req->result) << ", file: " << req->file << std::endl;
    if (req->result == 0)
    {
        //sync
        // uv_fs_close(&loop, &reqClose, req->file, nullptr);
        //async
        uv_fs_close(&loop, &reqClose, req->file, on_close);
    }
    else if (req->result > 0)
    {
        nTotalRead += req->result;
        std::cout << rdBuf.base << std::endl;
        static int count = 0;
        sprintf(wrBuffer, "write %d-th times\n", ++count);
        if (count < 100)
        {
            wrBuf.len = strlen(wrBuffer);
            wrBuf.base = wrBuffer;
            uv_fs_write(&loop, &reqWrite, req->file, &wrBuf, 1, -1, on_write);
        }
        else
        {
            uv_fs_close(&loop, &reqClose, req->file, on_close);
        }
    }
}

void on_write(uv_fs_t *req)
{
    std::cout << "on file write: " << req->result << ", " << uv_strerror(req->result) << ", file: " << req->file << std::endl;
    uv_fs_read(&loop, &reqRead, req->file, &rdBuf, 1, nTotalRead, on_read);
}