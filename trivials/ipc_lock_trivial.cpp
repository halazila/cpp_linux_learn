#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

//pthread in share
struct ShareMutex
{
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
};

//file lock
struct FileLockMutex
{
    int fd;
    struct flock f_lock;
    FileLockMutex() : fd(-1)
    {
    }
    ~FileLockMutex()
    {
        if (fd > 0)
            close(fd);
    }
    bool init(char *file)
    {
        fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        return fd < 0 ? false : true;
    }
    void readLock()
    {
        f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = 0;
        fcntl(fd, F_SETLKW, &f_lock);
    }
    void writeLock()
    {
        f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = 0;
        fcntl(fd, F_SETLKW, &f_lock);
    }
    void unlock()
    {
        f_lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &f_lock);
    }
};

void ShareMutexFunc()
{
    int i;
    ShareMutex *shmtx;
    pid_t pid;
    //shared-mutex
    shmtx = (ShareMutex *)mmap(NULL, sizeof(*shmtx), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pthread_mutexattr_init(&shmtx->mutexattr);
    pthread_mutexattr_setpshared(&shmtx->mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shmtx->mutex, &shmtx->mutexattr);

    //shared operate memory
    int *num;
    num = (int *)mmap(NULL, sizeof(*num), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *num = 0;

    pid = fork();
    if (pid == 0) //child
    {
        for (int i = 0; i < 10; i++)
        {
            sleep(1);
            pthread_mutex_lock(&shmtx->mutex);
            (*num)++;
            // if (i == 5)//exit without unlock, parent process will never lock success
            //     goto exit_label;
            pthread_mutex_unlock(&shmtx->mutex);
            printf("child------num++ %d\n", *num);
        }
    }
    else //parent
    {
        for (int i = 0; i < 10; i++)
        {
            sleep(1);
            pthread_mutex_lock(&shmtx->mutex);
            *num += 2;
            pthread_mutex_unlock(&shmtx->mutex);
            printf("parent-----num+2 %d\n", *num);
        }
    }
exit_label:
    pthread_mutexattr_destroy(&shmtx->mutexattr);
    pthread_mutex_destroy(&shmtx->mutex);
    munmap(shmtx, sizeof(*shmtx));
    munmap(num, sizeof(*num));
}

void FileLockFunc()
{
    FileLockMutex fmutex;
    if (!fmutex.init("fmutex"))
    {
        printf("init fmutex error: %s", strerror(errno));
        return;
    }
    //shared operate memory
    int *num;
    num = (int *)mmap(NULL, sizeof(*num), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *num = 0;
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            sleep(1);
            fmutex.writeLock();
            (*num)++;
            printf("child------num++ %d\n", *num);
            fmutex.unlock();
        }
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            sleep(1);
            fmutex.writeLock();
            (*num) += 2;
            printf("parent-----num+2 %d\n", *num);
            fmutex.unlock();
        }
    }
    munmap(num, sizeof(*num));
}

int main(int argc, char *argv[])
{
    // ShareMutexFunc();
    FileLockFunc();
    return 0;
}