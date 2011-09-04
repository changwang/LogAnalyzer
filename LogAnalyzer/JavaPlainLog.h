#include "z3.h"
#include "log.h"

class JavaPlainLog : public Log
{
public:
    JavaPlainLog(Z3_context ctx, const string logname="");
    ~JavaPlainLog(void) {};

    void ParseLog(void);

private:

    vector<string> TokenizeLine(const string &line);

    void TokenByColon(const string &token, string &ret);

    void TokenOpAndAddr(const string &token, string &op, string &addr);

    void CreateLogEntryFromTokens(const vector<string> &tokens);

    void AddLogEntryToMap(LogEntry &entry);
};
