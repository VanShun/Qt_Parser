#include "ruleediter.h"
#include "ui_ruleediter.h"

RuleEditer::RuleEditer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RuleEditer)
{
    ui->setupUi(this);
    initEditer();
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

    theselect = new QItemSelectionModel(themodel);
    ui->tableView->setSelectionModel(theselect);
#endif
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase");
    if (!db.open()) {
        qDebug() << "open database failed!" << endl;
        return;
    }
    themodel = new QSqlTableModel(this);
    ui->tableView->setModel(themodel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void RuleEditer::intXmlFile()
{
    QString xmlfilename = curtable;
    xmlfilename += ".xml";
    QFile file(xmlfilename);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << QString("open file %1 failed").arg(xmlfilename) << endl;
        return;
    }
    if (!doc.setContent(&file)) {
        qDebug() << "doc setContent failed" << endl;
        file.close();
        return;
    }
}

void RuleEditer::showXml()
{

}

void RuleEditer::saveTabletoXml()
{

}

void RuleEditer::on_act_Insert_triggered()
{
    int curRow = ui->tableView->currentIndex().row();
    int id = curRow;
    themodel->insertRow(curRow);
    themodel->setData(themodel->index(curRow, 0), id);
}

void RuleEditer::on_act_Apend_triggered()
{
    int rowCount = themodel->rowCount();
    qDebug() << "rowcount: " << rowCount;
    int id = rowCount;
    themodel->insertRow(rowCount);
    themodel->setData(themodel->index(rowCount, 0), id);
}

void RuleEditer::on_act_Delete_triggered()
{
    int curRow = ui->tableView->currentIndex().row();
    themodel->removeRow(curRow);
    //QModelIndexList indexlist = theselect->selectedRows(0);
    //for (int i = 0; i < indexlist.count(); i++) {
        //themodel->removeRow(indexlist.at(i).row());
    //}
}

void RuleEditer::on_act_Undo_triggered()
{
    themodel->revertAll();
}

void RuleEditer::on_act_Save_triggered()
{
    themodel->submitAll();
}

void RuleEditer::on_act_Saveto_triggered()
{

}

void RuleEditer::on_act_trunc_triggered()
{
    QSqlQuery query;
    QString str = "truncate table " + curtable;
    qDebug() << "truncate str:" << str << endl;
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, QString("警告"), QString("确认清空表格？"),
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::NoButton);
    if (result == QMessageBox::Yes) {
        query.exec(str);
        themodel->select();
    }
}

void RuleEditer::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu *menulist = new QMenu(this);
    menulist->addAction(ui->act_Insert);
    menulist->addAction(ui->act_Apend);
    menulist->addAction(ui->act_Delete);
    menulist->addAction(ui->act_Undo);
    menulist->addAction(ui->act_trunc);
    menulist->exec(QCursor::pos());
    delete menulist;
}

void RuleEditer::on_act_New_triggered()
{
    bool ok;
    QString tablename = QInputDialog::getText(this, QString("请输入表名"), QString("表名"), QLineEdit::Normal, QString("table1"), &ok);
    qDebug() << tablename << endl;
    if (ok && !tablename.isEmpty()) {
        QSqlQuery query;
        QString str = "create table " + tablename;
        str = str + "(id int primary key, name varchar(20), len int, unit varchar(10), precision float, offset int)";

        if (!query.exec(str)) {
            qDebug() << "create table failed";
            QStringList tablelist = db.tables();
            for (int i = 0; i < tablelist.count(); i++) {
                if(tablelist.at(i) == tablename) {
                    QMessageBox::information(this, QString("错误提示"), QString("文件已存在，不能新建同名文件!"), QMessageBox::Ok, QMessageBox::NoButton);
                }
            }
            return;
        }
    }

    themodel->setTable(tablename);
    themodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    themodel->select();
    curtable = tablename;
}

void RuleEditer::on_act_Open_triggered()
{
    QStringList tablelist;
    tablelist = db.tables();
    bool ok;
    QString tablename = QInputDialog::getItem(this, QString("打开规则表"), QString("选择表"), tablelist, 0, false, &ok);
    if (ok && !tablename.isEmpty()) {
        themodel->setTable(tablename);
        themodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        themodel->select();
        curtable = tablename;
    }
}



