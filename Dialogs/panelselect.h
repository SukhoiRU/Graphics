#ifndef PANELSELECT_H
#define PANELSELECT_H

#include <QWidget>

namespace Ui {
class PanelSelect;
}

class PanelSelect : public QWidget
{
    Q_OBJECT

public:
    explicit PanelSelect(QWidget *parent = 0);
    ~PanelSelect();

public:
    Ui::PanelSelect *ui;
};

#endif // PANELSELECT_H
