#ifndef GRAPHICSDOC_H
#define GRAPHICSDOC_H

#include <QMainWindow>
#include <vector>
using std::vector;

namespace Ui {
class GraphicsDoc;
}

class PanelSelect;
class GAxe;
class Accumulation;

struct	Panel
{
    QString				Name;		//Название панели
    vector<GAxe*>		Axes;		//Список графиков в панели
    Panel(){};
	~Panel()
	{
		for(int i = 0; i < Axes.size(); i++)	delete Axes.at(i);
		Axes.clear();
	}
};

class GraphicsDoc : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicsDoc(QWidget *parent = 0);
    ~GraphicsDoc();

private slots:
    void on_actionOpen_triggered();
    void on_action_LoadOrion_triggered();

    void on_actionAddAxe_triggered();

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
