#ifndef _HELPER__H_
#define _HELPER__H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "ezlogger_headers.hpp"
#include "LogEntry.h"

using namespace std;

/* macro indicates whether it is under development,
   in order to print out some information.
*/
#define kLADebug        0

/*
  macro indicates it's under production environment,
  then redirects the log information to log file instead of console.
 */
#define kLAProduction   0

/*
  macro is used to indicate performance tuning is turn on.
 */
#define kLAPerformance  1

const int kLADupError = -1000;

/*
  prefix used for symbolic variable names
 */
const string kLAZ3VarPrefix = "LA_";

const int kLAConvertArrayLength = 5;

/*
  log file name used by Z3
 */
const char kZ3LogFileName[] = "LA_Z3_Trace.log";

/*
  log file name used by Log Analyzer
 */
const char kLALogFileName[] = "LA_Trace.log";

void PrintVectorOfString(const vector<string> &strs);

void PrintVectorOfLogEntry(const vector<LogEntry> &entries);

void PrintMap(const map<string, vector<LogEntry> > &mp);

void PrintLogEntry(const LogEntry &entry);

string IntToString(int i);

OperationType StringToOpType(const string &strOp);

string OpTypeToString(OperationType op);

/*
  get the PC CPU frequency.
 */
inline double PCPerformanceFreq()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        cout << "Failed!";
    return double(li.QuadPart) / 1000.0;
}

/*
  get the CPU counter.
 */
inline __int64 PerformanceCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
}

#endif
