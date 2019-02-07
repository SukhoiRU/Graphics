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
#include "Dialogs/locator_view.h"
#include "Graph/GAxe.h"

Panel::~Panel()
{
    for(size_t i = 0; i < Axes.size(); i++)	delete Axes.at(i);
    Axes.clear();
}

GraphicsDoc::GraphicsDoc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicsDoc)
{
    ui->setupUi(this);
	ui->oglView->vBar	= ui->verticalScrollBar;
	ui->oglView->hBar	= ui->horizontalScrollBar;
	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 0);

	connect(ui->actionPageInfo, &QAction::triggered, ui->oglView, &GraphicsView::openPageSetup);
	connect(this, &GraphicsDoc::panelChanged, ui->oglView, &GraphicsView::on_panelChanged, Qt::QueuedConnection);
	connect(this, &GraphicsDoc::panelChanged, ui->locator, &LocatorView::on_panelChanged, Qt::QueuedConnection);
	connect(ui->oglView, &GraphicsView::timeChanged, ui->locator, &LocatorView::on_timeChanged, Qt::QueuedConnection);
	connect(ui->oglView, &GraphicsView::axesMoved, ui->locator, &LocatorView::on_axesMoved, Qt::QueuedConnection);
	connect(ui->oglView, &GraphicsView::hasSelectedAxes, ui->locator, &LocatorView::on_axeSelected);

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
	m_pActivePanel	= p;
	on_PanelListChanged();

	connect(ui->oglView, &GraphicsView::dt, [=](int msecs){if(msecs) ui->statusBar->showMessage(QString("Темп %1").arg(msecs), 100);});
}

GraphicsDoc::~GraphicsDoc()
{
	//Очищаем список панелей
    for(size_t i = 0; i < m_PanelList.size(); i++)
	{
		Panel*	p	= m_PanelList.at(i);
		delete p;
	}
	m_PanelList.clear();

	delete ui->oglView;
	delete ui;
}

void GraphicsDoc::on_actionOpen_triggered()
{
	//Открываем файл экрана
    ui->oglView->pause(true);
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение файла экрана", "", "*.grf");
    ui->oglView->pause(false);
    if(FileName.isEmpty())  return;

	LoadScreen(FileName);
}

void GraphicsDoc::LoadScreen(QString FileName)
{
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
			Graph::GAxe*	pAxe	= new Graph::GAxe;
			pAxe->Load(&g);
			pPanel->Axes.push_back(pAxe);
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
    QString     path;
    ui->oglView->pause(true);
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение накопления Орион", path, "*.orion");
    ui->oglView->pause(false);
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
			emit dataRemoved(&m_BufArray);
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
	emit dataChanged(&m_BufArray);
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
	emit panelChanged(&m_pActivePanel->Axes, &m_BufArray);
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

    emit panelChanged(&m_pActivePanel->Axes, &m_BufArray);
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
		Graph::GAxe*	pAxe	= p->Axes.at(i);
		Graph::GAxe*	pAxe2	= new Graph::GAxe(*pAxe);
		pNew->Axes.push_back(pAxe2);
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
        int nAcc	= dlg.m_nBufIndex;
        int AccIndex = dlg.m_nAccIndex;

		if(!m_pActivePanel)			return;
		if(m_BufArray.empty())		return;

		//Создаем оси
		Accumulation*	pAcc					= m_BufArray.at(nAcc);

		Graph::GAxe*	pAxe	= new Graph::GAxe;
		pAxe->m_Name		= pAcc->GetName(AccIndex);
		pAxe->m_Path		= pAcc->GetPath(AccIndex);
		pAxe->m_nAcc		= nAcc;
		pAxe->m_Record		= AccIndex;
		pAxe->m_Color		= vec3(1.0,0.,0.);//GetNextColor();
		pAxe->m_nMarker		= 0;//GetNextMarker();
		pAxe->setAxeLength(4);
		pAxe->m_Min			= -10;
		pAxe->m_AxeScale	= 10;
		pAxe->SetPosition(20, 200);
//		pAxe->UpdateRecord();

		//Axe.FitToScale();

		////Вызовем диалог параметров оси
		//Dialog::CAxeParam	dlg(&Axe);
		//if(IDOK != dlg.DoModal()) return 0;

		//Добавляем информацию в активную панель
		m_pActivePanel->Axes.push_back(pAxe);

		emit panelChanged(&m_pActivePanel->Axes, &m_BufArray);
    }
}
