#include "Helper.h"

/*
  helper method, in order to print out all strings in the list.
 */
void PrintVectorOfString(const vector<string> &strs)
{
    vector<string>::const_iterator itr;

    for (itr = strs.begin(); itr != strs.end(); itr++)
    {
        cout << *itr << endl;
    }
}

/*
  helper method, in order to print out all log entries in the vector.
 */
void PrintVectorOfLogEntry(const vector<LogEntry> &entries)
{
    vector<LogEntry>::const_iterator itr;
    for (itr = entries.begin(); itr != entries.end(); itr++)
    {
        cout << "\t";
        PrintLogEntry(*itr);
    }
}

/*
  helper method, in order to print out all pairs in the map.
 */
void PrintMap(const map<string, vector<LogEntry> > &mp)
{
    map<string, vector<LogEntry> >::const_iterator mitr;
    for (mitr = mp.begin(); mitr != mp.end(); mitr++)
    {
        cout << "At address " << mitr->first << endl;
        PrintVectorOfLogEntry(mitr->second);
        cout << endl;
    }
}

/*
  helper method, in order to print out a log entry with readable content.
 */
void PrintLogEntry(const LogEntry &entry)
{
    cout << entry.ToString() << endl;
}

/*
  helper method, convert an integer to string object.
 */
string IntToString(int i)
{
    char tmp[kLAConvertArrayLength];
    // itoa(i, tmp, 10); this is not a standard method
    sprintf(tmp, "%d", i); // this is standard
    return string(tmp);
}

/*
  helper method, convert string to OperationType
 */
OperationType StringToOpType(const string &strOp)
{
    if (strOp == "read_var")
    {
        return OperationRead;
    }
    else if (strOp == "write_var")
    {
        return OperationWrite;
    }
    else if (strOp == "acquire_lock")
    {
        return OperationAcquire;
    }
    else if (strOp == "release_lock")
    {
        return OperationRelease;
    }
    else
    {
        return OperationUndefined;
    }
}

/*
  helper method, convert OperationType to readable string
 */
string OpTypeToString(OperationType op)
{
    switch (op)
    {
    case OperationRead:
        return "read_var";
    case OperationWrite:
        return "write_var";
    case OperationAcquire:
        return "acquire_lock";
    case OperationRelease:
        return "release_lock";
    default:
        return "undefined";
    }
}
