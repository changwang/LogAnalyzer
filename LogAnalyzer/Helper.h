#ifndef _HELPER__H_
#define _HELPER__H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "LogEntry.h"

using namespace std;

/* Macro indicates whether it is under development,
   in order to print out some information
*/

const int kLAShowLog = 0;

const int kLADupError = -1000;

const string kLAZ3VarPrefix = "LA_";

const int kLAConvertArrayLength = 5;

void PrintVectorOfString(const vector<string> &strs);

void PrintVectorOfLogEntry(const vector<LogEntry> &entries);

void PrintMap(const map<string, vector<LogEntry> > &mp);

void PrintLogEntry(const LogEntry &entry);

string IntToString(const int i);

OperationType StringToOpType(const string &strOp);

string OpTypeToString(const OperationType op);

#endif
