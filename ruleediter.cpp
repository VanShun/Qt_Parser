#include "ruleediter.h"
#include "ui_ruleediter.h"

RuleEditer::RuleEditer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RuleEditer)
{
    ui->setupUi(this);
    initEditer();
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

RuleEditer::~RuleEditer()
{
    delete ui;
}

void RuleEditer::initEditer()
{
#if 0
    themodel = new QStandardItemModel(2, FIXED_CULUMCOUNT, this);
    theselect = new QItemSelectionModel(themodel);
    ui->tableView->setModel(themodel);
    ui->tableView->setSelectionModel(theselect);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    QStringList headerlist;
    headerlist << QString("字段名") << QString("字段长度") << QString("单位(没有则不填)")
               << QString("精度(没有则不填)") << QString("偏移(没有则不填)");
    themodel->setHorizontalHeaderLabels(headerlist);
#endif
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase");
    if (!db.open()) {
        qDebug() << "open database failed!" << endl;
        return;
    }
    themodel = new QSqlTableModel(this);


}
void RuleEditer::on_act_Insert_triggered()
{

}

void RuleEditer::on_act_Apend_triggered()
{

}

void RuleEditer::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu *menulist = new QMenu(this);
    menulist->addAction(ui->act_Insert);
    menulist->addAction(ui->act_Apend);
    menulist->exec(QCursor::pos());
    delete menulist;
}
