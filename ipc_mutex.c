#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "ipc_mutex.h"

int shm_fd;
sem_t *semaphore;
//  void *shared_memory;
void *shared_memory;
typedef struct
{
    /* data */
    pthread_rwlock_t rwlock;
    pthread_mutex_t mutex; 
    pthread_cond_t cond;
    int writers_count;
    int init_lock;

    unsigned char data[1024*1024];
} ipc_mutex;

unsigned char *init_ipc()
{
    shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666); // 创建共享内存

    printf("shm_fd  %d\n", shm_fd);
    ftruncate(shm_fd, sizeof(ipc_mutex)); // 设置共享内存大小
    shared_memory = mmap(NULL, sizeof(ipc_mutex), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // 映射共享内存到进程地址空间

    // sem_t *semaphore1 = &(((ipc_mutex *)shared_memory)->semaphore);

    semaphore = sem_open("/mysem", O_CREAT, 0644, 1);
    if(semaphore ==SEM_FAILED)
    {
        printf("1212\n");
    }
    return ((ipc_mutex *)shared_memory)->data;
}

int init_lock()
{
    semaphore = sem_open("/mysem", O_CREAT, 0644, 1);
    if(semaphore ==SEM_FAILED)
    {
        printf("1212\n");
    }
    sem_close(semaphore);

    (((ipc_mutex *)(shared_memory))->init_lock) = 0;
    (((ipc_mutex *)(shared_memory))->writers_count) = 0;
    if((((ipc_mutex *)(shared_memory))->init_lock) != 1)
    {
        memset(shared_memory,0,sizeof(ipc_mutex));
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
        if (pthread_rwlock_init(&(((ipc_mutex *)(shared_memory))->rwlock), &attr) != 0)
        {

            perror("pthread_rwlock_init");

            munmap(shared_memory, sizeof(ipc_mutex));

            close(shm_fd);
        }

        pthread_condattr_t attrt;

        pthread_condattr_init(&attrt);

        pthread_condattr_setpshared(&attrt, PTHREAD_PROCESS_SHARED);

        pthread_cond_init(&(((ipc_mutex *)(shared_memory))->cond), &attrt);

        pthread_mutexattr_t mutexattr; 

        pthread_mutexattr_init(&mutexattr);

        pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&(((ipc_mutex *)(shared_memory))->mutex), &mutexattr);

        (((ipc_mutex *)(shared_memory))->init_lock) = 1;
        // ((ipc_mutex *)shared_memory)->semaphore = NULL;
    }    
        return 0;
    }

int writeData(unsigned char *p,int size)
{
    printf("sem_wait\n");
    sem_wait(semaphore);
    printf("sem_wait111\n");
    while ((((ipc_mutex *)(shared_memory))->writers_count) != 0)
    {
        /* code */
        printf("->writers_count %d\n",(((ipc_mutex *)(shared_memory))->writers_count));
        printf("sslep\n");
        sleep(1000);
    }
    // export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH

    pthread_rwlock_wrlock(&(((ipc_mutex *)(shared_memory))->rwlock));
    (((ipc_mutex *)(shared_memory))->writers_count) ++;
    printf("pthread_rwlock_wrlock\n");
    memcpy(&(((ipc_mutex *)(shared_memory))->data),p,size);
    pthread_rwlock_unlock(&(((ipc_mutex *)(shared_memory))->rwlock));
    printf("pthread_rwlock_unlock\n");
    (((ipc_mutex *)(shared_memory))->writers_count) --;
    sem_post(semaphore);

    pthread_mutex_lock(&(((ipc_mutex *)(shared_memory))->mutex));
    printf("pthread_mutex_lock\n");
    pthread_cond_broadcast(&(((ipc_mutex *)(shared_memory))->cond));
    pthread_mutex_unlock(&(((ipc_mutex *)(shared_memory))->mutex));
    printf("pthread_mutex_unlock\n");

    return 0;
}

int readData(unsigned char *p,int size)
{
    pthread_mutex_lock(&(((ipc_mutex *)(shared_memory))->mutex));
    int ret = 0;
    if(!pthread_cond_wait(&(((ipc_mutex *)(shared_memory)))->cond,&(((ipc_mutex *)(shared_memory))->mutex)))
    {
        pthread_mutex_unlock(&(((ipc_mutex *)(shared_memory))->mutex));
        pthread_rwlock_rdlock(&(((ipc_mutex *)(shared_memory))->rwlock));
        printf("pthread_rwlock_rdlock\n");
        memcpy(p,&(((ipc_mutex *)(shared_memory))->data),size);
        pthread_rwlock_unlock(&(((ipc_mutex *)(shared_memory))->rwlock));
        printf("pthread_rwlock_unlock\n");
    }
    else
    {
        printf("554555545\n");
    }
    printf("ret %d\n",ret);

    return 0;
}

int unlock()
{
    pthread_rwlock_unlock(&(((ipc_mutex *)(shared_memory))->rwlock));
    return 0;
}