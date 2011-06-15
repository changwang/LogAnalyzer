#ifndef _ADDRESSUPDATETHREAD__H_
#define _ADDRESSUPDATETHREAD__H_

#include <QtCore/QThread>
#include <QtGui/QListWidget>
#include <map>
#include "LogEntry.h"

class AddressUpdateThread : public QThread
{
    QListWidget *_lst_addr;
    map<string, vector<LogEntry> > _mp;

public:
    AddressUpdateThread(QListWidget *address, const map<string, vector<LogEntry> > &mp);
    ~AddressUpdateThread(void) {};

    void run();
};

#endif
