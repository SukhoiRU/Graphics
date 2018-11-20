#include "stdafx.h"
#include "graphselect.h"
#include "../GeneratedFiles/ui_graphselect.h"
#include "ui_graphselect.h"
#include "Accumulation.h"
#include "Dialogs/treemodel.h"

GraphSelect::GraphSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphSelect)
{
    ui->setupUi(this);
	ui->treeView->setAlternatingRowColors(true);
	ui->treeView->setHeaderHidden(true);

	QHeaderView*	h	= ui->treeView->header();
	h->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->treeView, &QGridTree::onSignalAccepted, this, &GraphSelect::onSignalAccepted);
    m_nBufIndex = -1;
    m_nAccIndex = -1;
}

GraphSelect::~GraphSelect()
{
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

void    GraphSelect::onSignalAccepted(int nBufIndex, int nAccIndex)
{
    m_nBufIndex = nBufIndex;
    m_nAccIndex = nAccIndex;
    accept();
}
