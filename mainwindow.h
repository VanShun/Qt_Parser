#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml>
#include <QTextDocument>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void parseXml(QString filename);  //xml文件解析
    void parseSeg(QString segname, QString lenstr, QString unitstr, QString precsionstr, QString offsetstr, QString remarksstr);
    int parseRegExp(QString regstr);  //按照正则表达式处理字运算字符串，返回计算结果
    QString getVluestr(int i);
    void curTableChanged(QString tablename);
private slots:
    void on_act_OpenEditer_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString xmlfilename;
    QString orgstr;
    QStringList valuestrlist;
    int offset;
    QLabel statuslabel;
};
#endif // MAINWINDOW_H
