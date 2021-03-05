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
