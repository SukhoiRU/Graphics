#ifndef QMYTREE_H
#define QMYTREE_H

#include <QTreeView>

class QGridTree : public QTreeView
{
    Q_OBJECT
private:
	bool	m_bHeader;
	bool	m_bAutoSize;
	bool	m_bGrid;
	bool	m_bAnimated;

public:
    explicit QGridTree(QWidget *parent = 0);
	virtual ~QGridTree();

private:
    virtual void	drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    virtual void	paintEvent(QPaintEvent* evnt);
    virtual void	keyPressEvent(QKeyEvent* evnt);
    virtual void    mouseDoubleClickEvent(QMouseEvent *event);
    void    onAccept();
	void	onCustomMenuRequested(QPoint pos);

signals:
    void    onSignalAccepted(int nBufIndex, int nAccIndex);
};

#endif // QMYTREE_H
