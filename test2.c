#include <stdio.h>
#include "ipc_mutex.h"


int main()
{
    unsigned char *p = init_ipc();
    // init_lock();
    unsigned char p1[10];
    unsigned char i = 0;
    while(1)
    {
        readData(p1,10);
        printf("p1[0]  %d\n",p1[0]);
        // unlock();
        i++;
        // printf("i %d\n",i);
        // sleep(1);
    }
}