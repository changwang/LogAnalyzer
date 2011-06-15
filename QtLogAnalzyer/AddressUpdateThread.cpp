#include "AddressUpdateThread.h"


AddressUpdateThread::AddressUpdateThread(QListWidget *address, const map<string, vector<LogEntry> > &mp)
{
    _lst_addr = address;
    _mp = mp;
}

void AddressUpdateThread::run()
{
    map<string, vector<LogEntry> >::const_iterator itr;
    for (itr = _mp.begin(); itr != _mp.end(); itr++)
    {
        itr->first.c_str();
        _lst_addr->addItem(QString::fromStdString(itr->first));
    }
}
