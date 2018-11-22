#include "stdafx.h"
#include "pageSetup.h"
#include "ui_PageSetup.h"
#include <QKeyEvent>

PageSetup::PageSetup(QWidget *parent) :
QDialog(parent),
ui(new Ui::Dialog)
{
	ui->setupUi(this);

	connect(this, &PageSetup::accepted, this, &PageSetup::update);
	connect(ui->applyButton, &QPushButton::clicked, this, &PageSetup::update);
}

PageSetup::~PageSetup()
{
	delete ui;
}

void PageSetup::load_data()
{
	ui->pageWidth->setText(QString("%1").arg(pageSize.width()));
	ui->pageHeight->setText(QString("%1").arg(pageSize.height()));

	ui->borderLeft->setText(QString("%1").arg(pageBorders.left()));
	ui->borderRight->setText(QString("%1").arg(pageBorders.right()));
	ui->borderTop->setText(QString("%1").arg(pageBorders.top()));
	ui->borderBottom->setText(QString("%1").arg(pageBorders.bottom()));

	ui->graphLeft->setText(QString("%1").arg(graphBorders.left()));
	ui->graphRight->setText(QString("%1").arg(graphBorders.right()));
	ui->graphTop->setText(QString("%1").arg(graphBorders.top()));
	ui->graphBottom->setText(QString("%1").arg(graphBorders.bottom()));

	ui->gridWidth->setText(QString("%1").arg(gridStep.width()));
	ui->gridHeight->setText(QString("%1").arg(gridStep.height()));

    ui->pageZoom->setText(QString("%1").arg(zoom));
    ui->zoomSlider->setValue(zoom*100.f);
}

void	PageSetup::update()
{
	pageSize.setWidth(ui->pageWidth->text().toInt());
	pageSize.setHeight(ui->pageHeight->text().toInt());

	pageBorders.setLeft(ui->borderLeft->text().toInt());
	pageBorders.setRight(ui->borderRight->text().toInt());
	pageBorders.setTop(ui->borderTop->text().toInt());
	pageBorders.setBottom(ui->borderBottom->text().toInt());

	graphBorders.setLeft(ui->graphLeft->text().toInt());
	graphBorders.setRight(ui->graphRight->text().toInt());
	graphBorders.setTop(ui->graphTop->text().toInt());
	graphBorders.setBottom(ui->graphBottom->text().toInt());

	gridStep.setWidth(ui->gridWidth->text().toInt());
	gridStep.setHeight(ui->gridHeight->text().toInt());

	emit onApply();
}

void PageSetup::on_zoomSlider_valueChanged(int value)
{
    float	scale	= value/100.f;
    if(zoom != scale)
    {
        zoom    = scale;
        ui->pageZoom->setText(QString("%1").arg(scale));

        emit onZoomChanged(scale);
    }
}

void PageSetup::on_pageZoom_editingFinished()
{
    float scale = ui->pageZoom->text().toFloat()*100.;
    ui->zoomSlider->setValue(scale);
}
