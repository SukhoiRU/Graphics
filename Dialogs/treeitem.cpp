#include "stdafx.h"
#include "treeitem.h"
#include <QTextCodec>
#include <QBitmap>

TreeItem::TreeItem(const Data& data, TreeItem *parent)
{
	m_pParent	= parent;
	m_Data		= data;
	m_childNumber	= 0;
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_ChildItems);
}

TreeItem*	TreeItem::child(int number)		{return m_ChildItems.at(number);}
int			TreeItem::childCount() const	{return m_ChildItems.size();}

TreeItem* TreeItem::addChildren(const Data& data)
{
	TreeItem*	item = new TreeItem(data, this);
	item->m_childNumber	= m_ChildItems.size();
	m_ChildItems.push_back(item);

	return item;
}

void	TreeItem::addChildsFrom(TreeItem* item)
{
	//Копирование чужих деток
	TreeItem* subItem	= addChildren(item->m_Data);
	for(size_t i = 0; i < item->m_ChildItems.size(); i++)
	{
		TreeItem*	child	= item->m_ChildItems[i];
		subItem->addChildsFrom(child);		
	}
}

void	TreeItem::delLastChildren()
{
	m_ChildItems.pop_back();
}

QVariant TreeItem::data(int column) const
{
	switch(column)
	{
		//В первую колонку всегда возвращаем имя
	case 0:		return m_Data.name;	break;

		//Во вторую - комментарий
	case 1:		return m_Data.comm; break;

		//Для неправильных колонок возвращаем пустоту
	default:
		return QVariant();
	}
}

QString	TreeItem::Data::typeName() const
{
	switch(type)
	{
	case Item_System:	return "System";
	case Item_Context:	return "Context";
	case Item_Struct:	return "Struct";
	case Item_Vector2D: return "Vector2D";
	case Item_Vector3D: return "Vector3D";
	case Item_Matrix2D: return "Matrix2D";
	case Item_Matrix3D: return "Matrix3D";
	case Item_Bank:		return "Bank";
	default:			return "Item";
	}
}

TreeItem*	TreeItem::findChild(QString name)
{
	//Ищем элемент с таким же именем
    for(size_t i = 0; i < m_ChildItems.size(); i++)
	{
		TreeItem*	pItem	= m_ChildItems.at(i);
		if(pItem->m_Data.name == name)
			return pItem;
	}

	//Не нашли
	return nullptr;
}
