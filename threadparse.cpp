#include "threadparse.h"

threadparse::threadparse()
{
    index = 0;
}

void threadparse::run()
{
    while(adOrigData.size() != 0)
    {
        data = adOrigData.head();
    }
}
