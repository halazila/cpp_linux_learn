#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>


constexpr int MAXBUFFSIZE = 128;

typedef struct _UserInfo
{
    char name[32];
    int age;
} UserInfo;

const int shm_size = 4096;
const char *shm_name = "./myshm1";

typedef struct _ShmUserInfo
{
    sem_t semr;
    sem_t semw;
    UserInfo user[MAXBUFFSIZE];
} ShmUserInfo;