#include <string>
#include "gtest/gtest.h"
#include "z3.h"
#include "Helper.h"
#include "Log.h"
#include "JavaPlainLog.h"
#include "LogEntry.h"
#include "Parser.h"

// ======================================
// Test Helper modulo
// ======================================

TEST(IntToStringTest, ZeroInput)
{
    EXPECT_EQ("0", IntToString(0));
}

TEST(IntToStringTest, NegativeInput)
{
    EXPECT_EQ("-1", IntToString(-1));
    EXPECT_EQ("-255", IntToString(-255));
}

TEST(IntToStringTest, PositiveInput)
{
    EXPECT_EQ("1", IntToString(1));
    EXPECT_EQ("255", IntToString(255));
}

TEST(StringToOpTypeTest, ReadOperation)
{
    EXPECT_EQ(OperationRead, StringToOpType("read_var"));
    EXPECT_EQ(OperationRead, StringToOpType("read"));
    EXPECT_NE(OperationUndefined, StringToOpType("read_var"));
}

TEST(StringToOpTypeTest, WriteOperation)
{
    EXPECT_EQ(OperationWrite, StringToOpType("write_var"));
    EXPECT_EQ(OperationWrite, StringToOpType("write"));
    EXPECT_NE(OperationUndefined, StringToOpType("write_var"));
}

TEST(StringToOpTypeTest, AcquireOperation)
{
    EXPECT_EQ(OperationAcquire, StringToOpType("acquire_lock"));
    EXPECT_EQ(OperationAcquire, StringToOpType("acquire_var"));
    EXPECT_EQ(OperationAcquire, StringToOpType("acquire"));
    EXPECT_NE(OperationUndefined, StringToOpType("acquire_lock"));
}

TEST(StringToOpTypeTest, ReleaseOperation)
{
    EXPECT_EQ(OperationRelease, StringToOpType("release_lock"));
    EXPECT_EQ(OperationRelease, StringToOpType("release_var"));
    EXPECT_EQ(OperationRelease, StringToOpType("release"));
    EXPECT_NE(OperationUndefined, StringToOpType("release_lock"));
}

TEST(StringToOpTypeTest, UndefinedOperation)
{
    EXPECT_EQ(OperationUndefined, StringToOpType("undefined"));
    EXPECT_NE(OperationUndefined, StringToOpType("read_var"));
    EXPECT_NE(OperationUndefined, StringToOpType("write_var"));
    EXPECT_NE(OperationUndefined, StringToOpType("acquire_lock"));
    EXPECT_NE(OperationUndefined, StringToOpType("release_lock"));
}

TEST(OpTypeToStringTest, ReadString)
{
    EXPECT_EQ("read_var", OpTypeToString(OperationRead));
    EXPECT_NE("write_var", OpTypeToString(OperationRead));
    EXPECT_NE("acquire_lock", OpTypeToString(OperationRead));
    EXPECT_NE("release_lock", OpTypeToString(OperationRead));
    EXPECT_NE("undefined", OpTypeToString(OperationRead));
}

TEST(OpTypeToStringTest, WriteString)
{
    EXPECT_EQ("write_var", OpTypeToString(OperationWrite));
    EXPECT_NE("read_var", OpTypeToString(OperationWrite));
    EXPECT_NE("acquire_lock", OpTypeToString(OperationWrite));
    EXPECT_NE("release_lock", OpTypeToString(OperationWrite));
    EXPECT_NE("undefined", OpTypeToString(OperationWrite));
}

TEST(OpTypeToStringTest, AcquireString)
{
    EXPECT_EQ("acquire_lock", OpTypeToString(OperationAcquire));
    EXPECT_NE("read_var", OpTypeToString(OperationAcquire));
    EXPECT_NE("write_var", OpTypeToString(OperationAcquire));
    EXPECT_NE("release_lock", OpTypeToString(OperationAcquire));
    EXPECT_NE("undefined", OpTypeToString(OperationAcquire));
}

TEST(OpTypeToStringTest, ReleaseString)
{
    EXPECT_EQ("release_lock", OpTypeToString(OperationRelease));
    EXPECT_NE("read_var", OpTypeToString(OperationRelease));
    EXPECT_NE("write_var", OpTypeToString(OperationRelease));
    EXPECT_NE("acquire_lock", OpTypeToString(OperationRelease));
    EXPECT_NE("undefined", OpTypeToString(OperationRelease));
}

// ======================================
// Test LogEntry class
// ======================================

class LogEntryTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        _cfg = Z3_mk_config();
        Z3_set_param_value(_cfg, "MODEL", "true");
        _ctx = Z3_mk_context(_cfg);
        _entry = new LogEntry(0);
    }

    virtual void TearDown()
    {
        delete _entry;
        Z3_del_config(_cfg);
        Z3_del_context(_ctx);
    }

    Z3_config _cfg;
    Z3_context _ctx;
    LogEntry *_entry;
};

TEST_F(LogEntryTest, IsNotInitialized)
{
    EXPECT_EQ(0, _entry->GetThreadId());
    EXPECT_EQ(OperationUndefined, _entry->GetOpType());
    EXPECT_EQ("", _entry->GetAddress());
    EXPECT_EQ("", _entry->GetOldValue());
    EXPECT_EQ("", _entry->GetNewValue());
    EXPECT_EQ(0, _entry->GetTotalOrderNum());
    EXPECT_EQ(0, _entry->GetSybmolValue());
}

TEST_F(LogEntryTest, DeclareSymbolVar)
{
    string strSymVar(kLAZ3VarPrefix);
    _entry->SetTotalOrderNum(1);
    strSymVar.append(IntToString(_entry->GetThreadId()) + "_" + IntToString(_entry->GetTotalOrderNum()));
    EXPECT_EQ(0, strcmp(strSymVar.c_str(), Z3_ast_to_string(_ctx, _entry->GetSymbolVarible(_ctx))));

    strSymVar = string(kLAZ3VarPrefix);
    _entry->SetTotalOrderNum(10);
    strSymVar.append(IntToString(_entry->GetThreadId()) + "_" + IntToString(_entry->GetTotalOrderNum()));
    EXPECT_EQ(0, strcmp(strSymVar.c_str(), Z3_ast_to_string(_ctx, _entry->GetSymbolVarible(_ctx))));
}

TEST_F(LogEntryTest, Equality)
{
#define kTestTotalOrderNumber   10
    LogEntry *other = new LogEntry(20);
    other->SetTotalOrderNum(kTestTotalOrderNumber);
    _entry->SetTotalOrderNum(kTestTotalOrderNumber);
    EXPECT_EQ(true, *other == *_entry);

#define kTestTotalOrderNumber2  20
    other->SetTotalOrderNum(kTestTotalOrderNumber2);
    EXPECT_EQ(false, *other == *_entry);
    delete other;
}

TEST_F(LogEntryTest, CompareOrder)
{
#define kTestComparationOrder   10
    LogEntry *other = new LogEntry(20);
    _entry->SetSymbolValue(kTestComparationOrder);

#define kTestComparationOrder2   11
    other->SetSymbolValue(kTestComparationOrder2);
    EXPECT_EQ(true, *_entry < *other);

#define kTestComparationOrder3   9
    other->SetSymbolValue(kTestComparationOrder3);
    EXPECT_EQ(true, *other < *_entry);

    delete other;
}

// ======================================
// Test Log class
// ======================================

class LogTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        _cfg = Z3_mk_config();
        Z3_set_param_value(_cfg, "MODEL", "true");
        _ctx = Z3_mk_context(_cfg);
        _log = new JavaPlainLog(_ctx, "test.txt");
    }

    virtual void TearDown()
    {
        delete _log;
        Z3_del_config(_cfg);
        Z3_del_context(_ctx);
    }

    Z3_config _cfg;
    Z3_context _ctx;
    Log *_log;
};

TEST_F(LogTest, IsInitialized)
{
#define kTestLogFileName    "test.txt"
    EXPECT_EQ(kTestLogFileName, _log->GetLogFileName());
    EXPECT_EQ(0, _log->GetParsedAddresses()->size());
}

TEST_F(LogTest, ParseLog)
{
    _log->ParseLog();
    EXPECT_EQ(1, _log->GetParsedAddresses()->size());

#define kTestEntryThreadID  7
#define kTestLogAddress "27744459"

    LogEntry entry = (*_log->GetParsedAddresses())[kTestLogAddress][0];
    EXPECT_EQ(kTestEntryThreadID, entry.GetThreadId());
    EXPECT_EQ(OperationWrite, entry.GetOpType());
    EXPECT_EQ(kTestLogAddress, entry.GetAddress());
    EXPECT_EQ("1", entry.GetOldValue());
    EXPECT_EQ("0", entry.GetNewValue());
    EXPECT_EQ(1, entry.GetTotalOrderNum());
    EXPECT_EQ(0, entry.GetSybmolValue());
}

// ======================================
// Test Parser class
// ======================================

class ParserTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        _log = new JavaPlainLog(_parser.GetZ3Context(), "1.txt");
        _log->ParseLog();
    }

    virtual void TearDown()
    {
        delete _log;
    }

    Parser _parser;
    Log *_log;
};

TEST_F(ParserTest, DumpAddressValue)
{
    EXPECT_EQ("2", _parser.DumpValue(""));
}

TEST_F(ParserTest, ThreadOrderConstr)
{

}

TEST_F(ParserTest, UniquenessConstr)
{

}

TEST_F(ParserTest, CoherenceConstr)
{

}

TEST_F(ParserTest, ModelResult)
{

}
