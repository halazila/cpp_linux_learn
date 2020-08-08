#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

//使用有名信号量实现共享内存读写同步
typedef struct _UserInfo
{
    char name[64];
    int age;
} UserInfo;

int main(int argc, char **argv)
{
    //共享内存相关
    int shm_id;
    key_t key;
    UserInfo *pUserInfo = nullptr;
    char *shm_name = "./myshm1";
    int shm_size = 4096;
    //有名信号量
    sem_t *semr = nullptr, *semw = nullptr;
    char *semr_name = "sem_r", *semw_name = "sem_w";
    //在 /dev/shm/ 目录下生成文件 sem.sem_r
    semr = sem_open(semr_name, O_CREAT | O_RDWR, 0644, 0);
    if (semr == SEM_FAILED)
    {
        cerr << "sem_open sem_r error:" << strerror(errno) << endl;
        return -1;
    }
    //在 /dev/shm/ 目录下生成文件 sem.sem_w
    semw = sem_open(semw_name, O_CREAT | O_RDWR, 0644, 1);
    if (semw == SEM_FAILED)
    {
        cerr << "sem_open sem_w error:" << strerror(errno) << endl;
        return -1;
    }
    //
    key = ftok(shm_name, 0);
    if (key == -1)
    {
        cerr << "ftok error:" << strerror(errno) << endl;
        return -1;
    }
    shm_id = shmget(key, shm_size, IPC_CREAT | 0644);
    if (shm_id == -1)
    {
        cerr << "shmget error:" << strerror(errno) << endl;
        return -1;
    }
    pUserInfo = (UserInfo *)shmat(shm_id, nullptr, 0);
    if (pUserInfo == (UserInfo *)-1)
    {
        cerr << "shmat error:" << strerror(errno) << endl;
        return -1;
    }
    int n = 10;
    while (n--)
    {
        sem_wait(semr);
        cout << "name:" << pUserInfo->name << endl;
        cout << "age:" << pUserInfo->age << endl;
        sem_post(semw);
    }
    if (shmdt(pUserInfo) == -1)
    {
        cerr << "detach error:" << strerror(errno) << endl;
        return -1;
    }

    //删除共享内存
    if (shmctl(shm_id, IPC_RMID, nullptr) == -1)
    {
        cerr << "shmctl error:" << strerror(errno) << endl;
        return -1;
    }
    //释放信号量
    if (sem_close(semr) == -1)
    {
        cerr << "sem_close sem_r error:" << strerror(errno) << endl;
    }
    if (sem_close(semw) == -1)
    {
        cerr << "sem_close sem_w error:" << strerror(errno) << endl;
    }
    //销毁信号量，如果不销毁，/dev/shm/目录下的文件不会删除
    if (sem_unlink("sem_r") == -1)
    {
        cerr << "sem_unlink sem_r error:" << strerror(errno) << endl;
    }
    if (sem_unlink("sem_w") == -1)
    {
        cerr << "sem_unlink sem_w error:" << strerror(errno) << endl;
    }
    return 0;
}