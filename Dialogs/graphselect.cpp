#include "stdafx.h"
#include "graphselect.h"
#include "ui_graphselect.h"
#include "Accumulation.h"
#include "Dialogs/treemodel.h"
#include <QtGui>
#include <QtWidgets>

GraphSelect::GraphSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphSelect)
{
    ui->setupUi(this);
	ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_nBufIndex = -1;
    connect(ui->treeView, &QGridTree::onSignalAccepted, this, &GraphSelect::onSignalAccepted);

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	QSize	size	= settings.value("QGridTree/size", QSize(536, 235)).toSize();
	resize(size);
}

GraphSelect::~GraphSelect()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.setValue("QGridTree/size", size());
	settings.sync();

    delete ui;
}

void	GraphSelect::SetAccumulation(const vector<Accumulation*>* pBuffer)
{
	//Загружаем буфер в модель
	TreeModel*	model	= new TreeModel;
    model->loadAcc(pBuffer);
	ui->treeView->setModel(model);
    ui->treeView->setCurrentIndex(model->index(0,0));

    //Если накопление одно, то раскроем его
    if(pBuffer->size() == 1)
    {
        ui->treeView->expand(model->index(0,0));
    }
}

void    GraphSelect::onSignalAccepted(int nBufIndex, QString path)
{
	//Запоминаем выделенный элемент
    m_nBufIndex = nBufIndex;
    m_Path		= path;

    accept();
}

void	GraphSelect::SetPath(QString path, int nAcc)
{
	ui->treeView->expandTo(path, nAcc);
}
