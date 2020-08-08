#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;

typedef struct
{
    char name[4];
    int age;
} people;

int main(int argc, char **argv)
{
    int shm_id, i;
    key_t key;
    char temp;
    char name[32] = "./myshm2";
    people *p_map;
    //文件必须存在，否则创建失败
    key = ftok(name, 0);
    if (key == -1)
    {
        cerr << "ftok error" << strerror(errno) << endl;
        return 1;
    }
    shm_id = shmget(key, 4096, IPC_CREAT | 0644);
    if (shm_id == -1)
    {
        cerr << "shmget error:" << strerror(errno) << endl;
        return -1;
    }
    p_map = (people *)shmat(shm_id, nullptr, 0);
    if ((int64_t)p_map == -1)
    {
        cerr << "shmat error:" << strerror(errno) << endl;
        return -1;
    }

    temp = 'a' - 1;
    for (int i = 0; i < 20; i++)
    {
        temp += 1;
        memcpy((*(p_map + i)).name, &temp, 1);
        (*(p_map + i)).age = 20 + i;
    }
    if (shmdt(p_map) == -1)
    {
        cerr << "detach error" << endl;
        return 1;
    }
    return 0;
}