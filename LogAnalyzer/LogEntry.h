#ifndef _LOGENTRY__H_
#define _LOGENTRY__H_

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
    virtual ~LogEntry() {}
    
    unsigned GetThreadId() const { return _threadID; }

    void SetOpType(OperationType op) { this->_op = op; }
    OperationType GetOpType() const { return _op; }

    void SetAddress(string addr) { this->_address = addr; }
    string GetAddress() const { return _address; }

    void SetOldValue(string oldValue) { this->_oldValue = oldValue; }
    string GetOldValue() const { return _oldValue; }

    void SetNewValue(string newValue) { this->_newValue = newValue; }
    string GetNewValue() const { return _newValue; }

    void SetTotalOrderNum(unsigned num) { this->_totalOrderNum = num; }
    unsigned GetTotalOrderNum() const { return _totalOrderNum; }

    Z3_ast GetSymbolVarible(Z3_context ctx);

    void SetSymbolValue(unsigned symValue) { this->_symValue = symValue; }
    unsigned GetSybmolValue() const { return _symValue; }

    const bool operator ==(const LogEntry &other) const; // whether two entries are the same
    const bool operator <(const LogEntry &other) const;  // used by sort() method

    string ToString() const;

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
