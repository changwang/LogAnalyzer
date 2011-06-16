#ifndef _LAWINDOW__H_
#define _LAWINDOW__H_

#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#include "Log.h"
#include "LogEntry.h"
#include "Parser.h"

class LAWindow : public QMainWindow
{
    Q_OBJECT

    QPushButton *_btn_choose;
    QPushButton *_btn_parse;
    QPushButton *_btn_start;

    QLabel *_lbl_path;

    QListWidget *_lst_address;
    QTableWidget *_tbl_orders;

    QString _filePath;
    Parser _parser;
    Log *_log;

public:
    LAWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~LAWindow() {}

public slots:

    void OnChooseLogFile(void);
    void OnParse(void);
    void OnStart(void);

};

#endif // _LAWINDOW__H_
