#ifndef GAXE_DIALOG_H
#define GAXE_DIALOG_H

#include <QDialog>

namespace Ui {
class GAxe_dialog;
}

class GAxe_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit GAxe_dialog(QWidget *parent = 0);
    ~GAxe_dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::GAxe_dialog *ui;
};

#endif // GAXE_DIALOG_H
