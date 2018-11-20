#include "stdafx.h"
#include "qgridtree.h"
#include <QPainter>
#include <QPaintEvent>
#include <QHeaderView>
#include "treeitem.h"
#include <QTextCodec>
#include "treemodel.h"

QGridTree::QGridTree(QWidget *parent) :
    QTreeView(parent)
{
	QHeaderView*	h	= header();
    h->setSectionResizeMode(QHeaderView::ResizeToContents);
	h->setMinimumSectionSize(50);
	
    setHeaderHidden(true);
	setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
	setAlternatingRowColors(true);
	setUniformRowHeights(true);
    setAutoFillBackground(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAnimated(false);
}

void	QGridTree::paintEvent(QPaintEvent* evnt)
{
	QTreeView::paintEvent(evnt);

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

void QGridTree::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
	QTreeView::drawRow(painter,options,index);

	int h	= height();
	int	w	= width();
	int	b	= options.rect.bottom();
	int	hl	= options.rect.height();
	m_nRowHeight	= hl;

	//Горизонтальная линия
	QPalette p = palette();
	painter->setPen(QPen(p.color(QPalette::Dark),1,Qt::SolidLine));
    //painter->drawLine(0, b, w, b);

	//Проверяем, что под этим рядом ничего нет
	if(!indexAt(QPoint(0,b+hl)).isValid())
	{
		//Рисуем оставшиеся линии
		for(int y = b; y < h; y += hl)
		{
			painter->drawLine(0,y,w,y);
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
