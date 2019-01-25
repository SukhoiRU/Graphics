#include "stdafx.h"
#include "qgridtree.h"
#include <QPainter>
#include <QPaintEvent>
#include <QHeaderView>
#include "treeitem.h"
#include <QTextCodec>
#include "treemodel.h"
#include <QtGui>
#include <QtWidgets>

QGridTree::QGridTree(QWidget *parent) :
    QTreeView(parent)
{
	m_bHeader	= false;
	m_bAutoSize	= true;
	m_bGrid		= true;
	m_bAnimated	= true;

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	m_bHeader	= settings.value("QGridTree/m_bHeader", false).toBool();
	m_bAutoSize	= settings.value("QGridTree/m_bAutoSize", true).toBool();
	m_bGrid		= settings.value("QGridTree/m_bGrid", false).toBool();
	m_bAnimated	= settings.value("QGridTree/m_bAnimated", true).toBool();
	m_bAlternate	= settings.value("QGridTree/m_bAlternate", true).toBool();

	QHeaderView*	h	= header();
	if(m_bAutoSize)	h->setSectionResizeMode(QHeaderView::ResizeToContents);
	else			h->setSectionResizeMode(QHeaderView::Interactive);
	setHeaderHidden(!m_bHeader);
	setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
	setAlternatingRowColors(m_bAlternate);
	setUniformRowHeights(true);
    setAutoFillBackground(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAnimated(m_bAnimated);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QTreeView::customContextMenuRequested, this, &QGridTree::onCustomMenuRequested);
}

QGridTree::~QGridTree()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.setValue("QGridTree/m_bHeader", m_bHeader);
	settings.setValue("QGridTree/m_bAutoSize", m_bAutoSize);
	settings.setValue("QGridTree/m_bGrid", m_bGrid);
	settings.setValue("QGridTree/m_bAnimated", m_bAnimated);

	settings.sync();
}

void	QGridTree::paintEvent(QPaintEvent* evnt)
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

void QGridTree::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
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

void	QGridTree::keyPressEvent(QKeyEvent* evnt)
{
	QModelIndex index = currentIndex();
	if(!index.isValid())	return;
	switch(evnt->key())
	{
	case Qt::Key_Enter:
	case Qt::Key_Return:    onAccept(); break;
	case Qt::Key_C:
	{
		if(evnt->modifiers() == Qt::KeyboardModifier::ControlModifier)
		{
			//Копируем в буфер
			TreeItem*	item	= static_cast<TreeItem*>(index.internalPointer());
			TreeItem::Data*	data	= item->GetData();
			if(data->nBufIndex != -1 && data->nAccIndex != -1)
			{
				QString	line	= data->typeName() + "\t\t" + data->name + ";\t\t\t//" + data->comm + "\n";
				QClipboard*	c	= QGuiApplication::clipboard();
				c->setText(line);
				evnt->accept();
				return;
			}
		}
	}break;
	}

	QTreeView::keyPressEvent(evnt);
}

void    QGridTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = currentIndex();
    if(!index.isValid())    return;
    onAccept();
    QTreeView::mouseDoubleClickEvent(event);
}

void    QGridTree::onAccept()
{
    QModelIndex index = currentIndex();
    TreeItem*	item	= static_cast<TreeItem*>(index.internalPointer());
    const TreeItem::Data*	data	= item->GetData();
    if(data->nBufIndex != -1 && data->nAccIndex != -1)
    {
        emit onSignalAccepted(data->nBufIndex, data->nAccIndex);
    }
}

void	QGridTree::onCustomMenuRequested(QPoint pos)
{
	QMenu*		menu			= new QMenu(this);
	QAction*	actHeader		= new QAction("Заголовок", this);
	QAction*	actAutoSize		= new QAction("Размер по содержимому", this);	
	QAction*	actShowGrid		= new QAction("Сетка", this);		
	QAction*	actAnimation	= new QAction("Анимация", this);	
	QAction*	actAlternate	= new QAction("Полоски", this);	
	QAction*	actCopyPath		= new QAction("Копировать в С++", this);

	actHeader->setCheckable(true);
	actAutoSize->setCheckable(true);
	actShowGrid->setCheckable(true);
	actAnimation->setCheckable(true);
	actAlternate->setCheckable(true);

	actHeader->setChecked(m_bHeader);
	actAutoSize->setChecked(m_bAutoSize);
	actShowGrid->setChecked(m_bGrid);
	actAnimation->setChecked(m_bAnimated);
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
	connect(actAnimation, &QAction::toggled, [=](bool bAnimation){m_bAnimated = bAnimation; setAnimated(m_bAnimated);});
	connect(actAlternate, &QAction::toggled, [=](bool bAlternate){m_bAlternate = bAlternate; setAlternatingRowColors(m_bAlternate);});
	connect(actCopyPath, &QAction::triggered, [=]()
	{
		//Копируем в буфер
		QModelIndex index = currentIndex();
		if(!index.isValid())	return;
		TreeItem*	item	= static_cast<TreeItem*>(index.internalPointer());
		TreeItem::Data*	data	= item->GetData();
		if(data->nBufIndex != -1 && data->nAccIndex != -1)
		{
			QString	line	= data->typeName() + "\t\t" + data->name + ";\t\t\t//" + data->comm + "\n";
			QClipboard*	c	= QGuiApplication::clipboard();
			c->setText(line);
		}
	});

	menu->addAction(actHeader);
	menu->addAction(actAutoSize);
	menu->addAction(actShowGrid);
	menu->addAction(actAnimation);
	menu->addAction(actAlternate);
	menu->addSeparator();
	menu->addAction(actCopyPath);
	menu->popup(viewport()->mapToGlobal(pos));
}

