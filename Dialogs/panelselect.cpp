#include "stdafx.h"
#include "panelselect.h"
#include "ui_panelselect.h"

PanelSelect::PanelSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelSelect)
{
    ui->setupUi(this);

}

PanelSelect::~PanelSelect()
{
    delete ui;
}
