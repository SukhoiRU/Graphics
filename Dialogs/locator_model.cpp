#include "stdafx.h"
#include "locator_item.h"
#include "locator_model.h"
#include "Graph/GAxe.h"
#include <QtGui>
#include <QtWidgets>

LocatorModel::LocatorModel(QObject *parent): QAbstractItemModel(parent)
{
	LocatorItem::Data	data;
	data.name	= "Параметр";
    data.value	= "Значение";
	data.pAxe	= nullptr;
	m_rootItem	= new LocatorItem(data);

	m_bHasSelected	= false;
	bUseTimeLeft	= false;
	timeLeft		= 0;
}

LocatorModel::~LocatorModel()
{
    delete m_rootItem;
}

void	LocatorModel::setPanel(const std::vector<Graph::GAxe*>* axes, bool bUseBool, bool bUseEmpty)
{
	//Очищаем старые данные
	m_rootItem->clear();

	//Загружаем новые
	for(size_t i = 0; i < axes->size(); i++)
	{
		Graph::GAxe*	pAxe	= axes->at(i);
		LocatorItem::Data	data;
		data.name	= pAxe->m_Name;
		data.pAxe	= pAxe;

		if(!pAxe->IsBoolean() || (pAxe->IsBoolean() && bUseBool))
			if(!pAxe->isEmpty() || (pAxe->isEmpty() && bUseEmpty))
				m_rootItem->addChildren(data);
	}

	//Сортируем список по высоте осей
	m_rootItem->sortAxes();

	//Запоминаем индексы для обновления
	i_Start	= index(0, 1);
	i_Stop	= index(rowCount(QModelIndex())-1, 1);
}

int LocatorModel::columnCount(const QModelIndex & /* parent */) const
{
    return m_rootItem->columnCount();
}

QVariant LocatorModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())	return QVariant();

	LocatorItem*		item	= getItem(index);
	const LocatorItem::Data*	pData	= item->GetData();

	switch(role)
	{
	case Qt::DisplayRole:
		{
			return item->data(index.column());
		}break;

	case Qt::TextColorRole:
		{
			//Определяем цвет текста
			vec3	c	= pData->pAxe->m_Color;
			if(m_bHasSelected && !pData->pAxe->m_IsSelected)
			{
				//Устанавливаем бледный цвет
				const float alpha	= 0.3f;
				c	= c*alpha + vec3(1.0f)*(1.0f-alpha);
			}
			return QColor(c.r*255, c.g*255, c.b*255);
		}break;

	case Qt::TextAlignmentRole:
		{
			if(index.column() == 0)	return (Qt::AlignLeft + Qt::AlignVCenter);
            else					return (Qt::AlignLeft + Qt::AlignVCenter);
		}break;
	}

	return QVariant();
}

QVariant LocatorModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */) const
{
	switch(role)
	{
	case Qt::DisplayRole:
	{
		switch(section)
		{
		case 0: return m_rootItem->GetData()->name;
		case 1: return m_rootItem->GetData()->value;
		default:	return QVariant();
		}
	}break;

	default:
		break;
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags LocatorModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())	return 0;
	return Qt::ItemIsEnabled; 
}

LocatorItem* LocatorModel::getItem(const QModelIndex &index) const
{
    if(index.isValid()) 
	{
        LocatorItem*	item	= static_cast<LocatorItem*>(index.internalPointer());
        if(item) return item;
    }
    return m_rootItem;
}

QModelIndex LocatorModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
		return QModelIndex();

	LocatorItem*	parentItem	= getItem(parent);
	if(!parentItem->childCount())	return QModelIndex();
	LocatorItem*	childItem	= parentItem->child(row);
    
	if(childItem)	return createIndex(row, column, childItem);
    else			return QModelIndex();
}

QModelIndex	LocatorModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())	return QModelIndex();

    LocatorItem*	childItem	= getItem(index);
    LocatorItem*	parentItem	= childItem->parent();

    if(parentItem == m_rootItem)	return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int LocatorModel::rowCount(const QModelIndex &parent) const
{
    LocatorItem*	parentItem	= getItem(parent);

    return parentItem->childCount();
}

void	LocatorModel::updateModel(double time)
{
	if(m_rootItem)
	{
		if(bUseTimeLeft)	
		{
			m_rootItem->setHeaderData(QString("dt = %1").arg(time - timeLeft), QString("t = %1").arg(time));
			emit headerDataChanged(Qt::Horizontal, 0, 1);
		}
		else
		{
			m_rootItem->setHeaderData("Параметр", QString("t = %1").arg(time));
			emit headerDataChanged(Qt::Horizontal, 0, 1);
		}

		m_rootItem->update(time);
		emit dataChanged(i_Start, i_Stop);
	}
}

void	LocatorModel::resortAxes()
{
	if(m_rootItem)
	{
		m_rootItem->sortAxes();
		emit dataChanged(i_Start, i_Stop);
	}
}

void	LocatorModel::setSelected(bool bSelected)
{
	m_bHasSelected	= bSelected;
	emit dataChanged(i_Start, i_Stop);
}

void	LocatorModel::updateNames()
{
	if(m_rootItem)
	{
		m_rootItem->updateNames();
		emit dataChanged(index(0,0), i_Stop);
	}
}

void	LocatorModel::setTimeLeft(double time, bool use)
{
	timeLeft		= time;
	bUseTimeLeft	= use;
}
