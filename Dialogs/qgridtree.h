#ifndef QMYTREE_H
#define QMYTREE_H

#include <QTreeView>

class QGridTree : public QTreeView
{
    Q_OBJECT
private:
	mutable int	m_nRowHeight;
public:
    explicit QGridTree(QWidget *parent = 0);

private:
    virtual void	drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    virtual void	paintEvent(QPaintEvent* evnt);
    virtual void	keyPressEvent(QKeyEvent* evnt);
    virtual void    mouseDoubleClickEvent(QMouseEvent *event);
    void    onAccept();

signals:
    void    onSignalAccepted(int nBufIndex, int nAccIndex);
};

#endif // QMYTREE_H
