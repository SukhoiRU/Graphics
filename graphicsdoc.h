#ifndef GRAPHICSDOC_H
#define GRAPHICSDOC_H
#include "stdafx.h"
#include "Accumulation.h"
#include <vector>
using std::vector;

namespace Ui {
class GraphicsDoc;
}
namespace Graph{
	class GAxe;
}

class PanelSelect;
class GraphicsView;

struct	Panel
{
    QString					Name;		//Название панели
    vector<Graph::GAxe*>	Axes;		//Список графиков в панели
    Panel(){}
    ~Panel();
};

class GraphicsDoc : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicsDoc(QWidget *parent = nullptr);
    ~GraphicsDoc();

	void	loadScreen(QString FileName);
	void	saveScreen(QString FileName);
	void	loadData(QString FileName, const Accumulation::AccType acc_type);

private slots:
	void	on_actionOpen_triggered();
	void	on_menu_LoadData(QAction* pAction);
	void	on_actionSave_triggered();
	void	on_actionSaveAs_triggered();
	void	on_actionAddAxe_triggered();
	void	on_changeAxe(Graph::GAxe* pAxe, QWidget* pDlg);
	void	on_deleteAxe(vector<Graph::GAxe*>* pAxes);
	void	on_substractAxe(Graph::GAxe* pAxe, QWidget* pDlg);
protected:
	virtual void	dragEnterEvent(QDragEnterEvent *event) override;
	virtual void	dropEvent(QDropEvent *event) override;
signals:
    void	panelChanged(vector<Graph::GAxe*>* axes);
	void	panelDeleted(vector<Graph::GAxe*>* axes);
	void	dataChanged(std::vector<Accumulation*>* pBuffer);	//Признак загрузки новых данных
	void	dataRemoved(std::vector<Accumulation*>* pBuffer);	//Признак смены данных в том же накоплении
	void	axeAdded(Graph::GAxe* pAxe);

private:
    Ui::GraphicsDoc *ui;
	GraphicsView*	oglView;
	QWidget*		container;
    PanelSelect*    m_pPanelSelect;
	QString			m_screenFileName;
	QString			m_dataFileName;

private:
    vector<Accumulation*>   m_BufArray;	//Набор накоплений
    vector<Panel*>	m_PanelList;		//Список панелей просмотра
    Panel*			m_pActivePanel;		//Указатель на активную панель
    bool			m_bAddAcc_Mode;		//Режим добавления накоплений
    int				m_nLastColor;		//Индекс последнего цвета графика
    int				m_nLastMarker;		//Индекс последнего маркера
	QString			m_oldPath;			//Данные о прошлой добавленной оси
	int				m_colorIndex;		//Текущий номер стандартного цвета
	int				m_markerIndex;		//Текущий номер маркера

	void on_PanelListChanged();
	void on_PanelAdd();
	void on_PanelDelete();
	void on_PanelCopy();
	void on_PanelRenamed(const QString &text);
	void on_PanelIndexChanged(int index);
	void preloadPanel();
};

#endif // GRAPHICSDOC_H
