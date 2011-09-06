#include "LogEntry.h"
#include "Helper.h"

LogEntry::LogEntry(unsigned tid)
{
    // if tid is 0, which means it has not been initialized yet.
    _threadID = tid;
    _op = OperationUndefined;
    _address = "";
    _oldValue = "";
    _newValue = "";
    // use total order number to distinguish different log entries,
    // which may have same values.
    _totalOrderNum = 0;

    _ctx = NULL;
    _symVariable = NULL;
    _symValue = 0;  // this means it has not been solved.
}

/*
  create Z3 variable.
  the symbol variable has following format:
  LA_tid_totalOrderNum
 */
void LogEntry::DeclareSymbolVarible(Z3_context ctx)
{
#if kLADebug
    if (ctx == NULL) EZLOGGER("Z3 context cannot be NULL!");
#endif
    assert(ctx != NULL); // make sure context is not NULL

    _ctx = ctx;
    string symStr = kLAZ3VarPrefix;
    symStr += IntToString(_threadID) + "_" + IntToString(_totalOrderNum);
    Z3_symbol s = Z3_mk_string_symbol(_ctx, symStr.c_str());
    _symVariable = Z3_mk_const(_ctx, s, Z3_mk_int_sort(_ctx));

#if kLADebug
    EZLOGGER(Z3_ast_to_string(_ctx, _symVariable));
#endif
    // make sure the symbol variable value is greater than 0
    Z3_assert_cnstr(_ctx, 
        Z3_mk_gt(_ctx, _symVariable, Z3_mk_int(_ctx, 0, Z3_mk_int_sort(_ctx))));
}

/*
  each time when one Z3Context is passed in,
  should call DeclareSymbolVariable again, 
  because the context has changed.
 */
Z3_ast LogEntry::GetSymbolVarible(Z3_context ctx)
{
    if (ctx != NULL && ctx != this->_ctx)
    {
        DeclareSymbolVarible(ctx);
    }
    return _symVariable;
}

/*
  compares whether two entries are the same,
  according to their total order numbers.
 */
const bool LogEntry::operator ==(const LogEntry &other) const
{
    return _totalOrderNum == other.GetTotalOrderNum();
}

/*
  compares two entries, used by sort() in algorithms.
 */
const bool LogEntry::operator <(const LogEntry &other) const
{
    return _symValue < other.GetSybmolValue();
}

/*
  whether two entries are from the same thread.
 */
bool LogEntry::FromSameThread(const LogEntry &other) const
{
    return _threadID == other.GetThreadId();
}

/*
  displays log entry with a readable style.
 */
string LogEntry::ToString(void) const
{
    string strEntry("Thread " + IntToString(GetThreadId()));
    strEntry.append(": " + OpTypeToString(GetOpType()));
    strEntry.append(":" + GetAddress());
    strEntry.append(" oldValue:" + GetOldValue());
    strEntry.append(" newValue:" + GetNewValue());
    return strEntry;
}
