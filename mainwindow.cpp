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


void MainWindow::on_act_OpenEditer_triggered()
{
    RuleEditer *editer = new RuleEditer();
    editer->setWindowState(Qt::WindowNoState);
    editer->setAttribute(Qt::WA_DeleteOnClose);
    editer->show();
}
