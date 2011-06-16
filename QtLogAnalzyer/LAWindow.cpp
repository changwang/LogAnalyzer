#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <map>
#include <vector>
#include "LAWindow.h"

enum {
    ResultColumnOrder,
    ResultColumnLogEntry
} ResultColumn;

LAWindow::LAWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;

    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    _btn_choose = new QPushButton(centralWidget);
    _btn_choose->setText("Choose Log");
    connect(_btn_choose, SIGNAL(clicked()), this, SLOT(OnChooseLogFile(void)));

    _lbl_path = new QLabel(centralWidget);
    _lbl_path->setText("File path will be shown here!");

    _btn_parse = new QPushButton(centralWidget);
    _btn_parse->setText("Parse");
    _btn_parse->setEnabled(false);
    connect(_btn_parse, SIGNAL(clicked()), this, SLOT(OnParse(void)));

    _prg_bar = new QProgressBar(centralWidget);
    _prg_bar->setMinimum(0);
    _prg_bar->setMaximum(0);
    _prg_bar->setTextVisible(false);
    _prg_bar->setVisible(false);

    gridLayout->addWidget(_btn_choose, 0, 0, 1, 1);
    gridLayout->addWidget(_lbl_path, 0, 1, 1, 1);    
    gridLayout->addWidget(_btn_parse, 1, 0, 1, 1);
    gridLayout->addWidget(_prg_bar, 2, 0, 1, 2);

    QGroupBox *grp_result = new QGroupBox(centralWidget);
    grp_result->setTitle("Result");

    hboxLayout = new QHBoxLayout(grp_result);
    hboxLayout->setSpacing(5);
    hboxLayout->setContentsMargins(5, 5, 5, 5);

    _lst_address = new QListWidget(grp_result);
    connect(_lst_address, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(OnStart(void)));
    _btn_start = new QPushButton(grp_result);
    _btn_start->setText(">");
    connect(_btn_start, SIGNAL(clicked()), this, SLOT(OnStart(void)));

    _tbl_orders = new QTableWidget(grp_result);
    _tbl_orders->setColumnCount(2);
    _tbl_orders->verticalHeader()->hide();
    _tbl_orders->setColumnWidth(0, 100);
    _tbl_orders->setColumnWidth(1, 400);
    QStringList headers;
    headers << "Symbol Value" << "Log Entry";
    _tbl_orders->setHorizontalHeaderLabels(headers);
    _tbl_orders->setAlternatingRowColors(true);

    hboxLayout->addWidget(_lst_address);
    hboxLayout->addWidget(_btn_start);
    hboxLayout->addWidget(_tbl_orders);
    hboxLayout->setStretch(2, 1);

    gridLayout->addWidget(grp_result, 3, 0, 1, 2);
    gridLayout->setColumnStretch(1, 1);

    setCentralWidget(centralWidget);
    setWindowTitle("Log Analzyer");
    resize(800, 600);

    _log = new Log(_parser.GetZ3Context());
    connect(&_outhread, SIGNAL(OnFinishedSorting()), this, SLOT(OnUpdateOrders()));
}

void LAWindow::OnChooseLogFile()
{
    _filePath = QFileDialog::getOpenFileName(this);
    if ((_filePath != NULL) && (_filePath != "")) {
        _lbl_path->setText(_filePath);
        _btn_parse->setEnabled(true);
    }
}

void LAWindow::OnParse()
{
    _log->SetLogFileName(_filePath.toStdString());
    _log->ParseLog();

    _lst_address->clear();

    map<string, vector<LogEntry> > mp = _log->GetParsedAddresses();
    map<string, vector<LogEntry> >::const_iterator itr;
    for (itr = mp.begin(); itr != mp.end(); itr++)
    {
        itr->first.c_str();
        _lst_address->addItem(QString::fromStdString(itr->first));
    }
}

void LAWindow::OnStart()
{
    QList<QListWidgetItem *> items = _lst_address->selectedItems();
    if (items.empty()) return;
    QListWidgetItem *item = items.first();

    _tbl_orders->clearContents();
    _parser.Start(_log, item->text().toStdString(), "2");
    if (_parser.GetResult() != NULL)
    {
        _prg_bar->setVisible(true);
        vector<LogEntry> entries = _log->GetParsedAddresses()[item->text().toStdString()];
        if (entries.empty()) return;
        _tbl_orders->setRowCount(entries.size());
        _outhread.Sort(entries);
    }
}

void LAWindow::OnUpdateOrders()
{
    vector<LogEntry> entries = _outhread.entries();
    for (unsigned i = 0; i < entries.size(); i++)
    {
        QTableWidgetItem *column1Item = new QTableWidgetItem();
        column1Item->setText(QString::number(entries[i].GetSybmolValue()));
        column1Item->setFlags(column1Item->flags() & (~Qt::ItemIsEditable));
        column1Item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        _tbl_orders->setItem(i, ResultColumnOrder, column1Item);

        QTableWidgetItem *column2Item = new QTableWidgetItem();
        column2Item->setText(QString::fromStdString(entries[i].ToString()));
        column2Item->setFlags(column1Item->flags() & (~Qt::ItemIsEditable));
        column2Item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        _tbl_orders->setItem(i, ResultColumnLogEntry, column2Item);
    }
    _prg_bar->setVisible(false);
}

