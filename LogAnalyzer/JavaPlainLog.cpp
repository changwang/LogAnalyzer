#include "JavaPlainLog.h"


JavaPlainLog::JavaPlainLog(Z3_context ctx, const string logname)
{
    this->_ctx = ctx;
    this->_logFileName = logname;
}


JavaPlainLog::~JavaPlainLog(void)
{
}
