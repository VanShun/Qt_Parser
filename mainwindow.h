#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml>
#include <QTextDocument>

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
    void parseSeg(QString segname, QString lenstr, QString unitstr, QString precsionstr, QString offsetstr);

private slots:
    void on_act_OpenEditer_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString xmlfilename;
    QString orgstr;
    int offset;
};
#endif // MAINWINDOW_H
