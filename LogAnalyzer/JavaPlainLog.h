#include "z3.h"
#include "log.h"

class JavaPlainLog : public Log
{
public:
    JavaPlainLog(Z3_context ctx, const string logname="");
    ~JavaPlainLog(void);
};

