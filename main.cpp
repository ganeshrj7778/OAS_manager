#include "Manager/OASManager.h"

int main()
{
    OASManager oasManager;

    oasManager.init();

    oasManager.thread_join();

    return 0;
}