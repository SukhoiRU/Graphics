#include "stdafx.h"
#include "graphicsdoc.h"
#include "graphicsview.h"
#include <QDomDocument>
#include "ui_graphicsdoc.h"
#include "ui_panelselect.h"
#include "Accumulation.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "Dialogs/panelselect.h"
#include "Dialogs/graphselect.h"

GraphicsDoc::GraphicsDoc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicsDoc)
{
    ui->setupUi(this);
	ui->oglView->vBar	= ui->verticalScrollBar;
	ui->oglView->hBar	= ui->horizontalScrollBar;
	connect(ui->actionPageInfo, &QAction::triggered, ui->oglView, &GraphicsView::openPageSetup);
	connect(this, &GraphicsDoc::panelChanged, ui->oglView, &GraphicsView::on_panelChanged);

    m_pPanelSelect  = new PanelSelect(ui->toolBarPanel);
    ui->toolBarPanel->addWidget(m_pPanelSelect);
	connect(m_pPanelSelect->ui->pushButtonAdd, &QPushButton::clicked, this, &GraphicsDoc::on_PanelAdd);
	connect(m_pPanelSelect->ui->pushButtonDelete, &QPushButton::clicked, this, &GraphicsDoc::on_PanelDelete);
	connect(m_pPanelSelect->ui->pushButtonCopy, &QPushButton::clicked, this, &GraphicsDoc::on_PanelCopy);
	connect(m_pPanelSelect->ui->comboBox, &QComboBox::editTextChanged, this, &GraphicsDoc::on_PanelRenamed);
	connect(m_pPanelSelect->ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GraphicsDoc::on_PanelIndexChanged);

    m_bAddAcc_Mode  = false;

	//Создаем новую пустую панель
	Panel*	p	= new Panel;
	p->Name	= "Новая";
	m_PanelList.clear();
	m_PanelList.push_back(p);
	on_PanelListChanged();
	m_pActivePanel	= p;
}

GraphicsDoc::~GraphicsDoc()
{
    delete ui;

	//Очищаем список панелей
    for(size_t i = 0; i < m_PanelList.size(); i++)
	{
		Panel*	p	= m_PanelList.at(i);
		delete p;
	}
	m_PanelList.clear();
}

void GraphicsDoc::on_actionOpen_triggered()
{
	//Открываем файл экрана
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение файла экрана", "", "*.grf");
    if(FileName.isEmpty())  return;

    // TODO: Add your dispatch handler code here
    QFile	file(FileName);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка чтения файла экрана!", QString("Cannot read file %1: %2.").arg(FileName).arg(file.errorString()));
        return;
    }

	QDomDocument xml;

	QString errorStr;
	int errorLine;
	int errorColumn;

	//Закидываем файл в QDomDocument
	if(!xml.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(this, "Чтение файла экрана", QString("%1\nParse error at line %2, column %3:\n%4").arg(FileName).arg(errorLine).arg(errorColumn).arg(errorStr));
		return;
	}

	//Проверяем тип файла
	QDomElement root = xml.documentElement();
	if(root.tagName() != "Файл_экрана")
	{
		QMessageBox::critical(this, "Чтение файла экрана", "Файл не содержит описание экрана!");
		return;
	}

	if(root.hasAttribute("version") && root.attribute("version") != "1.0")
	{
		QMessageBox::critical(this, "Чтение файла экрана", "Версия экрана должна быть не меньше 1.0!");
		return;
	}

	//Очищаем список панелей
    for(size_t i = 0; i < m_PanelList.size(); i++)
	{
		Panel*	p	= m_PanelList.at(i);
		delete p;
	}
	m_PanelList.clear();

	//Читаем список панелей
	QDomElement	e	= root.firstChildElement("Список_панелей");
	if(e.isNull())
	{
		QMessageBox::critical(this, "Чтение файла экрана", "Файл не содержит списка панелей!");
		return;
	}

	for(QDomElement p = e.firstChildElement("Панель"); !p.isNull(); p = p.nextSiblingElement("Панель"))
	{
		//Создаем новую панель
		Panel*	pPanel	= new Panel;
		m_PanelList.push_back(pPanel);

		if(p.hasAttribute("Название"))	pPanel->Name	= p.attribute("Название", "Новая");
		
		//Читаем оси
		for(QDomElement g = p.firstChildElement("График"); !g.isNull(); g = g.nextSiblingElement("График"))
		{
			//GAxe*	pAxe	= new GAxe;
			//if(g.hasAttribute("Название"))	pAxe->Name	= g.attribute("Название");
			//pPanel->Axes.push_back(pAxe);
		}
	}

	on_PanelListChanged();

	//Устанавливаем активную панель
	QDomElement act = e.firstChildElement("Активная_панель");
	if(!e.isNull())
	{
		int	cur	= act.text().toInt();
		m_pPanelSelect->ui->comboBox->setCurrentIndex(cur);
	}
}

void GraphicsDoc::on_action_LoadOrion_triggered()
{
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение накопления Орион", "", "*.orion");
    if(FileName.isEmpty())  return;

    // TODO: Add your dispatch handler code here
    QFile	in(FileName);
    if(!in.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка чтения файла накопления Орион!", QString("Cannot read file %1: %2.").arg(FileName).arg(in.errorString()));
        return;
    }
    in.close();

	LoadOrion(FileName);
	ui->statusBar->showMessage(QString("Загружен файл %1").arg(FileName), 5000);
}

void	GraphicsDoc::LoadOrion(QString FileName)
{
    //Обновим данные осей
    for(size_t j = 0; j < m_PanelList.size(); j++)
    {
        Panel*	p	= m_PanelList.at(j);
        for(size_t pos = 0; pos < p->Axes.size(); pos++)
        {
            GAxe*	Axe	= p->Axes[pos];
            //Axe->ClearFiltering();
        }
    }

    Accumulation*	pAcc;
    if(m_bAddAcc_Mode)
    {
        //Добавляем новое накопление
        pAcc	= new Accumulation;
        pAcc->m_Name    = QString("Данные №%1").arg(m_BufArray.size()+1);
        pAcc->LoadOrion(FileName);
        m_BufArray.push_back(pAcc);
    }
    else
    {
        //Меняем данные в последнем
        if(m_BufArray.size())
        {
            pAcc	= m_BufArray.back();
			emit dataRemoved();
        }
        else
        {
            //Буфер пустой, поэтому вставим хоть одно
            pAcc	= new Accumulation;
            pAcc->m_Name    = QString("Данные №%1").arg(m_BufArray.size()+1);
            m_BufArray.push_back(pAcc);
        }

        //Загружаем данные
        pAcc->LoadOrion(FileName);
    }
	emit dataChanged();
/*
    m_DataFileName	= FileName;
    if(m_bUseFileAsText)
    {
        m_pText->m_Text	= FileName;
        int n	= m_pText->m_Text.ReverseFind('.');
        if(n != -1)	m_pText->m_Text	= m_pText->m_Text.Left(n);
        n	= m_pText->m_Text.ReverseFind('\\');
        if(n != -1)	m_pText->m_Text.Delete(0, n+1);
    }

    if(m_bFitArgument)
    {
        //Обновим масштаб времени
        m_pArg->FitToScale();
    }
*/
    //Обновим данные осей
    for(size_t pos = 0; pos < m_pActivePanel->Axes.size(); pos++)
    {
        GAxe*	Axe	= m_pActivePanel->Axes[pos];
        //Axe->ClearFiltering();
        //Axe->UpdateRecord();
    }
}

void	GraphicsDoc::on_PanelListChanged()
{
	//Записываем новый список
	QComboBox*	pBox	= m_pPanelSelect->ui->comboBox;
	pBox->clear();

    for(size_t i = 0; i < m_PanelList.size(); i++)
	{
		Panel*	p = m_PanelList.at(i);
        pBox->addItem(p->Name, (int)i);
	}
}

void	GraphicsDoc::on_PanelIndexChanged(int index)
{
	if(index == -1)	return;
    if(index > (int)(m_PanelList.size()-1))	return;
	m_pActivePanel	= m_PanelList.at(index);
    emit panelChanged(&m_pActivePanel->Axes);
}

void	GraphicsDoc::on_PanelAdd()
{
	//Добавляем панель после текущей
	QComboBox*	pBox	= m_pPanelSelect->ui->comboBox;
	int	cur	= pBox->currentIndex();

	Panel*	p	= new Panel;
	p->Name	= "Новая";
    if(cur < (int)m_PanelList.size())	m_PanelList.insert(m_PanelList.begin()+cur+1, p);
    else                                m_PanelList.push_back(p);
	on_PanelListChanged();
	pBox->setCurrentIndex(cur+1);
}

void	GraphicsDoc::on_PanelDelete()
{
	//Удаляем текущую панель
	QComboBox*	pBox	= m_pPanelSelect->ui->comboBox;
	int	cur	= pBox->currentIndex();
	if(cur == -1)	return;

	delete m_PanelList.at(cur);
	m_PanelList.erase(m_PanelList.begin()+cur);

	//Должна остаться хоть одна панель
	if(m_PanelList.size() == 0)
	{
		pBox->setCurrentIndex(-1);
		on_PanelAdd();
	}
	on_PanelListChanged();
    if(cur > (int)(m_PanelList.size()-1))	cur	= m_PanelList.size()-1;
	pBox->setCurrentIndex(cur);
}

void	GraphicsDoc::on_PanelCopy()
{
	//Копируем текущую панель
	QComboBox*	pBox	= m_pPanelSelect->ui->comboBox;
	int	cur	= pBox->currentIndex();

	Panel*	p	= m_PanelList.at(cur);
	Panel*	pNew	= new Panel;
	pNew->Name	= "Копия_" + p->Name;
	
	//Копируем оси
    for(size_t i = 0; i < p->Axes.size(); i++)
	{
		GAxe*	pAxe	= p->Axes.at(i);
//		GAxe*	pAxe2	= new GAxe(pAxe);
//		pNew->Axes.push_back(pAxe2);
	}
	
	//Добавляем панель
	m_PanelList.insert(m_PanelList.begin()+cur+1, pNew);
	on_PanelListChanged();
	pBox->setCurrentIndex(cur+1);
}

void	GraphicsDoc::on_PanelRenamed(const QString &text)
{
	QComboBox*	pBox	= m_pPanelSelect->ui->comboBox;
	int	cur	= pBox->currentIndex();
	if(cur == -1)	return;
    if(cur > (int)(m_PanelList.size()-1))	return;
	Panel*	p	= m_PanelList.at(cur);
	p->Name	= text;
	pBox->setItemText(cur, text);
}

void GraphicsDoc::on_actionAddAxe_triggered()
{
    GraphSelect dlg(this);
	dlg.SetAccumulation(&m_BufArray);
    if(dlg.exec() == QDialog::Accepted)
    {
        //Добавляем ось
        int a = dlg.m_nBufIndex;
        int b = dlg.m_nAccIndex;
    }
}
