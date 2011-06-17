#ifndef _ORDERUPDATETHREAD__H_
#define _ORDERUPDATETHREAD__H_

#include <QtCore/QThread>
#include <vector>
#include "LogEntry.h"

/*
  back-end thread, used to sort log entries by symbolic value in ascending order.
 */
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
