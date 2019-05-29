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
	oglView = new GraphicsView();
	oglView->setObjectName(QStringLiteral("oglView"));
	container	= createWindowContainer(oglView, this);
	ui->gridLayout->addWidget(container, 0, 0, 1, 1);
	container->setFocus();
	oglView->setUI(ui);

	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 0);
	ui->actionZoom->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GraphicsDoc");
	if(settings.contains("splitter"))	ui->splitter->restoreState(settings.value("splitter").toByteArray());
	settings.endGroup();

	connect(this, &GraphicsDoc::panelChanged, oglView, &GraphicsView::on_panelChanged);
	connect(this, &GraphicsDoc::panelChanged, ui->locator, &LocatorView::on_panelChanged);
	connect(this, &GraphicsDoc::panelDeleted, oglView, &GraphicsView::on_panelDeleted);
	connect(oglView, &GraphicsView::timeChanged, ui->locator, &LocatorView::on_timeChanged, Qt::QueuedConnection);
	connect(oglView, &GraphicsView::axesMoved, ui->locator, &LocatorView::on_axesMoved, Qt::QueuedConnection);
	connect(oglView, &GraphicsView::axesRenamed, ui->locator, &LocatorView::on_axesRenamed, Qt::QueuedConnection);
	connect(oglView, &GraphicsView::hasSelectedAxes, ui->locator, &LocatorView::on_axeSelected);

    m_pPanelSelect  = new PanelSelect(ui->toolBarPanel);
    ui->toolBarPanel->addWidget(m_pPanelSelect);
	connect(m_pPanelSelect->ui->pushButtonAdd, &QPushButton::clicked, this, &GraphicsDoc::on_PanelAdd);
	connect(m_pPanelSelect->ui->pushButtonDelete, &QPushButton::clicked, this, &GraphicsDoc::on_PanelDelete);
	connect(m_pPanelSelect->ui->pushButtonCopy, &QPushButton::clicked, this, &GraphicsDoc::on_PanelCopy);
	connect(m_pPanelSelect->ui->comboBox, &QComboBox::editTextChanged, this, &GraphicsDoc::on_PanelRenamed);
	connect(m_pPanelSelect->ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GraphicsDoc::on_PanelIndexChanged);

    m_bAddAcc_Mode  = false;
	m_colorIndex	= 0;
	m_markerIndex	= 0;

	//Создаем новую пустую панель
	Panel*	p	= new Panel;
	p->Name	= "Новая";
	m_PanelList.clear();
	m_PanelList.push_back(p);
	m_pActivePanel	= p;
	on_PanelListChanged();

	connect(oglView, &GraphicsView::dt, [=](int msecs){if(msecs) ui->statusBar->showMessage(QString("Темп %1").arg(msecs), 100);});
	connect(oglView, &GraphicsView::change_axe, this, &GraphicsDoc::on_changeAxe);
	connect(oglView, &GraphicsView::delete_axe, this, &GraphicsDoc::on_deleteAxe);
	connect(oglView, &GraphicsView::substract_axe, this, &GraphicsDoc::on_substractAxe);
	connect(this, &GraphicsDoc::axeAdded, oglView, &GraphicsView::on_axeAdded);

	//Меню
	connect(ui->action_LoadSapr, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadSapr); });
	connect(ui->action_LoadOrion, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadOrion);});
	connect(ui->action_LoadTRF, &QAction::triggered, [this]{on_menu_LoadData(ui->action_LoadTRF); });

	connect(ui->actionReload, &QAction::triggered, [this]
	{
		preloadPanel();
		emit panelChanged(&m_pActivePanel->Axes);}
	);
}

GraphicsDoc::~GraphicsDoc()
{
	//Сохраняем сплиттер
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GraphicsDoc");
	settings.setValue("splitter", ui->splitter->saveState());
	settings.endGroup();
	settings.sync();

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

	delete oglView;
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
	oglView->loadAxeArg(&arg, root.attribute("version").toDouble());

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
		oglView->saveAxeArg(xml);
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
        acc_type	= Accumulation::AccType::Acc_Orion;
		fileType	= "*.orion";
	}
	else if(pAction == ui->action_LoadSapr)	
	{
        acc_type	= Accumulation::AccType::Acc_SAPR;
		fileType	= "*.buf";
	}
	else if(pAction == ui->action_LoadTRF)
	{
        acc_type	= Accumulation::AccType::Acc_TRF;
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
        case Accumulation::AccType::Acc_Orion:	pAcc	= new Orion_Accumulation;	break;
        case Accumulation::AccType::Acc_SAPR:	pAcc	= new Sapr_Accumulation;	break;
        case Accumulation::AccType::Acc_TRF:	pAcc	= new TRF_Accumulation;		break;
		default:
			break;
	}
	
    pAcc->setName(QString("Данные №%1").arg(m_BufArray.size()+1));
	m_BufArray.push_back(pAcc);

	pAcc->load(FileName);
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
	emit panelChanged(&m_pActivePanel->Axes);
	ui->statusBar->showMessage(QString("Загружен файл %1").arg(FileName), 5000);
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
		//Выделяем последнее имя
		QStringList	pathList	= dlg.m_Path.split('\\');
		QString	name	= pathList.back();
		if(name == "x" || name == "y" || name == "z")
			name	= pathList.at(pathList.size()-2) + "." + pathList.back();

		//Определяем цвет
		QColor	c	= QColorDialog::standardColor(m_colorIndex);
//		QColor	c	= QColorDialog::standardColor(48*rand()/RAND_MAX);

		//Добавляем ось
		Graph::GAxe* pAxe	= new Graph::GAxe;
		pAxe->m_Name		= name;
		pAxe->m_Path		= dlg.m_Path;
		pAxe->m_Color		= vec3(c.red()/255., c.green()/255., c.blue()/255.);
		pAxe->m_nMarker		= m_markerIndex;
		pAxe->m_AxeMin		= -10;
		pAxe->m_AxeScale	= 10;
		pAxe->SetPosition(-20, 200);
		pAxe->setAxeLength(4);

		//Сдвигаем индексы цвета и маркера
		if(m_colorIndex++ > 48)		m_colorIndex	= 0;
		if(m_markerIndex++ > 20)	m_colorIndex	= 0;

		//Добавляем информацию в активную панель
		m_pActivePanel->Axes.push_back(pAxe);

		preloadPanel();
		pAxe->fitToScale();
		pAxe->initializeGL();

		emit panelChanged(&m_pActivePanel->Axes);
		emit axeAdded(pAxe);

		//Вызовем диалог параметров оси
		{
			vector<Graph::GAxe*>	axes;
			axes.push_back(pAxe);

			GAxe_dialog*	dlg	= new GAxe_dialog(&axes, ui->centralwidget);
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
		//Выделяем последнее имя
		QStringList	pathList	= dlg.m_Path.split('\\');
		QString	name	= pathList.back();
		if(name == "x" || name == "y" || name == "z")
			name	= pathList.at(pathList.size()-2) + "." + pathList.back();

		//Меняем ось
		pAxe->m_Name	= name;
		pAxe->m_Path	= dlg.m_Path;
		pAxe->m_DeltaPath.clear();
		preloadPanel();
		pAxe->fitToScale();
		
		//Обновляем графики и таблицу
		emit panelChanged(&m_pActivePanel->Axes);
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
	emit panelChanged(&m_pActivePanel->Axes);
}

void	GraphicsDoc::on_substractAxe(Graph::GAxe* pAxe, QWidget* pDlg)
{
	GraphSelect	dlg(pDlg);
	dlg.SetAccumulation(&m_BufArray);
	if(pAxe->m_DeltaPath.isEmpty())	dlg.SetPath(pAxe->m_Path);
	else							dlg.SetPath(pAxe->m_DeltaPath);
	if(dlg.exec() == QDialog::Accepted)
	{
		//Выделяем последнее имя
		QStringList	pathList	= dlg.m_Path.split('\\');
		QString	name	= pathList.back();
		if(name == "x" || name == "y" || name == "z")
			name	= pathList.at(pathList.size()-2) + "." + pathList.back();

		//Меняем ось
		pAxe->m_Name		= pAxe->m_Name + " - " + name;
		pAxe->m_DeltaPath	= dlg.m_Path;
		preloadPanel();
		pAxe->fitToScale();

		//Обновляем графики и таблицу
		emit panelChanged(&m_pActivePanel->Axes);
	}
}

GAxe::DataType	toGAxe(const Accumulation::DataType nType)
{
	switch(nType)
	{
		case Accumulation::DataType::Bool:		return GAxe::DataType::Bool;
		case Accumulation::DataType::Int:		return GAxe::DataType::Int;
		case Accumulation::DataType::Double:	return GAxe::DataType::Double;
        case Accumulation::DataType::Float:		return GAxe::DataType::Float;
        case Accumulation::DataType::Short:		return GAxe::DataType::Short;
		default:
		{
			QMessageBox::critical(nullptr, "Преобразование Accumulation::DataType", "Ошибка!!!");
			throw;
		}
	}
}

void	GraphicsDoc::preloadPanel()
{
	//////////////////////////////////////////////////////////////////////////
	//Предварительная загрузка данных для текущей панели

	//Перечень путей для загрузки
	QStringList	list;
	for(size_t i = 0; i < m_pActivePanel->Axes.size(); i++)
	{
		Graph::GAxe*	pAxe	= m_pActivePanel->Axes.at(i);
		list.push_back(pAxe->m_Path);

		//Для дельт добавляем дополнительный путь
		if(!pAxe->m_DeltaPath.isEmpty())
			list.push_back(pAxe->m_DeltaPath);
	}

	//Загружаем данные в набор накоплений
	for(size_t i = 0; i < m_BufArray.size(); i++)
	{
		Accumulation*	pAcc	= m_BufArray.at(i);
		pAcc->preloadData(&list);
	}

	//Перекидываем данные в графики
	for(size_t i = 0; i < m_pActivePanel->Axes.size(); i++)
	{
		GAxe*	pAxe	= m_pActivePanel->Axes.at(i);

		//Ищем нужное для графика накопление по имени
		int	nSlash	= pAxe->m_Path.indexOf('\\');
		if(nSlash == -1)	continue;
		QString		path		= pAxe->m_Path.mid(nSlash+1);
		QString		accName		= pAxe->m_Path.left(nSlash);
		QString		path_delta;

		//Путь для дельты
		if(!pAxe->m_DeltaPath.isEmpty())
		{
			int nSlash	= pAxe->m_DeltaPath.indexOf('\\');
			if(nSlash == -1)	continue;
			path_delta	= pAxe->m_DeltaPath.mid(nSlash+1);
		}

		const Accumulation*	pAcc	= nullptr;
		for(size_t j = 0; j < m_BufArray.size(); j++)
		{
			const Accumulation*	acc	= m_BufArray.at(j);
			if(acc->name()	== accName)
			{
				pAcc	= acc;
				break;
			}
		}
		if(pAcc == nullptr)
		{
			//Раз не нашли, то чистим
			pAxe->clearData();
			continue;
		}

		//Получаем данные
		size_t			len;
		const double*	pTime;
		const char*		pData;
		Accumulation::DataType	nType;
		if(!pAcc->getData(path, &len, &pTime, &pData, &nType))
		{
			//Раз не нашли, то чистим
			pAxe->clearData();
			continue;
		}

		//Загружаем данные в ось
		pAxe->uploadData(len, pTime, pData, toGAxe(nType));

		if(!pAxe->m_DeltaPath.isEmpty())
		{
			//Считаем дельту
			size_t			delta_len;
			const double*	delta_pTime;
			const char*		delta_pData;
			Accumulation::DataType	delta_nType;
			if(!pAcc->getData(path_delta, &delta_len, &delta_pTime, &delta_pData, &delta_nType))
			{
				continue;
			}
			
			GAxe	axe;
			axe.m_bInterpol	= pAxe->m_bInterpol;
			axe.uploadData(delta_len, delta_pTime, delta_pData, toGAxe(delta_nType));

			double*	pDelta	= new double[len];
            for(size_t i = 0; i < len; i++)
			{
				*(pDelta + i)	= pAxe->GetValueAtTime(pTime[i]) - axe.GetValueAtTime(pTime[i]);
			}
			
			//Принудительно выставляем тип Double
			pAxe->uploadData(len, pTime, (const char*)pDelta, Graph::GAxe::DataType::Double);
			delete[] pDelta;
		}
	}
}
