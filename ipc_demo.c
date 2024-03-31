#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE_OF_SHARED_MEMORY 1024
int main()
{
    // 创建或打开共享内存对象
    int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    // 设置共享内存大小
    ftruncate(shm_fd, SIZE_OF_SHARED_MEMORY);

    // 将共享内存映射到进程A的地址空间
    void *shared_memory_ptr = mmap(NULL, SIZE_OF_SHARED_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
}
// 使用shared_memory_ptr进行数据操作...