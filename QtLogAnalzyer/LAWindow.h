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

    QPushButton *btn_choose;
    QPushButton *btn_parse;
    QPushButton *btn_start;

    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;

    QLabel *lbl_path;

    QListWidget *lst_address;
    QTableWidget *tbl_orders;

    QString filePath;
    Parser _parser;
    Log *_log;

public:
    LAWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~LAWindow();

public slots:

    void OnChooseLogFile();
    void OnParse();
    void OnStart();

};

#endif // _LAWINDOW__H_
