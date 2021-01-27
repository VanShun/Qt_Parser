#ifndef RULEEDITER_H
#define RULEEDITER_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include <QDebug>

#define FIXED_CULUMCOUNT     5

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

private slots:
    void on_act_Insert_triggered();

    void on_act_Apend_triggered();

    void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::RuleEditer *ui;
    QSqlTableModel *themodel;
    //QItemSelectionModel *theselect;
};

#endif // RULEEDITER_H
