#ifndef QLOCATOR_H
#define QLOCATOR_H

#include <QTreeView>
#include <vector>
using std::vector;
namespace Graph{
	class GAxe;
}
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
	bool	m_bUseBool;
	bool	m_bUseEmpty;

	LocatorModel*	m_model;
	vector<Graph::GAxe*>* m_axes;
	double	oldTime;

public:
    explicit LocatorView(QWidget *parent = nullptr);
	virtual ~LocatorView();

private:
    virtual void	drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    virtual void	paintEvent(QPaintEvent* evnt);
	void	onCustomMenuRequested(QPoint pos);

public slots:
	void	on_panelChanged(vector<Graph::GAxe*>* axes);
	void	on_timeChanged(double time);
	void	on_axeSelected(bool bSelect);
	void	on_axesMoved();
	void	on_axesRenamed();
};

#endif // QLOCATOR_H
