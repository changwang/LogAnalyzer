#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <map>
#include <vector>
#include "LogEntry.h"
#include "LAWindow.h"
#include "AddressUpdateThread.h"

LAWindow::LAWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    QWidget *centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    btn_choose = new QPushButton(centralWidget);
    btn_choose->setText("Choose Log");
    connect(btn_choose, SIGNAL(clicked()), this, SLOT(OnChooseLogFile()));

    lbl_path = new QLabel(centralWidget);
    lbl_path->setText("File path will be shown here!");

    btn_parse = new QPushButton(centralWidget);
    btn_parse->setText("Parse");
    connect(btn_parse, SIGNAL(clicked()), this, SLOT(OnParse()));

    gridLayout->addWidget(btn_choose, 0, 0, 1, 1);
    gridLayout->addWidget(lbl_path, 0, 1, 1, 1);
    gridLayout->addWidget(btn_parse, 1, 0, 1, 1);

    QGroupBox *grp_result = new QGroupBox(centralWidget);
    grp_result->setTitle("Result");
    grp_result->setEnabled(true);

    hboxLayout = new QHBoxLayout(grp_result);
    hboxLayout->setSpacing(5);
    hboxLayout->setContentsMargins(5, 5, 5, 5);

    lst_address = new QListWidget(grp_result);
    connect(lst_address, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnStart()));
    btn_start = new QPushButton(grp_result);
    btn_start->setText(">");
    connect(btn_start, SIGNAL(clicked()), this, SLOT(OnStart()));
    tbl_orders = new QTableWidget(grp_result);
    tbl_orders->setColumnCount(2);
    tbl_orders->verticalHeader()->hide();
    tbl_orders->setColumnWidth(0, 100);
    tbl_orders->setColumnWidth(1, 400);
    QStringList headers;
    headers << "Symbol Value" << "Log Entry";
    tbl_orders->setHorizontalHeaderLabels(headers);
    tbl_orders->setAlternatingRowColors(true);
    hboxLayout->addWidget(lst_address);
    hboxLayout->addWidget(btn_start);
    hboxLayout->addWidget(tbl_orders);
    hboxLayout->setStretch(2, 1);

    gridLayout->addWidget(grp_result, 2, 0, 1, 2);
    gridLayout->setColumnStretch(1, 1);

    setCentralWidget(centralWidget);
    setWindowTitle("Log Analzyer");
    resize(800, 600);

    _log = new Log(_parser.GetZ3Context());
}

LAWindow::~LAWindow()
{
}

void LAWindow::OnChooseLogFile()
{
    filePath = QFileDialog::getOpenFileName(this);
    if ((filePath != NULL) && (filePath != ""))
        lbl_path->setText(filePath);
}

void LAWindow::OnParse()
{
    _log->SetLogFileName(filePath.toStdString());
    _log->ParseLog();

    lst_address->clear();

    map<string, vector<LogEntry> > mp = _log->GetParsedAddresses();
    AddressUpdateThread aut(lst_address, mp);
    aut.start();
}

void LAWindow::OnStart()
{
    QList<QListWidgetItem *> items = lst_address->selectedItems();
    if (items.empty()) return;
    QListWidgetItem *item = items.first();

    _parser.Start(_log, item->text().toStdString(), "2");
    if (_parser.GetResult() != NULL)
    {
        vector<LogEntry> entries = _log->GetParsedAddresses()[item->text().toStdString()];
        if (entries.size() < 1) return;
        tbl_orders->setRowCount(entries.size());
        for (unsigned i = 0; i < entries.size(); i++)
        {
            QTableWidgetItem *column1Item = new QTableWidgetItem();
            column1Item->setText(QString::number(entries[i].GetSybmolValue()));
            column1Item->setFlags(column1Item->flags() & (~Qt::ItemIsEditable));
            column1Item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            tbl_orders->setItem(i, 0, column1Item);

            QTableWidgetItem *column2Item = new QTableWidgetItem();
            column2Item->setText(QString::fromStdString(entries[i].ToString()));
            column2Item->setFlags(column1Item->flags() & (~Qt::ItemIsEditable));
            column2Item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            tbl_orders->setItem(i, 1, column2Item);
        }
    }
}

