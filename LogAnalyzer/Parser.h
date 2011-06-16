#ifndef _PARSER__H_
#define _PARSER__H_

#include <vector>
#include "z3.h"
#include "LogEntry.h"
#include "Log.h"

class Parser
{
public:
    Parser(void);
    virtual ~Parser(void);

    Z3_config GetZ3Config(void) const { return _cfg; }
    Z3_context GetZ3Context(void) const { return _ctx; }

    void Start(Log *log, const string &address, const string &dump);
    string DumpValue(const string &address);

    Z3_ast CreateThreadOrderConstraint(vector<LogEntry> &entries);
    Z3_ast CreateUniquenessConstraint(vector<LogEntry> &entries);
    Z3_ast CreateCoherenceConstraint(vector<LogEntry> &entries, const string &dump);
    Z3_ast CreateCoherenceLastConstraint(vector<LogEntry> &entries, const string &dump);

    Z3_model GetResult(void);

private:
    Z3_config _cfg;
    Z3_context _ctx;
    Z3_model _model;

    Log *_log;
    string _address;

    vector<LogEntry> CreatePotentialFollowers(const LogEntry &entry, const vector<LogEntry> &entries);
    vector<LogEntry> CreateCoherenceFollowers(const LogEntry &entry, const vector<LogEntry> &pfs);
    vector<LogEntry> CreateLastSet(const vector<LogEntry> &entries);

};

#endif
