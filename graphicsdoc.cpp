#include "stdafx.h"
#include "graphicsdoc.h"
#include "graphicsview.h"
#include <QDomDocument>
#include "ui_graphicsdoc.h"
#include "ui_panelselect.h"
#include "Accumulation.h"
#include "Orion_Accumulation.h"
#include "Sapr_Accumulation.h"
#include "TRF_Accumulation.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "Dialogs/panelselect.h"
#include "Dialogs/graphselect.h"
#include "Dialogs/locator_view.h"
#include "Graph/GAxe.h"
#include "Graph/GTextLabel.h"
#include "Dialogs/gaxe_dialog.h"

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
	ui->oglView->setUI(ui);
	ui->oglView->setFocus();
	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 0);

	connect(this, &GraphicsDoc::panelChanged, ui->oglView, &GraphicsView::on_panelChanged);
	connect(this, &GraphicsDoc::panelChanged, ui->locator, &LocatorView::on_panelChanged);
	connect(this, &GraphicsDoc::panelDeleted, ui->oglView, &GraphicsView::on_panelDeleted);
	connect(ui->oglView, &GraphicsView::timeChanged, ui->locator, &LocatorView::on_timeChanged, Qt::QueuedConnection);
	connect(ui->oglView, &GraphicsView::axesMoved, ui->locator, &LocatorView::on_axesMoved, Qt::QueuedConnection);
	connect(ui->oglView, &GraphicsView::axesRenamed, ui->locator, &LocatorView::on_axesRenamed, Qt::QueuedConnection);
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
	connect(ui->oglView, &GraphicsView::change_axe, this, &GraphicsDoc::on_changeAxe);
	connect(ui->oglView, &GraphicsView::delete_axe, this, &GraphicsDoc::on_deleteAxe);
	connect(this, &GraphicsDoc::axeAdded, ui->oglView, &GraphicsView::on_axeAdded);

	//Меню
//	connect(ui->menu_LoadData, &QMenu::triggered, this, &GraphicsDoc::on_menu_LoadData);
	connect(ui->action_LoadSapr, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadSapr); });
	connect(ui->action_LoadOrion, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadOrion);});
	connect(ui->action_LoadTRF, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadTRF); });
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

	//Удаляем статические данные объектов
	Graph::GAxe::finalDelete();
	Graph::GTextLabel::finalDelete();

	delete ui->oglView;
	delete ui;
}

void GraphicsDoc::on_actionOpen_triggered()
{
	//Открываем файл экрана
    QFileDialog::Options option;
#ifdef __linux__
    option = QFileDialog::DontUseNativeDialog;
#endif
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение файла экрана", "", "*.grf", nullptr, option);
    if(FileName.isEmpty())  return;

	loadScreen(FileName);
}

void GraphicsDoc::loadScreen(QString FileName)
{
	//Чтение файла экрана
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

	if(root.hasAttribute("version") && root.attribute("version").toDouble() < 1.0)
	{
		QMessageBox::critical(this, "Чтение файла экрана", "Версия экрана должна быть не меньше 1.0!");
		return;
	}

	//Читаем настройки оси времени
	QDomElement	arg	= root.firstChildElement("Ось_времени");
	ui->oglView->loadAxeArg(&arg, root.attribute("version").toDouble());

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

	//Получаем версию
	QDomElement e_ver = e.firstChildElement("Версия");
	double	ver	= e_ver.text().toDouble();

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
			pAxe->load(&g, ver);
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

	//Сохраняем путь к файлу
	m_screenFileName	= FileName;
}

void	GraphicsDoc::on_actionSave_triggered()
{
	//Сохраняем файл экрана
	if(!m_screenFileName.isEmpty())
		saveScreen(m_screenFileName);
}

void	GraphicsDoc::on_actionSaveAs_triggered()
{
	//Сохраняем файл экрана
	QFileDialog::Options option;
#ifdef __linux__
	option = QFileDialog::DontUseNativeDialog;
#endif
	QString	FileName	= QFileDialog::getSaveFileName(this, "Запись файла экрана", "", "*.grf", nullptr, option);
	if(FileName.isEmpty())  return;

	saveScreen(FileName);
}

void	GraphicsDoc::saveScreen(QString FileName)
{
	QFile	file(FileName);
	if(!file.open(QFile::WriteOnly))
	{
		QMessageBox::critical(this, "Ошибка сохранения файла экрана!", QString("Cannot open file %1: %2.").arg(FileName).arg(file.errorString()));
		return;
	}

	QXmlStreamWriter	xml(&file);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("Файл_экрана");
	xml.writeAttribute("version", "2.0");
	{
		xml.writeStartElement("Ось_времени");
		ui->oglView->saveAxeArg(xml);
		xml.writeEndElement();	//Ось_времени
	}
	xml.writeStartElement("Список_панелей");
	xml.writeTextElement("Версия", "2.1");
	xml.writeTextElement("Активная_панель", QString::number(m_pPanelSelect->ui->comboBox->currentIndex()));
	xml.writeTextElement("Текущий_цвет", "5");

	//Пишем список панелей
	for(size_t i = 0; i < m_PanelList.size(); i++)
	{
		Panel*	panel	= m_PanelList.at(i);
		xml.writeStartElement("Панель");
		xml.writeAttribute("Название", panel->Name);
		for(size_t j = 0; j < panel->Axes.size(); j++)
		{
			Graph::GAxe*	pAxe	= panel->Axes.at(j);
			pAxe->save(xml);
		}
		xml.writeEndElement(); //Панель
	}
	xml.writeEndElement();	//Список_панелей
	xml.writeEndElement();	//Файл_экрана
	xml.writeEndDocument();
	file.close();
}

void GraphicsDoc::on_menu_LoadData(QAction* pAction)
{
	//Определяем тип накопления
	Accumulation::AccType	acc_type;
	QString	title	=  pAction->text();
	QString	fileType;
	if(pAction == ui->action_LoadOrion)
	{
		acc_type	= Accumulation::Acc_Orion;
		fileType	= "*.orion";
	}
	else if(pAction == ui->action_LoadSapr)	
	{
		acc_type	= Accumulation::Acc_SAPR;
		fileType	= "*.buf";
	}
	else if(pAction == ui->action_LoadTRF)
	{
		acc_type	= Accumulation::Acc_TRF;
		fileType	= "*.trf";
	}


    QFileDialog::Options option;
#ifdef __linux__
    option = QFileDialog::DontUseNativeDialog;
#endif
    QString	FileName	= QFileDialog::getOpenFileName(this, "Чтение накопления " + title, "", fileType, nullptr, option);
    if(FileName.isEmpty())  return;

    // TODO: Add your dispatch handler code here
    QFile	in(FileName);
    if(!in.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, QString("Ошибка чтения файла накопления %1!").arg(title), QString("Ошибка чтения %1: %2.").arg(FileName).arg(in.errorString()));
        return;
    }
    in.close();

	loadData(FileName, acc_type);
	ui->statusBar->showMessage(QString("Загружен файл %1").arg(FileName), 5000);
}

void	GraphicsDoc::loadData(QString FileName, const Accumulation::AccType acc_type)
{
    Accumulation*	pAcc;

	//Удаляем последнее накопление
	if(!m_bAddAcc_Mode && m_BufArray.size())
	{
		pAcc	= m_BufArray.back();
		delete	pAcc;
		m_BufArray.pop_back();
		emit dataRemoved(&m_BufArray);
	}

	//Добавляем новое накопление
	switch(acc_type)
	{
		case Accumulation::Acc_Orion:	pAcc	= new Orion_Accumulation;	break;
		case Accumulation::Acc_SAPR:	pAcc	= new Sapr_Accumulation;	break;
		case Accumulation::Acc_TRF:		pAcc	= new TRF_Accumulation;		break;
		default:
			break;
	}
	
	pAcc->setName(QString("Данные №%1").arg(m_BufArray.size()+1).toStdString());
	m_BufArray.push_back(pAcc);

	pAcc->load(FileName.toLocal8Bit());
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
	preloadPanel();
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

	preloadPanel();
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

	emit panelDeleted(&m_PanelList.at(cur)->Axes);
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
	if(!m_pActivePanel)			return;
	if(m_BufArray.empty())		return;

	GraphSelect dlg(this);
	dlg.SetAccumulation(&m_BufArray);
	if(!m_oldPath.isEmpty())
		dlg.SetPath(m_oldPath);

    if(dlg.exec() == QDialog::Accepted)
    {
		QStringList	pathList	= dlg.m_Path.split('\\');

		//Добавляем ось
		Graph::GAxe* pAxe	= new Graph::GAxe;
		pAxe->m_Name		= pathList.back();
		pAxe->m_Path		= dlg.m_Path;
		pAxe->m_Color		= vec3(1.0,0.,0.);//GetNextColor();
		pAxe->m_nMarker		= 0;//GetNextMarker();
		pAxe->m_AxeMin		= -10;
		pAxe->m_AxeScale	= 10;
		pAxe->SetPosition(-20, 200);
		pAxe->setAxeLength(4);

		//Добавляем информацию в активную панель
		m_pActivePanel->Axes.push_back(pAxe);

		preloadPanel();
		pAxe->updateRecord(&m_BufArray);
		pAxe->fitToScale();
		pAxe->initializeGL();

		emit panelChanged(&m_pActivePanel->Axes, &m_BufArray);
		emit axeAdded(pAxe);

		//Вызовем диалог параметров оси
		{
			vector<Graph::GAxe*>	axes;
			axes.push_back(pAxe);

			GAxe_dialog*	dlg	= new GAxe_dialog(&axes, ui->oglView);
			connect(dlg, &GAxe_dialog::change_axe, [=](Graph::GAxe* pAxe){on_changeAxe(pAxe, dlg);});
			dlg->exec();
			delete	dlg;
		}

		//Запоминаем данные об оси
		m_oldPath	= pAxe->m_Path;
    }
}

void	GraphicsDoc::on_changeAxe(Graph::GAxe* pAxe, QWidget* pDlg)
{
	GraphSelect	dlg(pDlg);
	dlg.SetAccumulation(&m_BufArray);
	QString	path	= pAxe->m_Path;
	dlg.SetPath(path);
	if(dlg.exec() == QDialog::Accepted)
	{
		//Выделяем последнее имя и убираем имена накопления
		QStringList	pathList	= dlg.m_Path.split('\\');

		//Меняем ось
		pAxe->m_Name	= pathList.back();
		pAxe->m_Path	= dlg.m_Path;
		pAxe->updateRecord(&m_BufArray);
		pAxe->fitToScale();
	}
}

void	GraphicsDoc::on_deleteAxe(vector<Graph::GAxe *>* pAxes)
{
	//Удаляем заданный список осей
	for(size_t i = 0; i < m_pActivePanel->Axes.size(); i++)
	{
		Graph::GAxe*	pAxe	= m_pActivePanel->Axes.at(i);
		for(size_t j = 0; j < pAxes->size(); j++)
		{
			if(pAxe == pAxes->at(j))
			{
				m_pActivePanel->Axes.erase(m_pActivePanel->Axes.begin()+i);
				pAxes->erase(pAxes->begin()+j);
				delete pAxe;
				i--;
			}
		}
	}

	//Обновляем графики и таблицу
	emit panelChanged(&m_pActivePanel->Axes, &m_BufArray);
}

void	GraphicsDoc::preloadPanel()
{
	//Предварительная загрузка данных для текущей панели
	QStringList	list;
	for(size_t i = 0; i < m_pActivePanel->Axes.size(); i++)
	{
		Graph::GAxe*	pAxe	= m_pActivePanel->Axes.at(i);
		list.push_back(pAxe->m_Path);
	}

	for(size_t i = 0; i < m_BufArray.size(); i++)
	{
		Accumulation*	pAcc	= m_BufArray.at(i);
		pAcc->preloadData(&list);
	}
}
