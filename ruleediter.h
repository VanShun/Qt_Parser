#ifndef RULEEDITER_H
#define RULEEDITER_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include <QDebug>
#include <QInputDialog>
#include <QSqlQuery>
#include <QMessageBox>
#include <QtXml>
#include <QSqlError>

//#define FIXED_CULUMCOUNT     5

namespace Ui {
class RuleEditer;
}

class RuleEditer : public QMainWindow
{
    Q_OBJECT

public:
    explicit RuleEditer(QWidget *parent = nullptr);
    ~RuleEditer();

    void initEditer(void);

    void intXmlFile(void);

    void showXml(void);

    void saveTabletoXml(void);

private slots:
    void on_act_Insert_Above_triggered();

    void on_act_Insert_Under_triggered();

    void on_act_Apend_triggered();

    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void on_act_New_triggered();

    void on_act_Open_triggered();

    void on_act_Delete_triggered();

    void on_act_Undo_triggered();

    void on_act_Save_triggered();

    void on_act_Saveto_triggered();

    void on_act_trunc_triggered();

    void on_act_Drop_triggered();

    void on_act_Aplly_triggered();

    void on_act_help_triggered();

private:
    Ui::RuleEditer *ui;
    QSqlTableModel *themodel;
    QSqlDatabase db;
    QString curtable;
    QString xmlfilename;
    //QLabel status;
};

#endif // RULEEDITER_H
