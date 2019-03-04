#include "stdafx.h"
#include "locator_view.h"
#include "locator_item.h"
#include "locator_model.h"
#include <QPainter>
#include <QPaintEvent>
#include <QHeaderView>
#include <QTextCodec>
#include "locator_model.h"
#include <QtGui>
#include <QtWidgets>
#include "Graph/GAxe.h"

LocatorView::LocatorView(QWidget *parent) : QTreeView(parent)
{
	m_bHeader	= false;
	m_bAutoSize	= true;
	m_bGrid		= true;
	m_bAnimated	= true;

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	m_bHeader	= settings.value("LocatorView/m_bHeader", false).toBool();
	m_bAutoSize	= settings.value("LocatorView/m_bAutoSize", true).toBool();
	m_bGrid		= settings.value("LocatorView/m_bGrid", false).toBool();
	m_bAnimated	= settings.value("LocatorView/m_bAnimated", true).toBool();
	m_bAlternate	= settings.value("LocatorView/m_bAlternate", true).toBool();

	QHeaderView*	h	= header();
	if(m_bAutoSize)	h->setSectionResizeMode(QHeaderView::ResizeToContents);
	else			h->setSectionResizeMode(QHeaderView::Interactive);
	setHeaderHidden(!m_bHeader);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setAlternatingRowColors(m_bAlternate);
	setUniformRowHeights(true);
    setAutoFillBackground(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::NoSelection);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAnimated(m_bAnimated);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &LocatorView::customContextMenuRequested, this, &LocatorView::onCustomMenuRequested);

	m_model	= nullptr;
}

LocatorView::~LocatorView()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.setValue("LocatorView/m_bHeader", m_bHeader);
	settings.setValue("LocatorView/m_bAutoSize", m_bAutoSize);
	settings.setValue("LocatorView/m_bGrid", m_bGrid);
	settings.setValue("LocatorView/m_bAnimated", m_bAnimated);

	settings.sync();

	if(m_model)	delete m_model;
}

void	LocatorView::paintEvent(QPaintEvent* evnt)
{
	QTreeView::paintEvent(evnt);

	if(m_bGrid)
	{
		QPainter painter(viewport());
		QPalette p = palette();
		painter.setPen(QPen(p.color(QPalette::Dark), 1, Qt::SolidLine));

		//Рисуем вертикальные линии до самого конца
		int h	= height();

		//Вертикальные линии
		QHeaderView*	head	= header();
		int nColumns	= head->count();
		for(int i = 1; i < nColumns; i++)
		{
			int x	= head->sectionViewportPosition(i);
			painter.drawLine(x, 0, x, h);
		}
	}
}

void LocatorView::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
	QTreeView::drawRow(painter,options,index);

	if(m_bGrid)
	{
		int h	= height();
		int	w	= width();
		int	b	= options.rect.bottom();
		int	hl	= options.rect.height();

		//Горизонтальная линия
		QPalette p = palette();
		painter->setPen(QPen(p.color(QPalette::Dark), 1, Qt::SolidLine));
		painter->drawLine(0, b, w, b);

		//Проверяем, что под этим рядом ничего нет
		if(!indexAt(QPoint(0, b+hl)).isValid())
		{
			//Рисуем оставшиеся линии
			for(int y = b; y < h; y += hl)
			{
				painter->drawLine(0, y, w, y);
			}
		}
	}
}

void	LocatorView::onCustomMenuRequested(QPoint pos)
{
	QMenu*		menu			= new QMenu(this);
	QAction*	actHeader		= new QAction("Заголовок", this);
	QAction*	actAutoSize		= new QAction("Размер по содержимому", this);	
	QAction*	actShowGrid		= new QAction("Сетка", this);		
	QAction*	actAlternate	= new QAction("Полоски", this);	

	actHeader->setCheckable(true);
	actAutoSize->setCheckable(true);
	actShowGrid->setCheckable(true);
	actAlternate->setCheckable(true);

	actHeader->setChecked(m_bHeader);
	actAutoSize->setChecked(m_bAutoSize);
	actShowGrid->setChecked(m_bGrid);
	actAlternate->setChecked(m_bAlternate);

	connect(actHeader, &QAction::toggled, [=](bool bHeader){m_bHeader	= bHeader; setHeaderHidden(!m_bHeader);});
	connect(actAutoSize, &QAction::toggled, [=](bool bAutoSize)
	{
		m_bAutoSize	= bAutoSize;
		QHeaderView*	h	= header();
		if(m_bAutoSize)	h->setSectionResizeMode(QHeaderView::ResizeToContents);
		else			h->setSectionResizeMode(QHeaderView::Interactive);
	});
	connect(actShowGrid, &QAction::toggled, [=](bool bGrid){m_bGrid	= bGrid;});
	connect(actAlternate, &QAction::toggled, [=](bool bAlternate){m_bAlternate = bAlternate; setAlternatingRowColors(m_bAlternate);});

	menu->addAction(actHeader);
	menu->addAction(actAutoSize);
	menu->addAction(actShowGrid);
	menu->addAction(actAlternate);
	menu->popup(viewport()->mapToGlobal(pos));
}

void	LocatorView::on_panelChanged(vector<Graph::GAxe*>* axes)
{
	//Создаем новую модель
	if(m_model)	delete m_model;
	m_model	= new LocatorModel;
	m_model->setPanel(axes);
	setModel(m_model);
}

void	LocatorView::on_timeChanged(double time)
{
	if(m_model)
		m_model->updateModel(time);
}

void	LocatorView::on_axeSelected(bool bSelected)
{
	if(m_model)
		m_model->setSelected(bSelected);
}

void	LocatorView::on_axesMoved()
{
	//Пересортируем список осей
	if(m_model)
		m_model->resortAxes();
}

void	LocatorView::on_axesRenamed()
{
	if(m_model)
		m_model->updateNames();
}
