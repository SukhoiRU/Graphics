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
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("LocatorView");
	m_bHeader	= settings.value("m_bHeader", false).toBool();
	m_bAutoSize	= settings.value("m_bAutoSize", true).toBool();
	m_bGrid		= settings.value("m_bGrid", false).toBool();
	m_bAnimated	= settings.value("m_bAnimated", true).toBool();
	m_bAlternate	= settings.value("m_bAlternate", true).toBool();
	m_bUseBool	= settings.value("m_bUseBool", true).toBool();
	m_bUseEmpty	= settings.value("m_bUseEmpty", true).toBool();
	settings.endGroup();

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
	oldTime	= 0;
}

LocatorView::~LocatorView()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("LocatorView");
	settings.setValue("m_bHeader", m_bHeader);
	settings.setValue("m_bAutoSize", m_bAutoSize);
	settings.setValue("m_bGrid", m_bGrid);
	settings.setValue("m_bAnimated", m_bAnimated);
	settings.setValue("m_bAlternate", m_bAlternate);
	settings.setValue("m_bUseBool", m_bUseBool);
	settings.setValue("m_bUseEmpty", m_bUseEmpty);
	settings.endGroup();
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
	QAction*	actBool			= new QAction("Показывать bool", this);
	QAction*	actEmpty		= new QAction("Показывать пустые", this);

	actHeader->setCheckable(true);
	actAutoSize->setCheckable(true);
	actShowGrid->setCheckable(true);
	actAlternate->setCheckable(true);
	actBool->setCheckable(true);
	actEmpty->setCheckable(true);

	actHeader->setChecked(m_bHeader);
	actAutoSize->setChecked(m_bAutoSize);
	actShowGrid->setChecked(m_bGrid);
	actAlternate->setChecked(m_bAlternate);
	actBool->setChecked(m_bUseBool);
	actEmpty->setChecked(m_bUseEmpty);

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
	connect(actBool, &QAction::toggled, [=](bool bUseBool){m_bUseBool = bUseBool; on_panelChanged(m_axes); on_timeChanged(oldTime);});
	connect(actEmpty, &QAction::toggled, [=](bool bUseEmpty){m_bUseEmpty = bUseEmpty; on_panelChanged(m_axes); on_timeChanged(oldTime);});

	menu->addAction(actHeader);
	menu->addAction(actAutoSize);
	menu->addAction(actShowGrid);
	menu->addAction(actAlternate);
	menu->addSeparator();
	menu->addAction(actBool);
	menu->addAction(actEmpty);
	menu->popup(viewport()->mapToGlobal(pos));
}

void	LocatorView::on_panelChanged(vector<Graph::GAxe*>* axes)
{
	m_axes	= axes;
	//Создаем новую модель
	if(m_model)	delete m_model;
	m_model	= new LocatorModel;
	m_model->setPanel(axes, m_bUseBool, m_bUseEmpty);
	setModel(m_model);
	m_model->updateModel(oldTime);
}

void	LocatorView::on_timeChanged(double time)
{
	if(m_model)
	{
		oldTime	= time;
		m_model->updateModel(time);
	}
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
