#include "stdafx.h"
#include "graphSettings.h"
#include "Graph/GAxe.h"
using namespace Graph;

graphSettings::graphSettings(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.buttonBox, &QDialogButtonBox::clicked, this, &graphSettings::on_accept);
	ui.buttonBox->button(QDialogButtonBox::Apply)->setDefault(true);

	ui.lineEdit_width->setText(QString("%1").arg(GAxe::m_width));
	ui.lineEdit_alias->setText(QString("%1").arg(GAxe::m_alias));
	ui.lineEdit_sel_width->setText(QString("%1").arg(GAxe::m_selWidth));
	ui.lineEdit_sel_alias->setText(QString("%1").arg(GAxe::m_selAlias));
	ui.lineEdit_interp_width->setText(QString("%1").arg(GAxe::m_interpWidth));
	ui.lineEdit_interp_alias->setText(QString("%1").arg(GAxe::m_interpAlias));
}

graphSettings::~graphSettings()
{
}

void	graphSettings::on_accept(QAbstractButton* pButton)
{
	if(ui.buttonBox->buttonRole(pButton) == QDialogButtonBox::AcceptRole ||
	   ui.buttonBox->buttonRole(pButton) == QDialogButtonBox::ApplyRole)
	{
		//Разгребаем поля
		GAxe::m_width		= ui.lineEdit_width->text().toFloat();
		GAxe::m_alias		= ui.lineEdit_alias->text().toFloat();
		GAxe::m_selWidth	= ui.lineEdit_sel_width->text().toFloat();
		GAxe::m_selAlias	= ui.lineEdit_sel_alias->text().toFloat();
		GAxe::m_interpWidth	= ui.lineEdit_interp_width->text().toFloat();
		GAxe::m_interpAlias	= ui.lineEdit_interp_alias->text().toFloat();

		if(ui.buttonBox->buttonRole(pButton) == QDialogButtonBox::AcceptRole)
			accept();
	}
}