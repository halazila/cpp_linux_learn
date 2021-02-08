#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <atomic>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    //atomic unique
    int a_key = ftok("./shm_atomic.txt", 0);
    if (a_key == -1)
    {
        perror("ftok error:");
        return -1;
    }
    int a_shm_id = shmget(a_key, sizeof(atomic_int), IPC_CREAT | 0644);
    if (a_shm_id == -1)
    {
        perror("shmget error:");
        return -1;
    }
    atomic_int *a_int = (atomic_int *)shmat(a_shm_id, nullptr, 0);
    *a_int = 0;

    //numeric unique
    int n_key = ftok("./shm_numeric.txt", 0);
    if (n_key == -1)
    {
        perror("ftok error:");
        return -1;
    }
    int n_shm_id = shmget(n_key, sizeof(int), IPC_CREAT | 0644);
    if (n_shm_id == -1)
    {
        perror("shmget error:");
        return -1;
    }
    int *n_int = (int *)shmat(n_shm_id, nullptr, 0);
    *n_int = 0;

    int pid = fork();
    if (pid) //parent
    {
        for (size_t i = 0; i < 20; i++)
        {
            int t = (*a_int)++;
            printf("parent process atomic=%d\n", t);
        }
        printf("******parent process*********\n");
        for (size_t i = 0; i < 20; i++)
        {
            int t = (*n_int)++;
            printf("parent process numeric=%d\n", t);
        }
    }
    else //child
    {
        for (size_t i = 0; i < 20; i++)
        {
            int t = (*a_int)++;
            printf("child process atomic=%d\n", t);
        }
        printf("******child process*********\n");
        for (size_t i = 0; i < 20; i++)
        {
            int t = (*n_int)++;
            printf("child process numeric=%d\n", t);
        }
    }

    //detach shm
    if (shmdt(n_int) == -1)
    {
        perror("detach error:");
        return -1;
    }
    if (shmdt(a_int) == -1)
    {
        perror("detach error:");
        return -1;
    }
    //delete shm
    if (shmctl(n_shm_id, IPC_RMID, nullptr) == -1)
    {
        perror("shmctl error");
        return -1;
    }
    if (shmctl(a_shm_id, IPC_RMID, nullptr) == -1)
    {
        perror("shmctl error");
        return -1;
    }
    return 0;
}