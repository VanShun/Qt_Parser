#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ruleediter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseXml(QString filename)
{
    qDebug() << "parseXml" << endl;
    xmlfilename = filename;
}

void MainWindow::parseSeg(QString segname, QString lenstr, QString unitstr, QString precsionstr, QString offsetstr)
{
    qDebug() << QString("parseseg, segname:%1, lenstr:%2, unitstr:%3, precsionstr:%4, offsetstr:%5").
                arg(segname).arg(lenstr).arg(unitstr).arg(precsionstr).arg(offsetstr);
    int seglen = lenstr.toInt();
    if (offset + seglen * 2 > orgstr.size()) {  //1字节两字符
        qDebug() << QString("err,seglen:%1, offset:%2").arg(seglen).arg(offset);
        QMessageBox::warning(this, QString("错误提示"), QString("数据长度错误"));
        return;
    }
    QString tostr = segname + ": ";
    QString valuestr = orgstr.mid(offset, seglen * 2);
    double orgvalue, realvalue, precison, valueoffset;

    if (unitstr.isEmpty()) {
        tostr += valuestr;
    } else {        //有单位的数值型数据，需要根据精度偏移计算实际值
        orgvalue = valuestr.toFloat();
        precison = precsionstr.toFloat();
        valueoffset = offsetstr.toFloat();
        realvalue = orgvalue * precison + valueoffset;
        qDebug() << QString("orgvalue:%1, precison:%2, valueoffset:%3, realvalue:%4").
                    arg(orgvalue).arg(precison).arg(valueoffset).arg(realvalue);
        valuestr = QString("%1").arg(realvalue);
        tostr += valuestr;
        tostr += QString(" 单位：%1").arg(unitstr);
    }

    ui->plainTextEdit_2->appendPlainText(tostr);
    offset += seglen * 2;
}


void MainWindow::on_act_OpenEditer_triggered()
{
    RuleEditer *editer = new RuleEditer(this);
    editer->setWindowState(Qt::WindowNoState);
    editer->setAttribute(Qt::WA_DeleteOnClose);
    editer->show();
}

void MainWindow::on_pushButton_clicked()
{
    QFile file(xmlfilename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "parse open file failed" << endl;
        return;
    }
    QDomDocument readdoc;
    bool namespaceProcessing = true;
    QString *errorMsg = nullptr;
    int *errorLine = nullptr;
    int *errorColumn = nullptr;
    if (!readdoc.setContent(&file, namespaceProcessing, errorMsg, errorLine, errorColumn)) {
        qDebug() << "parse setcontent failed!" << endl;
        qDebug() << "errorMsg: " << errorMsg << endl;
        file.close();
        return;
    }
    file.close();

    ui->plainTextEdit_2->clear();
    orgstr = ui->plainTextEdit->toPlainText();
    orgstr.remove(QRegExp("\\s"));
    qDebug() << "orgstr:" << orgstr << "size: " << orgstr.size() << endl;
    offset = 0;

    //遍历xml文件
    QString segname, lenstr, unitstr, precsionstr, offsetstr;
    QDomElement docElement = readdoc.documentElement();
    QDomNode node = docElement.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement e = node.toElement();
            QDomNodeList list = e.childNodes();
            for (int i = 0; i < list.count(); i++) {
                QDomNode n = list.at(i);
                if (n.isElement()) {
                    if (n.toElement().tagName() == QString("字段名称")) {
                        segname = n.toElement().text();
                    } else if (n.toElement().tagName() == QString("字段长度")) {
                        lenstr = n.toElement().text();
                    } else if (n.toElement().tagName() == QString("单位")) {
                        unitstr = n.toElement().text();
                    } else if (n.toElement().tagName() == QString("精度")) {
                        precsionstr = n.toElement().text();
                    } else if (n.toElement().tagName() == QString("偏移")) {
                        offsetstr = n.toElement().text();
                    }
                }
            }
        }
        parseSeg(segname, lenstr, unitstr, precsionstr, offsetstr);
        node = node.nextSibling();
    }
}
