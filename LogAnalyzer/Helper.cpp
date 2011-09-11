#include "Helper.h"

/*
  helper method, in order to print out all strings in the list.
 */
void PrintVectorOfString(const vector<string> &strs)
{
    vector<string>::const_iterator itr;

    for (itr = strs.begin(); itr != strs.end(); itr++)
    {
#if kLADebug
        EZLOGGER(*itr);
#endif
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
#if kLADebug
        EZLOGGER("\t");
        PrintLogEntry(*itr);
#endif
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
#if kLADebug
        EZLOGGERPRINT("At address %s", mitr->first);
        PrintVectorOfLogEntry(mitr->second);
#endif
    }
}

/*
  helper method, in order to print out a log entry with readable format.
 */
void PrintLogEntry(const LogEntry &entry)
{
#if kLADebug
    EZLOGGER(entry.ToString());
#endif
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
  it seems like there are different ways to depict operations.
 */
OperationType StringToOpType(const string &strOp)
{
    if (strOp == "read_var" || strOp == "read")
    {
        return OperationRead;
    }
    else if (strOp == "write_var" || strOp == "write")
    {
        return OperationWrite;
    }
    else if (strOp == "acquire_lock" || strOp == "acquire" || strOp == "acquire_var")
    {
        return OperationAcquire;
    }
    else if (strOp == "release_lock" || strOp == "release" || strOp == "release_var")
    {
        return OperationRelease;
    }
    else
    {
        EZLOGGER("Unable to interpret operation type!", strOp);
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
        EZLOGGER("Unable to translate operation type!", op);
        return "undefined";
    }
}
