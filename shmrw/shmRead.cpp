#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
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
    people *p_map;
    char *name = "./myshm2";
    key = ftok(name, 0);
    if (key == -1)
    {
        cerr << "ftok error:" << strerror(errno) << endl;
        return -1;
    }
    shm_id = shmget(key, 4096, IPC_CREAT | 0644);
    if (shm_id == -1)
    {
        cerr << "shmget error:" << strerror(errno) << endl;
        return -1;
    }
    p_map = (people *)shmat(shm_id, NULL, 0);
    if ((int64_t)p_map == -1)
    {
        cerr << "shmat error:" << strerror(errno) << endl;
        return -1;
    }

    for (i = 0; i < 10; i++)
    {
        printf("name:%s\n", (*(p_map + i)).name);
        printf("age %d\n", (*(p_map + i)).age);
    }
    if (shmdt(p_map) == -1)
    {
        perror(" detach error: ");
        return -1;
    }
    //删除共享内存
    if (shmctl(shm_id, IPC_RMID, nullptr) == -1)
    {
        cerr << "shmctl error:" << strerror(errno) << endl;
        return -1;
    }

    return 0;
}