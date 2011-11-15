#ifndef _LOGENTRY__H_
#define _LOGENTRY__H_

#include <cassert>
#include <string>
#include "z3.h"

using namespace std;

typedef enum {
    OperationRead,
    OperationWrite,
    OperationAcquire,
    OperationRelease,
    OperationUndefined
} OperationType;

/*
  LogEntry presents the memory access in the log file, it has following format:
  threadID:tID operation:address oldValue:oValue newValue:nValue
 */
class LogEntry
{
public:
    LogEntry(unsigned tid = 0);
    virtual ~LogEntry(void) {}
    
    unsigned GetThreadId(void) const { return _threadID; }

    void SetOpType(OperationType op) { this->_op = op; }
    OperationType GetOpType(void) const { return _op; }

    void SetAddress(string addr) { this->_address = addr; }
    string GetAddress(void) const { return _address; }

    void SetOldValue(string oldValue) { this->_oldValue = oldValue; }
    string GetOldValue(void) const { return _oldValue; }

    void SetNewValue(string newValue) { this->_newValue = newValue; }
    string GetNewValue(void) const { return _newValue; }

    void SetTotalOrderNum(unsigned num);
    unsigned GetTotalOrderNum(void) const { return _totalOrderNum; }

    void SetSymbolValue(unsigned symValue) { this->_symValue = symValue; }
    unsigned GetSybmolValue(void) const { return _symValue; }

    Z3_ast GetSymbolVarible(Z3_context ctx);

    const bool operator== (const LogEntry &other) const; // whether two entries are the same
    const bool operator< (const LogEntry &other) const;  // used by sort() method

    bool FromSameThread(const LogEntry &other) const;

    string ToString(void) const;

private:
    unsigned _threadID;
    OperationType _op;
    string _address;
    string _oldValue;
    string _newValue;

    Z3_context _ctx;
    Z3_ast _symVariable;
    unsigned _symValue;   // after solve the symbol, set the symbol value
    unsigned _totalOrderNum;
    
    void DeclareSymbolVarible(Z3_context ctx);

};

#endif
