#include "stdafx.h"
#include "locator_item.h"
#include "Graph/GAxe.h"
#include <algorithm>
#include <QTextCodec>
#include <QBitmap>

LocatorItem::LocatorItem(const Data& data, LocatorItem* parent)
{
	m_pParent		= parent;
	m_Data			= data;
	m_childNumber	= 0;
}

LocatorItem::~LocatorItem()
{
	qDeleteAll(m_ChildItems);
}

LocatorItem*	LocatorItem::child(int number)	{return m_ChildItems.at(number);}
int				LocatorItem::childCount() const	{return m_ChildItems.size();}

LocatorItem* LocatorItem::addChildren(const Data& data)
{
	LocatorItem*	item = new LocatorItem(data, this);
	item->m_childNumber	= m_ChildItems.size();
	m_ChildItems.push_back(item);

	return item;
}

void	LocatorItem::addChildsFrom(LocatorItem* item)
{
	//Копирование чужих деток
	LocatorItem* subItem	= addChildren(item->m_Data);
	for(size_t i = 0; i < item->m_ChildItems.size(); i++)
	{
		LocatorItem*	child	= item->m_ChildItems[i];
		subItem->addChildsFrom(child);		
	}
}

void	LocatorItem::delLastChildren()
{
	m_ChildItems.pop_back();
}

QVariant LocatorItem::data(int column) const
{
	switch(column)
	{
		//В первую колонку всегда возвращаем имя
	case 0:		return m_Data.name;	break;

		//Во вторую - комментарий
	case 1:		return m_Data.value; break;

		//Для неправильных колонок возвращаем пустоту
	default:
		return QVariant();
	}
}

LocatorItem*	LocatorItem::findChild(QString name)
{
	//Ищем элемент с таким же именем
    for(size_t i = 0; i < m_ChildItems.size(); i++)
	{
		LocatorItem*	pItem	= m_ChildItems.at(i);
		if(pItem->m_Data.name == name)
			return pItem;
	}

	//Не нашли
	return nullptr;
}

void	LocatorItem::update(double time)
{
	//Обновляем себя
	if(m_Data.pAxe)
	{
		double	val	= m_Data.pAxe->GetValueAtTime(time);
		QString	strVal	= QString("%1").arg(val);
		if(m_Data.pAxe->m_Record == -1)	strVal	= "";
		if(strVal != m_Data.value)
		{
			m_Data.value	= strVal;
		}
	}

	//Обновляем детей
	for(size_t i = 0; i < m_ChildItems.size(); i++)
	{
		m_ChildItems.at(i)->update(time);
	}
}

void	LocatorItem::clear()
{
	m_ChildItems.clear();
}

void	LocatorItem::sortAxes()
{
	//Сортировка списка осей по высоте верхней точки
	std::stable_sort(m_ChildItems.begin(), m_ChildItems.end(), [](const LocatorItem* a, const LocatorItem* b) 
	{
		const Graph::GAxe*	pAxe1	= a->GetData()->pAxe;
		const Graph::GAxe*	pAxe2	= b->GetData()->pAxe;
		return (pAxe1->GetPosition().y + pAxe1->getAxeLength()) > (pAxe2->GetPosition().y + pAxe2->getAxeLength());
	});
}