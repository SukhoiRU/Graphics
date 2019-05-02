#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <vector>
using std::vector;

class TreeItem
{
public:
	enum ItemType{
		Item_System, 
		Item_Context, 
		Item_Struct, 
		Item_Bool, 
		Item_Int, 
		Item_Double, 
		Item_Vector2D,
		Item_Vector3D,
		Item_Matrix2D,
		Item_Matrix3D,
		Item_Enum,
		Item_Bank,
		Item_Chapter
	};

	struct Data
	{
        ItemType	type;       //Тип
		QString		name;		//Имя элемента
		QString		comm;		//Комментарий во второй колонке
		QString		typeName() const;
	};

	TreeItem(const Data& data, TreeItem *parent = 0);
	~TreeItem();

	TreeItem*	parent()	{return m_pParent;}
	TreeItem* 	addChildren(const Data& data);
	void		addChildsFrom(TreeItem* item);
	void 		delLastChildren();
	TreeItem*	child(int number);
	int			childCount() const;
	int			childNumber() const {return m_childNumber;}
	int 		columnCount() const	{return 2;}
	QVariant	data(int column) const;
	TreeItem::Data*		GetData(){return &m_Data;}
    TreeItem*	findChild(QString name);

private:
	TreeItem*			m_pParent;
	vector<TreeItem*>	m_ChildItems;
	int					m_childNumber;
	Data				m_Data;
};

#endif
