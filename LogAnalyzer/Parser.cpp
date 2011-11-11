#include <algorithm>
#include "Parser.h"
#include "Helper.h"

static int AstCount = 0;

Parser::Parser(void)
{
    _cfg = Z3_mk_config();
    Z3_set_param_value(_cfg, "MODEL", "true");
#if kLADebug
    Z3_set_param_value(_cfg, "TRACE", "true"); // enable tracing for the Axiom Profiler tool
    Z3_set_param_value(_cfg, "TRACE_FILE_NAME", "'LATrace.log'");   // tracing file name
    Z3_set_param_value(_cfg, "DISPLAY_ERROR_FOR_VISUAL_STUDIO", "true");    // display error messages in Visual Studio format
#endif
    _ctx = NULL;
    _model = NULL;
    _log = NULL;
}

Parser::~Parser(void)
{
    if (_ctx != NULL) Z3_del_context(_ctx);
    Z3_del_config(_cfg);
}

/*
  asks parser to start parsing.
 */
void Parser::Start(Log *log, const string &address, const string &dump)
{
    // each time creates a new context, 
    // otherwise the context contains the ast nodes from previous calculation
    if (_ctx != NULL) Z3_del_context(_ctx);        

    _ctx = Z3_mk_context(_cfg);
    _log = log;
    if (_log->GetParsedAddresses().empty())
        return;

    map<string, vector<LogEntry> > mp = _log->GetParsedAddresses();
    map<string, vector<LogEntry> >::iterator mitr;
    mitr = mp.find(address);
    if (mitr == mp.end())
        return;

    _address = address;
    vector<LogEntry> entries = mitr->second;

    Z3_ast c = NULL;

    Z3_ast ct = CreateThreadOrderConstraint(entries);
    
    Z3_ast cu = CreateUniquenessConstraint(entries);

    Z3_ast cc = CreateCoherenceConstraint(entries, dump);
    
    if (ct != NULL)
        c = ct;

    if (cu != NULL)
    {
        if (c != NULL)
        {
            Z3_ast args[] = { c, cu };
            c = Z3_mk_and(_ctx, 2, args);
        }
        else c = cu;
    }

    if (cc != NULL)
    {
        if (c != NULL)
        {
            Z3_ast args[] = { c, cc };
            c = Z3_mk_and(_ctx, 2, args);
        }
        else c = cc;
    }

    cout << "There are " << AstCount << " nodes in the context!" << endl;

    if (c != NULL)
    {
#if kLADebug
        EZLOGGER(Z3_ast_to_string(_ctx, c));
#endif
        Z3_assert_cnstr(_ctx, c);
    }
}

/*
  gets the final dump value for given address.
 */
string Parser::DumpValue(const string &address)
{
    // TODO: this should be fixed until getting the final dump.
    return "2";
}

/*
  creates thread order constraint by given entries.
 */
Z3_ast Parser::CreateThreadOrderConstraint(vector<LogEntry> &entries)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    Z3_ast ct_ast = NULL;
    bool first = true;

    Z3_ast left, right, lessThanCstr;
    vector<LogEntry>::iterator leftEntriesItr;
    vector<LogEntry>::iterator rightEntriesItr;

    for (leftEntriesItr = entries.begin(); leftEntriesItr != entries.end(); leftEntriesItr++)
    {
        left = leftEntriesItr->GetSymbolVarible(_ctx);
        for (rightEntriesItr = leftEntriesItr; rightEntriesItr != entries.end(); rightEntriesItr++)
        {
            // if M_i and M_j are in the same thread, and M_i < M_j
            if (rightEntriesItr->FromSameThread(*leftEntriesItr) &&
                rightEntriesItr->GetTotalOrderNum() != leftEntriesItr->GetTotalOrderNum())
            {
                right = rightEntriesItr->GetSymbolVarible(_ctx);
                lessThanCstr = Z3_mk_lt(_ctx, left, right);
                AstCount++;
                if (first)
                {
                    ct_ast = lessThanCstr;
                    first = false;
                }
                else
                {
                    Z3_ast args[] = { ct_ast, lessThanCstr };
                    ct_ast = Z3_mk_and(_ctx, 2, args);
                }
            }
        }
    }

#if kLADebug
    if (ct_ast != NULL)
        EZLOGGER(Z3_ast_to_string(_ctx, ct_ast));
#endif
    
#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return ct_ast;
}

/*
  creates uniqueness constraint by given entries.
 */
Z3_ast Parser::CreateUniquenessConstraint(vector<LogEntry> &entries)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    Z3_ast cu_ast = NULL;
    bool first = true;

    Z3_ast left, right, notEqualCstr;
    vector<LogEntry>::iterator leftEntriesItr;
    vector<LogEntry>::iterator rightEntriesItr;

    for (leftEntriesItr = entries.begin(); leftEntriesItr != entries.end(); leftEntriesItr++)
    {
        left = leftEntriesItr->GetSymbolVarible(_ctx);
        for (rightEntriesItr = leftEntriesItr; rightEntriesItr != entries.end(); rightEntriesItr++)
        {
            // if M_i and M_j are not in the same thread, then M_i != M_j
            if (!rightEntriesItr->FromSameThread(*leftEntriesItr))
            {
                right = rightEntriesItr->GetSymbolVarible(_ctx);
                notEqualCstr = Z3_mk_not(_ctx, Z3_mk_eq(_ctx, left, right));
                AstCount++;
                if (first)
                {
                    cu_ast = notEqualCstr;
                    first = false;
                }
                else
                {
                    Z3_ast args[] = { cu_ast, notEqualCstr };
                    cu_ast = Z3_mk_and(_ctx, 2, args);
                }
            }
        }
    }

#if kLADebug
    if (cu_ast != NULL)
        EZLOGGER(Z3_ast_to_string(_ctx, cu_ast));
#endif

#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return cu_ast;
}

/*
  creates coherence constraint by given entries.
  TODO: this one definitely needs refactory.
 */
Z3_ast Parser::CreateCoherenceConstraint(vector<LogEntry> &entries, const string &dump)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    Z3_ast cc_ast = NULL;
    bool entryfirst = true;     // first time to calculate C_i

    vector<LogEntry> pfs, cfs, last;

    vector<LogEntry>::iterator leitr;     // entries iterator

    for (leitr = entries.begin(); leitr != entries.end(); leitr++)  // *leitr presents M_i
    {
        LogEntry ent = *leitr;
        pfs = CreatePotentialFollowers(ent, entries);   // potential followers
        cfs = CreateCoherenceFollowers(ent, pfs);       // coherence followers
        last = CreateLastSet(entries);                  // last memory accesses

        Z3_ast cf_constrs = NULL;       // constraint for M_j in CFS
        bool cffirst = true;            // first time to calculate M_j
        while (!cfs.empty())
        {
            LogEntry cfle = cfs.back(); // retrieve M_j from CFS
            cfs.pop_back();
            Z3_ast cf_constr = Z3_mk_lt(_ctx, ent.GetSymbolVarible(_ctx), 
                                              cfle.GetSymbolVarible(_ctx)); // M_i < M_j
            AstCount++;

            vector<LogEntry> tmppfs = pfs;  // get a copy, because later using pop_back will destroy PFS
            Z3_ast pf_constrs = NULL;       // constraint for M_k in PFS
            bool pffirst = true;            // first time to calculate M_k
            while (!tmppfs.empty())
            {
                LogEntry pfle = tmppfs.back();  // retrieve M_k from PFS
                tmppfs.pop_back();
                // M_j != M_k
                if (cfle.GetTotalOrderNum() != pfle.GetTotalOrderNum())
                {
                    Z3_ast arg1 = NULL, arg2 = NULL;   
                    if (!pfle.FromSameThread(ent))
                    {
                        // M_k < M_i
                        arg1 = Z3_mk_lt(_ctx, pfle.GetSymbolVarible(_ctx), 
                            ent.GetSymbolVarible(_ctx));
                        AstCount++;
                    }

                    if (!pfle.FromSameThread(cfle))
                    {
                        // M_j < M_k
                        arg2 = Z3_mk_lt(_ctx, cfle.GetSymbolVarible(_ctx), 
                            pfle.GetSymbolVarible(_ctx));
                        AstCount++;
                    }
                    
                    Z3_ast pf_constr = NULL;
                    if (arg1 && arg2)
                    {
                        Z3_ast args[] = { arg1, arg2 };
                        pf_constr = Z3_mk_or(_ctx, 2, args); // (M_k < M_i) U (M_j < M_k)
                    }
                    else if (arg1 && !arg2)
                    {
                        pf_constr = arg1;
                    }
                    else
                    {
                        pf_constr = arg2;
                    }
                    
                    if (pffirst)
                    {
                        pf_constrs = pf_constr;
                        pffirst = false;
                    }
                    else
                    {
                        Z3_ast pf_ands[] = { pf_constrs, pf_constr };
                        pf_constrs = Z3_mk_and(_ctx, 2, pf_ands);
                    }
                }
            }

            if( pf_constrs != NULL)
            {
                Z3_ast cf_ands[] = { cf_constr, pf_constrs };
                cf_constr = Z3_mk_and(_ctx, 2, cf_ands);
            }

            if (cffirst)
            {
                cf_constrs = cf_constr;
                cffirst = false;
            }
            else
            {
                Z3_ast cf_ors[] = { cf_constrs, cf_constr };
                cf_constrs = Z3_mk_or(_ctx, 2, cf_ors);
            }
        }

        // whether is M_i in LAST
        vector<LogEntry>::iterator lstitr = find(last.begin(), last.end(), ent);
        if (lstitr != last.end() && ent.GetNewValue() == dump)  // ent.newValue == dump
        {
            bool lstfirst = true;   // first time to calculate M_i in LAST
            Z3_ast lst_ands = NULL;
            while (!last.empty())
            {
                LogEntry lstle = last.back();
                last.pop_back();
                if (lstle.GetTotalOrderNum() != ent.GetTotalOrderNum())
                {
                    Z3_ast lstcnt = Z3_mk_lt(_ctx, lstle.GetSymbolVarible(_ctx), 
                                                   ent.GetSymbolVarible(_ctx));
                    AstCount++;
                    if (lstfirst)
                    {
                        // M_k < M_i
                        lst_ands = lstcnt;
                        lstfirst = false;
                    }
                    else
                    {
                        Z3_ast lstargs[] = { lst_ands, lstcnt };
                        lst_ands = Z3_mk_and(_ctx, 2, lstargs);
                    }
                }
            }

            if (lst_ands != NULL)
            {
                if (cf_constrs != NULL)
                {
                    Z3_ast lstors[] = { cf_constrs, lst_ands };
                    cf_constrs = Z3_mk_or(_ctx, 2, lstors);
                }
                else
                {
                    cf_constrs = lst_ands;
                }
            }
        }

        if (cf_constrs != NULL)
        {
            if (entryfirst)
            {
                cc_ast = cf_constrs;
                entryfirst = false;
            }
            else
            {
                Z3_ast top_ands[] = { cc_ast, cf_constrs };
                cc_ast = Z3_mk_and(_ctx, 2, top_ands);
            }
        }
    }
#if kLADebug
    if (cc_ast != NULL)
        EZLOGGER(Z3_ast_to_string(_ctx, cc_ast));
#endif

#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return cc_ast;
}

/*
  creates potential followers set according to given entry.
 */
vector<LogEntry> Parser::CreatePotentialFollowers(const LogEntry &entry, const vector<LogEntry> &entries)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    vector<LogEntry> pfs;
    vector<LogEntry>::const_iterator itr;
    bool findSelf = false;      // findFlag means find the given entry from the argument entries
    
    for (itr = entries.begin(); itr != entries.end(); itr++)
    {
        if (!itr->FromSameThread(entry))    // if not from the same thread, put it into set
        {
            pfs.push_back(*itr);
            continue;
        }

        if (*itr == entry)
        {
            findSelf = true;
            continue;
        }

        if (findSelf && itr->FromSameThread(entry) 
            && itr->GetTotalOrderNum() > entry.GetTotalOrderNum())
        {
            pfs.push_back(*itr);
            findSelf = false;
            continue;
        }
    }
    
#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return pfs;
}

/*
  creates coherence followers set according to given entry.
 */
vector<LogEntry> Parser::CreateCoherenceFollowers(const LogEntry &entry, const vector<LogEntry> &pfs)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    vector<LogEntry> cfs;
    vector<LogEntry>::const_iterator pfsItr;

    for (pfsItr = pfs.begin(); pfsItr != pfs.end(); pfsItr++)
    {
        // if M_i.newValue == M_j.oldValue
        if (pfsItr->GetOldValue().compare(entry.GetNewValue()) == 0)
        {
            cfs.push_back(*pfsItr);
        }
    }

#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return cfs;
}

/*
  creates last set, which contains last access of each thread
 */
vector<LogEntry> Parser::CreateLastSet(const vector<LogEntry> &entries)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    vector<LogEntry> last;
    vector<LogEntry>::iterator insertItr;

    bool flag = false;

    vector<LogEntry>::const_iterator itr;
    for (itr = entries.begin(); itr != entries.end(); itr++)
    {
        for (insertItr = last.begin(); insertItr != last.end(); insertItr++)
        {
            if (itr->FromSameThread(*insertItr))
            {
                *insertItr = *itr;
                flag = true;
                break;
            }
            flag = false;
        }

        if (!flag)
        {
            last.push_back(*itr);
            flag = true;
        }
    }

#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return last;
}

/*
  return the final result.
 */
Z3_model Parser::GetResult(void)
{
#if kLAPerformance && kLADebug
    __int64 pStart = 0, pEnd = 0;
    pStart = PerformanceCounter();
#endif

    Z3_check_and_get_model(_ctx, &_model);

    if (NULL != _model) // set the symbolic values here, because the model has been solved.
    {
        Z3_ast odr;
        unsigned num = Z3_get_model_num_constants(_ctx, _model);
        vector<LogEntry> entries = _log->GetParsedAddresses()[_address];
        for (unsigned i = 0; i < num; i++)
        {
            Z3_ast out;
            if (!Z3_eval(_ctx, _model, entries[i].GetSymbolVarible(_ctx), &out))
            {
                EZLOGGER("Failed to evaluate symbolic value");
                exit(-1);
            }
            entries[i].SetSymbolValue(atoi(Z3_ast_to_string(_ctx, out)));
        }
        _log->GetParsedAddresses()[_address] = entries;
    }

#if kLAPerformance && kLADebug
    pEnd = PerformanceCounter();
    EZLOGGERPRINT("Takes %g ms.", (pEnd-pStart)/PCPerformanceFreq());
#endif
    return _model;
}
