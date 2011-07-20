#include <algorithm>
#include "OrderUpdateThread.h"

void OrderUpdateThread::run()
{
    sort(_entries.begin(), _entries.end());
    emit OnFinishedSorting();
}

vector<LogEntry> OrderUpdateThread::entries(void)
{
    return _entries;
}

void OrderUpdateThread::Sort(vector<LogEntry> entries)
{
    if (!isRunning())
    {
        _entries = entries;
        start();
    }
}
