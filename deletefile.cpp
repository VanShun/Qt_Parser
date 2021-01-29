#include "deletefile.h"
#include "ui_deletefile.h"

DeleteFile::DeleteFile(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DeleteFile)
{
    ui->setupUi(this);
}

DeleteFile::~DeleteFile()
{
    delete ui;
}
