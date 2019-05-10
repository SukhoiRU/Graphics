#ifndef GAXE_DIALOG_H
#define GAXE_DIALOG_H

#include <QDialog>
#include <vector>
using std::vector;

namespace Ui {
class GAxe_dialog;
}

namespace Graph{
	class GAxe;
}
class QAbstractButton;

class GAxe_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit GAxe_dialog(vector<Graph::GAxe*>* pAxes, QWidget *parent = 0);
    ~GAxe_dialog();

private slots:
    void	on_accept(QAbstractButton* pButton);
	void	on_replace();
	void	on_substract();

signals:
	void	change_axe(Graph::GAxe* pAxe);
	void	substract_axe(Graph::GAxe* pAxe);

private:
    Ui::GAxe_dialog *ui;
	vector<Graph::GAxe*>	axes;
};

#endif // GAXE_DIALOG_H
