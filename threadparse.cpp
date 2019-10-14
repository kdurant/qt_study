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
        if(data.mid(32, 8).toInt(Q_NULLPTR, 16) == 0)
        {
            data = adOrigData.dequeue();
            adSingleData =
        }
    }
}
