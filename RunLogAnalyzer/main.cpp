#include <map>
#include <vector>
#include <ctime>
#include <windows.h>
#include "Helper.h"
#include "Parser.h"
#include "Log.h"
#include "JavaPlainLog.h"
#include "LogEntry.h"

int main()
{
    __int64 CounterStart = 0;
    __int64 CounterEnd = 0;

    Parser parser;
    Log *log = new JavaPlainLog(parser.GetZ3Context(), "1.txt");
    log->ParseLog();

    CounterStart = PerformanceCounter();

    map<string, vector<LogEntry> > *mp = log->GetParsedAddresses();
    map<string, vector<LogEntry> >::iterator mitr;
    for (mitr = mp->begin(); mitr != mp->end(); mitr++)
    {
        parser.Start(log, mitr->first, parser.DumpValue(mitr->first));
    }

    CounterEnd = PerformanceCounter();
    EZLOGGERPRINT("Time elapsed: %g ms.", (CounterEnd-CounterStart)/PCPerformanceFreq());
    EZLOGGER(Z3_model_to_string(parser.GetZ3Context(), parser.GetResult()));
    return 0;
}
