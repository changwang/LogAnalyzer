#include <map>
#include <vector>
#include <ctime>
#include <windows.h>
#include "Parser.h"
#include "Log.h"
#include "LogEntry.h"

int main()
{
    LARGE_INTEGER li;
    __int64 CounterStart = 0;
    __int64 CounterEnd = 0;
    double PCFreq = 0.0;

    Parser parser;
    Log log(parser.GetZ3Context(), "1.txt");
    log.ParseLog();

    if (!QueryPerformanceFrequency(&li))
        cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart)/1000.0;
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;

    map<string, vector<LogEntry> > mp = log.GetParsedAddresses();
    map<string, vector<LogEntry> >::iterator mitr;
    for (mitr = mp.begin(); mitr != mp.end(); mitr++)
    {
        parser.Start(&log, mitr->first, parser.DumpValue(mitr->first));
    }

    QueryPerformanceCounter(&li);
    CounterEnd = li.QuadPart;
    cout << "Time elapsed: " << double(CounterEnd-CounterStart)/PCFreq << endl;
    //cout << Z3_model_to_string(parser.GetZ3Context(), parser.GetResult());
    return 0;
}
