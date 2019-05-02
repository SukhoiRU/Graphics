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
	void	SetPath(QString path);

    QString m_Path;
private:
    Ui::GraphSelect *ui;
    void    onSignalAccepted(QString path);
};

#endif // GRAPHSELECT_H
