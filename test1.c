#include <stdio.h>
#include "ipc_mutex.h"

int main()
{
    unsigned char *p = init_ipc();
    unsigned char p1[10];
    // init_lock();
    unsigned char i = 0;
    while(1)
    {
        p1[0] = i;
        writeData(p1,10);
        i++;
        printf("p %02x\n",p1[0]);
        usleep(10000);
    }
}