#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ruleediter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
    //setStyleSheet("QComboBox::drop-down{image:url(:/sheets.png)}");
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

void MainWindow::parseSeg(QString segname, QString lenstr, QString unitstr, QString precsionstr, QString offsetstr, QString remarksstr)
{
    qDebug() << QString("parseseg, segname:%1, lenstr:%2, unitstr:%3, precsionstr:%4, offsetstr:%5").
                arg(segname).arg(lenstr).arg(unitstr).arg(precsionstr).arg(offsetstr);
    int seglen = 0;
    if (lenstr == "0" || lenstr == 0) {
        seglen = parseRegExp(remarksstr);
        qDebug() << "seglen: " << seglen << endl;
    } else {
        seglen = lenstr.toInt();
    }
    if (offset + seglen * 2 > orgstr.size()) {  //1字节两字符
        qDebug() << QString("err,seglen:%1, offset:%2").arg(seglen).arg(offset);
        QMessageBox::warning(this, QString("错误提示"), QString("数据长度错误"));
        return;
    }
    QString tostr = segname + ": ";
    QString valuestr = orgstr.mid(offset, seglen * 2);
    int orgvalue;
    double realvalue, precison, valueoffset;

    if (unitstr.isEmpty()) {
        if (!remarksstr.isEmpty()) {
            if (remarksstr == QString("字符串")) {   //字符串，将十六进制ASCII码解析成字符串展示出来
                QByteArray orgarray = valuestr.toLatin1();
                QByteArray convertarray = orgarray.fromHex(orgarray);
                valuestr = convertarray;
            } else {
                valuestr += QString(" 备注: %1").arg(remarksstr);
            }
        }
        tostr += valuestr;
    } else {        //有单位的数值型数据，需要根据精度偏移计算实际值
        orgvalue = valuestr.toInt(nullptr ,16);
        precison = precsionstr.toFloat();
        valueoffset = offsetstr.toFloat();
        realvalue = orgvalue * precison + valueoffset;
        qDebug() << QString("orgvalue:%1, precison:%2, valueoffset:%3, realvalue:%4").
                    arg(orgvalue).arg(precison).arg(valueoffset).arg(realvalue);
        valuestr = QString("%1").arg(realvalue);
        tostr += valuestr;
        tostr += QString(" 单位：%1").arg(unitstr);
        if (!remarksstr.isEmpty()) {
            tostr += QString(" 备注: %1").arg(remarksstr);
        }
    }
    valuestrlist << valuestr;

    ui->plainTextEdit_2->appendPlainText(tostr);
    offset += seglen * 2;
}

int MainWindow::parseRegExp(QString regstr)
{
    QRegExp rxlen("(^\\$)(\\d+)(\\+|\\-|\\*|\\/)(\\d+)");
    regstr.remove(QRegExp("\\s*"));
    qDebug() << QString("regstr:%1").arg(regstr);
    int pos = rxlen.indexIn(regstr);
    qDebug() << QString("pos0:%1, pos1:%2, pos2:%3, pos3:%4").arg(rxlen.pos(0)).arg(rxlen.pos(1)).arg(rxlen.pos(2)).arg(rxlen.pos(3));
    int result = 0;
    if (pos > -1) {
        qDebug() << rxlen.cap(0);
        QString value1str = rxlen.cap(2);
        QString op = rxlen.cap(3);
        QString value2str = rxlen.cap(4);
        int count = rxlen.captureCount();
        qDebug() << QString("value1:%1, op:%2, value2:%3, count:%4").arg(value1str).arg(op).arg(value2str).arg(count);
        int i = value1str.toInt();
        QString ivaluestr = valuestrlist.at(i - 1);
        int ivalue = ivaluestr.toInt();
        int value2 = value2str.toInt();
        if (op == "+") {
            result = ivalue + value2;
        } else if (op == "-") {
            result = ivalue - value2;
        } else if (op == "*") {
            result = ivalue * value2;
        } else if (op == "/") {
            result = ivalue * value2;
        }
    } else qDebug() << "no match" << endl;
    qDebug() << "result: " << result << endl;
    return result;
}

QString MainWindow::getVluestr(int i)
{
    return valuestrlist.at(i);
}

void MainWindow::curTableChanged(QString tablename)
{
    statuslabel.clear();
    statuslabel.setText(QString("    当前运用的规则表: %1").arg(tablename));
    ui->statusbar->addWidget((QWidget *)&statuslabel);
    ui->pushButton->setEnabled(true);
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
    QString segname, lenstr, unitstr, precsionstr, offsetstr, remarksstr;
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
                    } else if (n.toElement().tagName() == QString("备注")) {
                        remarksstr = n.toElement().text();
                    }
                }
            }
        }
        parseSeg(segname, lenstr, unitstr, precsionstr, offsetstr, remarksstr);
        node = node.nextSibling();
    }
}
