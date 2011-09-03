#ifndef _LOG__H_
#define _LOG__H_

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include "z3.h"
#include "LogEntry.h"

using namespace std;

/*
  Log presents the log file, which will be parsed by parser.
  Log contains one hashmap,
  the key is the address which has been visited by LogEntry,
  the value is a vector, which contains all the log entries have visited the key address.
 */
class Log
{
public:
    //Log(Z3_context ctx, const string logname = "");
    Log();
    virtual ~Log(void) {}
    
    void ParseLog(void);
    map<string, vector<LogEntry> >& GetParsedAddresses(void) { return _addresses; }

    string GetLogFileName(void) const { return _logFileName; }
    void SetLogFileName(string logname) { _logFileName = logname; }

protected:
    Z3_context _ctx;
    string _logFileName;
    map<string, vector<LogEntry> > _addresses;

    virtual vector<string> TokenizeLine(const string &line) = 0;
    void TokenByColon(const string &token, string &ret);
    void TokenOpAndAddr(const string &token, string &op, string &addr);

    void CreateLogEntryFromTokens(const vector<string> &tokens);
    virtual void AddLogEntryToMap(LogEntry &entry) = 0;
};

#endif
