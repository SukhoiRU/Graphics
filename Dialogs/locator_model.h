#ifndef LOCATORMODEL_H
#define LOCATORMODEL_H
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QPixmap>
#include <QFont>
#include <QIcon>

class LocatorItem;
namespace Graph{
	class GAxe;
}

class LocatorModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	LocatorModel(QObject *parent = 0);
	~LocatorModel();

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &, const QVariant &, int) {return false;}

    void	setPanel(const std::vector<Graph::GAxe*>* axes);
	void	UpdateModel(double time);
	void	setSelected(bool bSelected);

protected:
	LocatorItem*	m_rootItem;
	LocatorItem*	getItem(const QModelIndex &index) const;	

	QModelIndex		i_Start;
	QModelIndex		i_Stop;

	bool	m_bHasSelected;
};

#endif	//LOCATORMODEL_H
