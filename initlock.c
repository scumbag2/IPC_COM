#include "ipc_mutex.h"

int main(int argc, char const *argv[])
{
    unsigned char *p = init_ipc();
    init_lock();
    return 0;
}
