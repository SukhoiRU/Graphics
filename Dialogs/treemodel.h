#ifndef TREEMODEL_H
#define TREEMODEL_H
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QPixmap>
#include <QFont>
#include <QIcon>

class TreeItem;
class Accumulation;

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	TreeModel(QObject *parent = 0);
	~TreeModel();

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &, const QVariant &, int) {return false;}

    void	loadAcc(const std::vector<Accumulation *>* pBuffer);
	void	UpdateModel();

protected:
	TreeItem*	m_rootItem;
	TreeItem*	getItem(const QModelIndex &index) const;
	
	//Набор иконок
    QIcon		m_IconSystem;
    QIcon		m_IconContext;
    QIcon		m_IconStruct;
    QIcon		m_IconBool;
    QIcon		m_IconInt;
    QIcon		m_IconDouble;
	QIcon		m_IconFloat;
    QIcon		m_IconVector2D;
    QIcon		m_IconVector3D;
    QIcon		m_IconMatrix2D;
    QIcon		m_IconMatrix3D;
    QIcon		m_IconEnum;
    QIcon		m_IconBank;
    QIcon		m_IconChapter;
};

#endif	//TREEMODEL_H
