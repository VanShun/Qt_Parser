#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->plainTextEdit->setReadOnly(true);
}

About::~About()
{
    delete ui;
}
