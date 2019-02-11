#ifndef GRAPHICSDOC_H
#define GRAPHICSDOC_H
#include "stdafx.h"
#include <vector>
using std::vector;

namespace Ui {
class GraphicsDoc;
}
namespace Graph{
	class GAxe;
}

class PanelSelect;
class Accumulation;
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

	void LoadScreen(QString FileName);
	void LoadOrion(QString FileName);

private slots:
    void on_actionOpen_triggered();
    void on_action_LoadOrion_triggered();

    void on_actionAddAxe_triggered();

signals:
    void	panelChanged(vector<Graph::GAxe*>* axes, std::vector<Accumulation*>* pBuffer);
	void	panelDeleted(vector<Graph::GAxe*>* axes);
	void	dataChanged(std::vector<Accumulation*>* pBuffer);	//Признак загрузки новых данных
	void	dataRemoved(std::vector<Accumulation*>* pBuffer);	//Признак смены данных в том же накоплении

private:
    Ui::GraphicsDoc *ui;
    PanelSelect*    m_pPanelSelect;

private:
    vector<Accumulation*>   m_BufArray;	//Набор накоплений
    vector<Panel*>	m_PanelList;		//Список панелей просмотра
    Panel*			m_pActivePanel;		//Указатель на активную панель
    bool			m_bAddAcc_Mode;		//Режим добавления накоплений
    int				m_nLastColor;		//Индекс последнего цвета графика
    int				m_nLastMarker;		//Индекс последнего маркера

	void on_PanelListChanged();
	void on_PanelAdd();
	void on_PanelDelete();
	void on_PanelCopy();
	void on_PanelRenamed(const QString &text);
	void on_PanelIndexChanged(int index);
};

#endif // GRAPHICSDOC_H
