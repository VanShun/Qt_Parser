#include "ruleediter.h"
#include "ui_ruleediter.h"
#include "mainwindow.h"

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

void RuleEditer::intXmlFile(void)
{
    xmlfilename = curtable;
    xmlfilename += ".xml";
    showXml();
}

void RuleEditer::showXml()
{
    QFile afile(xmlfilename);
    if (!afile.isOpen()) {
        if (!afile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "open xml failed" << endl;
            return;
        }
    }
    QTextStream astream(&afile);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(astream.readAll());
    astream.setAutoDetectUnicode(true);
    afile.close();
}

void RuleEditer::saveTabletoXml()
{
    QFile file(xmlfilename);
    QDomDocument writedoc;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qDebug() << QString(" saveTabletoXml open file %1 failed").arg(xmlfilename) << endl;
        return;
    } else qDebug() << QString("saveTabletoXml open file %1 success").arg(xmlfilename) << endl;

    QDomProcessingInstruction instruction = writedoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    writedoc.appendChild(instruction);    //添加xml申明
    QDomElement root = writedoc.createElement("协议");
    writedoc.appendChild(root);           //添加根结点

    QDomElement seg;// = rwdoc.createElement("字段");
    QDomAttr id;// = rwdoc.createAttribute("字段编号");
    QDomElement name;// = rwdoc.createElement("字段名称");
    QDomElement len;// = rwdoc.createElement("字段长度");
    QDomElement unit;// = rwdoc.createElement("单位");
    QDomElement precision;// = rwdoc.createElement("精度");
    QDomElement offset;// = rwdoc.createElement("偏移");
    QDomText text;

    int rowCount;
    rowCount = themodel->rowCount();
    for(int i = 0; i < rowCount; i++) {
        seg = writedoc.createElement("字段");
        id = writedoc.createAttribute("字段编号");
        QString idstr = themodel->data(themodel->index(i, 0)).toString();
        id.setValue(idstr);
        seg.setAttributeNode(id);   //设置id

        name = writedoc.createElement("字段名称");
        QString namestr = themodel->data(themodel->index(i, 1)).toString();
        text = writedoc.createTextNode(namestr);  //添加字段名
        name.appendChild(text);

        len = writedoc.createElement("字段长度");
        QString lenstr = themodel->data(themodel->index(i, 2)).toString();
        text = writedoc.createTextNode(lenstr);
        len.appendChild(text);

        unit = writedoc.createElement("单位");
        QString unitstr = themodel->data(themodel->index(i, 3)).toString();
        text = writedoc.createTextNode(unitstr);
        unit.appendChild(text);

        precision = writedoc.createElement("精度");
        QString precstr = themodel->data(themodel->index(i, 4)).toString();
        text = writedoc.createTextNode(precstr);
        precision.appendChild(text);

        offset = writedoc.createElement("偏移");
        QString offsetstr = themodel->data(themodel->index(i, 5)).toString();
        text = writedoc.createTextNode(offsetstr);
        offset.appendChild(text);

        seg.appendChild(name);
        seg.appendChild(len);
        seg.appendChild(unit);
        seg.appendChild(precision);
        seg.appendChild(offset);
        root.appendChild(seg);
    }
    QTextStream out(&file);
    writedoc.save(out, 4);
    file.close();
}

void RuleEditer::on_act_Insert_triggered()
{
    int curRow = ui->tableView->currentIndex().row();
    int id = curRow;
    int init_precision = 1;
    int init_offset = 0;
    themodel->insertRow(curRow);
    themodel->setData(themodel->index(curRow, 0), id);   //插入新行自动设置id
    themodel->setData(themodel->index(curRow, 4), init_precision); //精度初始为1
    themodel->setData(themodel->index(curRow, 5), init_offset);
}

void RuleEditer::on_act_Apend_triggered()
{
    int rowCount = themodel->rowCount();
    int id = rowCount;
    int init_precision = 1;
    int init_offset = 0;
    themodel->insertRow(rowCount);
    themodel->setData(themodel->index(rowCount, 0), id); //id自动设置为行号
    themodel->setData(themodel->index(rowCount, 4), init_precision); //精度初始为1
    themodel->setData(themodel->index(rowCount, 5), init_offset);
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
    themodel->setSort(0, Qt::AscendingOrder);   //按照第一列id升序排序
    themodel->submitAll();
    saveTabletoXml();
    showXml();
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
    qDebug() << "newtable:" << tablename << endl;
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
    intXmlFile();
}

void RuleEditer::on_act_Open_triggered()
{
    QStringList tablelist;
    tablelist = db.tables();
    bool ok;
    QString tablename = QInputDialog::getItem(this, QString("打开规则表"), QString("选择表"), tablelist, 0, false, &ok);
    if (ok && !tablename.isEmpty()) {
        qDebug() << "open table:" << tablename << endl;
        themodel->setTable(tablename);
        themodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        themodel->select();
        curtable = tablename;
        intXmlFile();
    }
}

void RuleEditer::on_act_Drop_triggered()
{
    QStringList tablelist;
    tablelist = db.tables();
    bool ok;
    QString tablename = QInputDialog::getItem(this, QString("删除规则表"), QString("选择表"), tablelist, 0, false, &ok);
    if (ok && !tablename.isEmpty()) {
        QSqlQuery query;
        if (!query.exec(QString("drop table %1").arg(tablename))) {
            qDebug() << "drop table:" << tablename << " failed" << endl;
        }
    }
}

void RuleEditer::on_act_Aplly_triggered()
{
    MainWindow *parWind = (MainWindow*)parentWidget();
    parWind->parseXml(xmlfilename);
}
