#include "stdafx.h"
#include "gaxe_dialog.h"
#include "ui_gaxe_dialog.h"

GAxe_dialog::GAxe_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GAxe_dialog)
{
    ui->setupUi(this);
}

GAxe_dialog::~GAxe_dialog()
{
    delete ui;
}

void GAxe_dialog::on_pushButton_clicked()
{

}
