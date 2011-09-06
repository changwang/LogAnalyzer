#ifndef _LAWINDOW__H_
#define _LAWINDOW__H_

#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QProgressBar>
#include "Log.h"
#include "JavaPlainLog.h"
#include "LogEntry.h"
#include "Parser.h"
#include "OrderUpdateThread.h"

class LAWindow : public QMainWindow
{
    Q_OBJECT

private:
    QPushButton *_btn_choose;
    QPushButton *_btn_parse;
    QPushButton *_btn_start;

    QLabel *_lbl_path;

    QTableWidget *_tbl_address;
    QTableWidget *_tbl_orders;

    QProgressBar *_prg_bar;

    QString _filePath;
    Parser _parser;
    Log *_log;

    OrderUpdateThread _outhread;

public:
    LAWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~LAWindow(void) {}

public slots:

    void OnChooseLogFile(void);
    void OnParse(void);
    void OnStart(QTableWidgetItem *item);
    void OnUpdateOrders(void);

};

#endif // _LAWINDOW__H_
