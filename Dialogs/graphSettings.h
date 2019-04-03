#pragma once

#include <QDialog>
#include "ui_graphSettings.h"

class graphSettings : public QDialog
{
	Q_OBJECT

public:
	graphSettings(QWidget *parent = Q_NULLPTR);
	~graphSettings();

private slots:
	void	on_accept(QAbstractButton* pButton);

private:
	Ui::graphSettings ui;
};
