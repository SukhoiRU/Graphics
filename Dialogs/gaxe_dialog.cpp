#include "stdafx.h"
#include "gaxe_dialog.h"
#include "ui_gaxe_dialog.h"
#include "../Graph/GAxe.h"
using namespace Graph;

GAxe_dialog::GAxe_dialog(vector<Graph::GAxe*>* pAxes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GAxe_dialog)
{
    ui->setupUi(this);
	connect(this, &QDialog::accepted, this, &GAxe_dialog::on_accept);

	axes	= *pAxes;

	//��������� ����
	{
		double	Min, Max;
		axes.front()->GetLimits(&Min, &Max);
		for(size_t i = 0; i < axes.size(); i++)
		{
			GAxe*	pAxe	= axes.at(i);
			double	axeMin, axeMax;
			pAxe->GetLimits(&axeMin, &axeMax);
			if(axeMin < Min)	Min	= axeMin;
			if(axeMax < Max)	Max	= axeMax;
		}
		ui->label_Min->setText(QString("[%1]").arg(Min));
		ui->label_Max->setText(QString("[%1]").arg(Max));
	}

	//���
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

	//������
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
	
	//�������
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

	//�����
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

	//����
	vec3	color	= axes.front()->m_Color;
	ui->pushButton_Color->setColor(QColor(color.r*255, color.g*255, color.b*255));
	for(size_t i = 0; i < axes.size(); i++)
	{
		GAxe*	pAxe	= axes.at(i);
		if(pAxe->m_Color != color)
		{
			ui->pushButton_Color->setDiffColor();
			break;
		}
	}

	//��������� ������ ������ ��� �������
	if(axes.size() > 1)
		ui->pushButton_Replace->setDisabled(true);

	//�������� ������� ��� �������� Tab
	ui->lineEdit_Min->setFocus();
}

GAxe_dialog::~GAxe_dialog()
{
    delete ui;
}

void GAxe_dialog::on_accept()
{
	//���������� ����

	//���
	QString	name	= ui->lineEdit_Name->text();
	if(name != "<diff options>")
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->m_Name = name;

	//������
	double	Min	= ui->lineEdit_Min->text().toDouble();
	if(ui->lineEdit_Min->text() != "<diff options>")
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->m_AxeMin = Min;

	//�������
	double	Scale	= ui->lineEdit_Scale->text().toDouble();
	if(ui->lineEdit_Scale->text() != "<diff options>")
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->m_AxeScale = Scale;

	//�����
	int	Len	= ui->lineEdit_Length->text().toInt();
	if(ui->lineEdit_Length->text() != "<diff options>")
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->setAxeLength(Len);

	//����
	QColor	color;
	if(ui->pushButton_Color->getColor(color))
		for(size_t i = 0; i < axes.size(); i++)
			axes.at(i)->m_Color = vec3(color.red()/255., color.green()/255., color.blue()/255.);
}

