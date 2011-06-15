#include <map>
#include <vector>
#include <ctime>
#include "Parser.h"
#include "Log.h"
#include "LogEntry.h"

int main()
{
    time_t startTime = time(NULL);
    Parser parser;
    Log log(parser.GetZ3Context(), "1.txt");
    log.ParseLog();

    map<string, vector<LogEntry> > mp = log.GetParsedAddresses();
    map<string, vector<LogEntry> >::iterator mitr;
    for (mitr = mp.begin(); mitr != mp.end(); mitr++)
    {
        parser.Start(&log, mitr->first, parser.DumpValue(mitr->first));
    }
    time_t endTime = time(NULL);
    cout << "Time elapsed: " << difftime(endTime, startTime) << endl;
    cout << Z3_model_to_string(parser.GetZ3Context(), parser.GetResult());
    return 0;
}
