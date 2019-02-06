#ifndef QLOCATOR_H
#define QLOCATOR_H

#include <QTreeView>
#include <vector>
using std::vector;
namespace Graph{
	class GAxe;
}
class Accumulation;
class LocatorModel;

class LocatorView : public QTreeView
{
    Q_OBJECT
private:
	bool	m_bHeader;
	bool	m_bAutoSize;
	bool	m_bGrid;
	bool	m_bAnimated;
	bool	m_bAlternate;

	LocatorModel*	m_model;

public:
    explicit LocatorView(QWidget *parent = nullptr);
	virtual ~LocatorView();

private:
    virtual void	drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    virtual void	paintEvent(QPaintEvent* evnt);
	void	onCustomMenuRequested(QPoint pos);

public slots:
	void	on_panelChanged(vector<Graph::GAxe*>* axes, vector<Accumulation*>* pBuffer);
	void	on_timeChanged(double time);
	void	on_axeSelected(bool bSelect);
};

#endif // QLOCATOR_H
