#include <algorithm>
#include <list>
#include "Parser.h"
#include "Helper.h"

Parser::Parser(void)
{
    _cfg = Z3_mk_config();
    Z3_set_param_value(_cfg, "MODEL", "true");
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
    if (_ctx != NULL)
    {
        Z3_del_context(_ctx);        
    }
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
        else if (c == NULL)
        {
            c = cu;
        }
    }

    if (cc != NULL)
    {
        if (c != NULL)
        {
            Z3_ast args[] = { c, cc };
            c = Z3_mk_and(_ctx, 2, args);
        }
        else if (c == NULL)
        {
            c = cc;
        }
    }

    if (c != NULL)
    {
        cout << Z3_ast_to_string(_ctx, c) << endl;
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
    Z3_ast ct_ast = NULL;
    bool first = true;

    Z3_ast left, right, constr;
    vector<LogEntry>::iterator outteritr;
    vector<LogEntry>::iterator inneritr;

    for (outteritr = entries.begin(); outteritr != entries.end(); outteritr++)
    {
        left = outteritr->GetSymbolVarible(_ctx);
        for (inneritr = outteritr; inneritr != entries.end(); inneritr++)
        {
            // if M_i and M_j are in the same thread, and M_i < M_j
            if (inneritr->GetThreadId() == outteritr->GetThreadId() &&
                inneritr->GetTotalOrderNum() != outteritr->GetTotalOrderNum())
            {
                right = inneritr->GetSymbolVarible(_ctx);
                constr = Z3_mk_lt(_ctx, left, right);
                if (first)
                {
                    ct_ast = constr;
                    first = false;
                }
                else
                {
                    Z3_ast args[] = { ct_ast, constr };
                    ct_ast = Z3_mk_and(_ctx, 2, args);
                }
            }
        }
    }
#if kLAShowLog
    if (ct_ast != NULL)
        cout << Z3_ast_to_string(_ctx, ct_ast) << endl;
#endif
    
    return ct_ast;
}

/*
  creates uniqueness constraint by given entries.
 */
Z3_ast Parser::CreateUniquenessConstraint(vector<LogEntry> &entries)
{
    Z3_ast cu_ast = NULL;
    bool first = true;

    Z3_ast left, right, constr;
    vector<LogEntry>::iterator outteritr;
    vector<LogEntry>::iterator inneritr;

    for (outteritr = entries.begin(); outteritr != entries.end(); outteritr++)
    {
        left = outteritr->GetSymbolVarible(_ctx);
        for (inneritr = outteritr; inneritr != entries.end(); inneritr++)
        {
            // if M_i and M_j are not in the same thread, then M_i != M_j
            if (inneritr->GetThreadId() != outteritr->GetThreadId())
            {
                right = inneritr->GetSymbolVarible(_ctx);
                constr = Z3_mk_not(_ctx, Z3_mk_eq(_ctx, left, right));
                if (first)
                {
                    cu_ast = constr;
                    first = false;
                }
                else
                {
                    Z3_ast args[] = { cu_ast, constr };
                    cu_ast = Z3_mk_and(_ctx, 2, args);
                }
            }
        }
    }
#if kLAShowLog
    if (cu_ast != NULL)
        cout << Z3_ast_to_string(_ctx, cu_ast) << endl;
#endif

    return cu_ast;
}

/*
  creates coherence constraint by given entries.
 */
Z3_ast Parser::CreateCoherenceConstraint(vector<LogEntry> &entries, const string &dump)
{
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
                    // M_k < M_i
                    Z3_ast arg1 = Z3_mk_lt(_ctx, pfle.GetSymbolVarible(_ctx), 
                                                 ent.GetSymbolVarible(_ctx));
                    // M_j < M_k
                    Z3_ast arg2 = Z3_mk_lt(_ctx, cfle.GetSymbolVarible(_ctx), 
                                                 pfle.GetSymbolVarible(_ctx));
                    Z3_ast args[] = { arg1, arg2 };
                    Z3_ast pf_constr = Z3_mk_or(_ctx, 2, args); // (M_k < M_i) U (M_j < M_k)
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
#if kLAShowLog
    if (cc_ast != NULL)
        cout << Z3_ast_to_string(_ctx, cc_ast) << endl;
#endif

    return cc_ast;
}

/*
  creates potential followers set according to given entry.
 */
vector<LogEntry> Parser::CreatePotentialFollowers(const LogEntry &entry, const vector<LogEntry> &entries)
{
    vector<LogEntry> pfs;
    vector<LogEntry>::const_iterator itr;
    bool firstSucc = true, findFlag = false;    // findFlag means find the given entry in the argument
    LogEntry succ(0);
 
    for (itr = entries.begin(); itr != entries.end(); itr++)
    {
        if ((itr->GetThreadId() == entry.GetThreadId()))
        {
            if (findFlag && itr->GetThreadId() == entry.GetThreadId() && firstSucc)
            {
                firstSucc = false;
                succ = *itr;
            }

            if (itr->GetTotalOrderNum() == entry.GetTotalOrderNum())
            {
                findFlag = true;
            }
        }
        else if (itr->GetThreadId() != entry.GetThreadId())
        {
            pfs.push_back(*itr);
        }
    }

    if (succ.GetTotalOrderNum() != 0)
    {
        pfs.push_back(succ);
    }

    return pfs;
}

/*
  creates coherence followers set according to given entry.
 */
vector<LogEntry> Parser::CreateCoherenceFollowers(const LogEntry &entry, const vector<LogEntry> &pfs)
{
    vector<LogEntry> cfs;

    vector<LogEntry>::const_iterator pfsitr;

    for (pfsitr = pfs.begin(); pfsitr != pfs.end(); pfsitr++)
    {
        // if M_i.newValue == M_j.oldValue
        if (pfsitr->GetOldValue().compare(entry.GetNewValue()) == 0)
        {
            cfs.push_back(*pfsitr);
        }
    }

    return cfs;
}

/*
  creates last set, which contains last access of each thread
 */
vector<LogEntry> Parser::CreateLastSet(const vector<LogEntry> &entries)
{
    vector<LogEntry> last;
    vector<LogEntry>::iterator insertitr;

    bool flag = false;

    vector<LogEntry>::const_iterator itr;
    for (itr = entries.begin(); itr != entries.end(); itr++)
    {
        for (insertitr = last.begin(); insertitr != last.end(); insertitr++)
        {
            if (itr->GetThreadId() == insertitr->GetThreadId())
            {
                *insertitr = *itr;
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

    return last;
}

/*
  return the final result.
 */
Z3_model Parser::GetResult(void)
{
    Z3_check_and_get_model(_ctx, &_model);

    if (NULL != _model) // set the symbolic values here, because the model has been solved.
    {
        Z3_ast odr;
        unsigned num = Z3_get_model_num_constants(_ctx, _model);
        vector<LogEntry> entries = _log->GetParsedAddresses()[_address];
        for (unsigned i = 0; i < num; i++)
        {
            Z3_func_decl fd = Z3_get_model_constant(_ctx, _model, i);
            Z3_eval_func_decl(_ctx, _model, fd, &odr);
            entries[i].SetSymbolValue(atoi(Z3_ast_to_string(_ctx, odr)));
        }
        _log->GetParsedAddresses()[_address] = entries;
    }

    return _model;
}
