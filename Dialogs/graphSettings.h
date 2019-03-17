#pragma once

#include <QWidget>
#include "ui_graphSettings.h"

class graphSettings : public QDialog
{
	Q_OBJECT

public:
	graphSettings(QWidget *parent = Q_NULLPTR);
	~graphSettings();

private:
	Ui::graphSettings ui;
};
