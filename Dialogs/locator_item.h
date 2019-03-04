#ifndef LOCATORITEM_H
#define LOCATORITEM_H

#include <QVariant>
#include <vector>
using std::vector;

namespace Graph{
	class GAxe;
}
using namespace Graph;

class LocatorItem
{
public:
	struct Data
	{
		QString		name;		//Имя сигнала
		QString		value;		//Значение сигнала
		const GAxe*	pAxe;		//Указатель на ось
	};

	LocatorItem(const Data& data, LocatorItem *parent = 0);
	~LocatorItem();

	LocatorItem*		parent()	{return m_pParent;}
	LocatorItem* 		addChildren(const Data& data);
	void				addChildsFrom(LocatorItem* item);
	void 				delLastChildren();
	LocatorItem*		child(int number);
	int					childCount() const;
	int					childNumber() const {return m_childNumber;}
	int 				columnCount() const	{return 2;}
	QVariant			data(int column) const;
	const LocatorItem::Data*	GetData() const {return &m_Data;}
    LocatorItem*		findChild(QString name);
	void				update(double time);
	void				clear();
	void				sortAxes();
	void				updateNames();
	void				setHeaderData(QString first, QString second);

private:
	LocatorItem*			m_pParent;
	vector<LocatorItem*>	m_ChildItems;
	int						m_childNumber;
	Data					m_Data;
};

#endif // LOCATORITEM_H
