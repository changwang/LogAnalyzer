#ifndef _ORDERUPDATETHREAD__H_
#define _ORDERUPDATETHREAD__H_

#include <QtCore/QThread>
#include <vector>
#include "LogEntry.h"

class OrderUpdateThread : public QThread
{
    Q_OBJECT

public:
    OrderUpdateThread(void) {}
    virtual ~OrderUpdateThread(void) {}

    void Sort(vector<LogEntry> entries);
    vector<LogEntry> entries(void);

signals:
    void OnFinishedSorting(void);

protected:
    void run();

private:
    vector<LogEntry> _entries;
};

#endif
