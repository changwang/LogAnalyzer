#include "JavaPlainLog.h"
#include "Helper.h"

const char *kJavaLogEntryTokenSeparator = " ";
const char *kJavaLogEntryKeyValueSeparator = ":";

JavaPlainLog::JavaPlainLog(Z3_context ctx, const string logname)
{
    this->_ctx = ctx;
    this->_logFileName = logname;
}

/*
  by default, words in each line is separated by space,
  tokenizes it into a string vector.
 */
vector<string> JavaPlainLog::TokenizeLine(const string &line)
{
    vector<string> tokens;
    string token;

    // duplicates a null-terminated string, because I want to use strtok(),
    // cannot apply string object to it.
    char *c_tok = strdup(line.c_str());
    if (NULL == c_tok)
    {
        cout << "Can't duplicate string, Exiting..." << endl;
        exit(kLADupError);
    }

    // make sure the string is separated by spaces
    assert(strstr(c_tok, kJavaLogEntryTokenSeparator) != NULL);

    char *p = strtok(c_tok, kJavaLogEntryTokenSeparator);
    while (p)
    {
        token = string(p);
        tokens.push_back(token);
        p = strtok(NULL, kJavaLogEntryTokenSeparator);
    }
    // created it, release it
    free(c_tok);
    return tokens;
}

/*
  each token created by TokenizeLine() is seperated by colon,
  string before colon let's call it key,
  string after colon is the value.
  I want to get the value, and save it to ret.
 */
void JavaPlainLog::TokenByColon(const string &token, string &ret)
{
    char *c_tok = strdup(token.c_str());
    if (NULL == c_tok)
    {
        cout << "Can't duplicate token, Exiting..." << endl;
        exit(kLADupError);
    }

    // make sure the token is separated by colon
    assert(strstr(c_tok, kJavaLogEntryKeyValueSeparator) != NULL);

    // this p is the key
    char *p = strtok(c_tok, kJavaLogEntryKeyValueSeparator);

    p = strtok(NULL, kJavaLogEntryKeyValueSeparator);
    ret = string(p);    // this p is the value
    free(c_tok);
}

/*
  to handle a special case,
  because second token consists of operation type and address value,
  I want both of them.
  save operation type to op,
  save address value to addr.
 */
void JavaPlainLog::TokenOpAndAddr(const string &token, string &op, string &addr)
{
    char *c_tok = strdup(token.c_str());
    if (NULL == c_tok)
    {
        cout << "Can't duplicate token, Exiting..." << endl;
        exit(kLADupError);
    }

    // make sure the token is separated by colon
    assert(strstr(c_tok, kJavaLogEntryKeyValueSeparator) != NULL);

    char *p = strtok(c_tok, kJavaLogEntryKeyValueSeparator);
    op = string(p);     // first one is operation type
        
    p = strtok(NULL, kJavaLogEntryKeyValueSeparator);
    addr = string(p);   // second is address value
    
    free(c_tok);
}

/*
  this is the method to really create LogEntry object from parsed tokens.
 */
void JavaPlainLog::CreateLogEntryFromTokens(const vector<string> &tokens)
{
    // if there is nothing in the list, ignore it.
    if (tokens.empty()) return;

    vector<string>::const_iterator itr;
    unsigned tokIdx = 0;    // remember which token is being handled

    string threadID, strOpType, addr, oldVal, newVal;
    OperationType opType;

    for (itr = tokens.begin(); itr != tokens.end(); itr++)
    {
        switch (tokIdx)
        {
        case 0:
            TokenByColon(*itr, threadID);
            break;
        case 1:
            TokenOpAndAddr(*itr, strOpType, addr);
            opType = StringToOpType(strOpType);
            break;
        case 2:
            TokenByColon(*itr, oldVal);
            break;
        case 3:
            TokenByColon(*itr, newVal);
            break;
        }
        tokIdx++;
    }

    /*
      if the list only contains two elements,
      which means this LogEntry is about lock.
      by default, if thread acquired lock, old value is 0, new value is 1.
      if thread released lock, old value is 1, new value is 0.
     */
    if (2 == tokIdx)
    {
        if (opType == OperationAcquire)
        {
            oldVal = "0";
            newVal = "1";
        }
        else if (opType == OperationRelease)
        {
            oldVal = "1";
            newVal = "0";
        }
    }

    // create an instance of LogEntry, and save corresponding values to it.
    LogEntry entry(atoi(threadID.c_str()));
    entry.SetOpType(opType);
    entry.SetAddress(addr);
    entry.SetOldValue(oldVal);
    entry.SetNewValue(newVal);

#if kLADebug
    PrintLogEntry(entry);
#endif

    AddLogEntryToMap(entry);
}

/*
  put the log entry into the right place.
 */
void JavaPlainLog::AddLogEntryToMap(LogEntry &entry)
{
    map<string, vector<LogEntry> >::iterator mitr;
    // find out whether the give address is in the map.
    mitr = _addresses.find(entry.GetAddress());

    vector<LogEntry> entries;
    if (_addresses.end() != mitr)
    {
        // if so, get the list of log entries.
        entries = _addresses[entry.GetAddress()];
    }

    entry.SetTotalOrderNum(entries.size() + 1);

    // save the log entry at the end of list
    entries.push_back(entry);
    // put the list back to the map
    _addresses[entry.GetAddress()] = entries;
}

/*
  Core method, retrieve each line from log file,
  create LogEntry object based on the content.
 */
void JavaPlainLog::ParseLog(void)
{
    string line;
    vector<string> tokens;
    ifstream _logFile;
    
    // log file name should not be null or empty string
    assert(_logFileName.compare("") != 0);
    
    _logFile.open(_logFileName);
    if (!_addresses.empty()) _addresses.clear();
    
    if (_logFile.is_open())
    {
        while (_logFile.good())
        {
            getline(_logFile, line);
            if (line.compare("") == 0) continue;    // ignore empty line
            tokens = TokenizeLine(line);
#if kLADebug
            PrintVectorOfString(tokens);
#endif
            CreateLogEntryFromTokens(tokens);
        }
#if kLADebug
        PrintMap(_addresses);
#endif
    }
    _logFile.close();
}
