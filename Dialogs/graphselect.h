#ifndef GRAPHSELECT_H
#define GRAPHSELECT_H

#include <QDialog>
#include <vector>
using std::vector;
class Accumulation;

namespace Ui {
class GraphSelect;
}

class GraphSelect : public QDialog
{
    Q_OBJECT

public:
    explicit GraphSelect(QWidget *parent = nullptr);
    ~GraphSelect();
	void	SetAccumulation(const vector<Accumulation*>*	pBuffer);

    int m_nBufIndex;
    int m_nAccIndex;
private:
    Ui::GraphSelect *ui;

    void    onSignalAccepted(int nBufIndex, int nAccIndex);
};

#endif // GRAPHSELECT_H
