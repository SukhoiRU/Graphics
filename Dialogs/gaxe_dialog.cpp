#include "stdafx.h"
#include "gaxe_dialog.h"
#include "ui_gaxe_dialog.h"
#include "../Graph/GAxe.h"
#include "graphselect.h"
using namespace Graph;

GAxe_dialog::GAxe_dialog(vector<GAxe*>* pAxes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GAxe_dialog)
{
    ui->setupUi(this);
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GAxe_dialog");
	if(settings.contains("size"))
		resize(settings.value("size").toSize());

    QList<QAbstractButton*> butList = ui->buttonBox->buttons();
    for(QAbstractButton* i : butList)
    {
        i->setIcon(QIcon());
    }

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &GAxe_dialog::on_accept);
	connect(ui->pushButton_Replace, &QPushButton::clicked, this, &GAxe_dialog::on_replace);
	connect(ui->pushButton_Substract, &QPushButton::clicked, this, &GAxe_dialog::on_substract);
	connect(ui->pushButton_Color, &ColorButton::colorChanged, this, &GAxe_dialog::on_colorChanged);
	connect(ui->pushButton_Stat, &QPushButton::clicked, this, &GAxe_dialog::on_statistic);
	axes		= *pAxes;

	//Заполняем поля
	{
		double	Min, Max;
		axes.front()->getLimits(&Min, &Max);
		for(size_t i = 0; i < axes.size(); i++)
		{
			GAxe*	pAxe	= axes.at(i);
			double	axeMin, axeMax;
			pAxe->getLimits(&axeMin, &axeMax);
			if(axeMin < Min)	Min	= axeMin;
			if(axeMax < Max)	Max	= axeMax;
		}
		ui->label_Min->setText(QString("[%1]").arg(Min));
		ui->label_Max->setText(QString("[%1]").arg(Max));
	}

	//Имя
	QString	name	= axes.front()->m_Name;
	ui->lineEdit_Name->setText(name);
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_Name != name)
		{
			ui->lineEdit_Name->setText("<diff options>");
			break;
		}
	}

	//Начало
	double	Min	= axes.front()->m_AxeMin;
	ui->lineEdit_Min->setText(QString("%1").arg(Min));
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_AxeMin != Min)
		{
			ui->lineEdit_Min->setText("<diff options>");
			break;
		}
	}
	
	//Масштаб
	double	Scale	= axes.front()->m_AxeScale;
	ui->lineEdit_Scale->setText(QString("%1").arg(Scale));
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_AxeScale != Scale)
		{
			ui->lineEdit_Scale->setText("<diff options>");
			break;
		}
	}

	//Длина
	int	Len	= axes.front()->getAxeLength();
	ui->lineEdit_Length->setText(QString("%1").arg(Len));
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->getAxeLength() != Len)
		{
			ui->lineEdit_Length->setText("<diff options>");
			break;
		}
	}

	//Цвет
	vec3	color	= axes.front()->m_Color;
	QColor  c(color.r*255, color.g*255, color.b*255);
	ui->pushButton_Color->setColor(c);
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_Color != color)
		{
			ui->pushButton_Color->setDiffColor();
			break;
		}
	}

	//Маркер
	int	markerType	= axes.front()->m_nMarker/3;
	int	markerStyle	= axes.front()->m_nMarker%3;
	int	marker		= axes.front()->m_nMarker;
	ui->comboType->setCurrentIndex(markerType);
	ui->comboStyle->setCurrentIndex(markerStyle);
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_nMarker != marker)
		{
			ui->comboType->setCurrentIndex(-1);
			ui->comboStyle->setCurrentIndex(-1);
			break;
		}
	}

	//Интерполяция
	bool	Interp	= axes.front()->m_bInterpol;
	ui->checkBox_Interpol->setChecked(Interp);
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_bInterpol != Interp)
		{
			ui->checkBox_Interpol->setCheckState(Qt::PartiallyChecked);
			break;
		}
	}

	//Отключаем кнопку замены для списков
	if(axes.size() > 1)
	{
		ui->pushButton_Replace->setDisabled(true);
		ui->pushButton_Stat->setDisabled(true);
	}

	//Выделяем минимум для удобства Tab
	ui->lineEdit_Min->setFocus();
}

GAxe_dialog::~GAxe_dialog()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GAxe_dialog");
	settings.setValue("size", size());
	settings.sync();

    delete ui;
}

void GAxe_dialog::on_accept(QAbstractButton* pButton)
{
	if(ui->buttonBox->buttonRole(pButton) == QDialogButtonBox::AcceptRole ||
		ui->buttonBox->buttonRole(pButton) == QDialogButtonBox::ApplyRole)
	{
		//Разгребаем поля

		//Имя
		QString	name	= ui->lineEdit_Name->text();
		if(name != "<diff options>")
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_Name = name;

		//Начало
		double	Min	= ui->lineEdit_Min->text().toDouble();
		if(ui->lineEdit_Min->text() != "<diff options>")
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_AxeMin = Min;

		//Масштаб
		double	Scale	= ui->lineEdit_Scale->text().toDouble();
		if(ui->lineEdit_Scale->text() != "<diff options>")
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_AxeScale = Scale;

		//Длина
		int	Len	= ui->lineEdit_Length->text().toInt();
		if(ui->lineEdit_Length->text() != "<diff options>")
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->setAxeLength(Len);

		//Цвет
		QColor	color;
		if(ui->pushButton_Color->getColor(color))
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_Color = vec3(color.red()/255., color.green()/255., color.blue()/255.);

		//Маркер
		if(ui->comboType->currentIndex() != -1 && ui->comboStyle->currentIndex() != -1)
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_nMarker = ui->comboType->currentIndex()*3 + ui->comboStyle->currentIndex();

		//Интерполяция
		if(ui->checkBox_Interpol->checkState() != Qt::PartiallyChecked)
			for(size_t i = 0; i < axes.size(); i++)
				axes.at(i)->m_bInterpol = ui->checkBox_Interpol->checkState() == Qt::Checked;

		emit accepted();
	}
}

void	GAxe_dialog::on_replace()
{
	if(axes.empty())	return;
	if(axes.size() > 1)	return;

	GAxe*	pAxe	= axes.front();
	emit	change_axe(pAxe);

	//Меняем диалог
	ui->lineEdit_Name->setText(pAxe->m_Name);
	double	Min, Max;
	pAxe->getLimits(&Min, &Max);
	ui->label_Min->setText(QString("[%1]").arg(Min));
	ui->label_Max->setText(QString("[%1]").arg(Max));
	ui->lineEdit_Min->setText(QString("%1").arg(pAxe->m_AxeMin));
	ui->lineEdit_Scale->setText(QString("%1").arg(pAxe->m_AxeScale));
}

void	GAxe_dialog::on_substract()
{
	if(axes.empty())	return;
	if(axes.size() > 1)	return;

	GAxe*	pAxe	= axes.front();
	emit	substract_axe(pAxe);

	//Меняем диалог
	ui->lineEdit_Name->setText(pAxe->m_Name);
	double	Min, Max;
	pAxe->getLimits(&Min, &Max);
	ui->label_Min->setText(QString("[%1]").arg(Min));
	ui->label_Max->setText(QString("[%1]").arg(Max));
	ui->lineEdit_Min->setText(QString("%1").arg(pAxe->m_AxeMin));
	ui->lineEdit_Scale->setText(QString("%1").arg(pAxe->m_AxeScale));
}

void	GAxe_dialog::on_colorChanged()
{
	//Сразу применяем цвет
	QColor	color;
	if(ui->pushButton_Color->getColor(color))
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->m_Color = vec3(color.red()/255., color.green()/255., color.blue()/255.);
}

void	GAxe_dialog::on_statistic()
{
	//Делаем только для одной оси
	if (axes.empty())		return;
	if (axes.size() > 1)	return;

	const GAxe*	pAxe = axes.front();
	emit	getStatistic(pAxe);
}
