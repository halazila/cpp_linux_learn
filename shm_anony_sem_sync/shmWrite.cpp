#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "coheader.h"

using namespace std;

int main(int argc, char **argv)
{
    //共享内存
    int shm_id;
    key_t key;
    ShmUserInfo *pShmUser;

    key = ftok(shm_name, 0);
    if (key == -1)
    {
        perror("ftok error:");
        return -1;
    }
    shm_id = shmget(key, shm_size, IPC_CREAT | 0644);
    if (shm_id == -1)
    {
        perror("shmget error:");
        return -1;
    }
    pShmUser = (ShmUserInfo *)shmat(shm_id, nullptr, 0);
    if (pShmUser == (ShmUserInfo *)-1)
    {
        perror("shmat error:");
        return -1;
    }
    //初始化信号量
    sem_t *semr = &(pShmUser->semr);
    if (sem_init(semr, 2, 0) == -1)
    {
        perror("sem_init semr error:");
    }
    sem_t *semw = &(pShmUser->semw);
    if (sem_init(semw, 2, 1) == -1)
    {
        perror("sem_init semw error:");
    }
    //同步写数据
    UserInfo *pUser = pShmUser->user;
    int n = 10;
    while (n--)
    {
        sem_wait(semw);
        cout << "input name:";
        cin >> pUser->name;
        pUser->age = 20 - n;
        sem_post(semr);
    }
    //destroy sem
    if (sem_destroy(semr) == -1)
    {
        perror("sem_destroy semr error");
    }
    if (sem_destroy(semw) == -1)
    {
        perror("sem_destroy semw error");
    }
    //detach shm
    if (shmdt(pShmUser) == -1)
    {
        perror("detach error:");
        return -1;
    }
    return 0;
}