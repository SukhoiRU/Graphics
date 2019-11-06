#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include "stdafx.h"
#define GLM_FORCE_SIMD_AVX2
#include <glm/glm.hpp>

#include <QPrinter>
using glm::vec2;
using glm::mat4;
#include <vector>
using std::vector;

namespace Ui {
	class GraphicsDoc;
}

class QOpenGLShaderProgram;
class QScrollBar;
class PageSetup;
class QDomElement;
class graphSettings;

namespace Graph{
class GraphObject;
class GAxe;
class GAxeArg;
class GTextLabel;
}

class GTextLabel;

class GraphicsView : public QWindow
{
	Q_OBJECT

private:
	QWidget*		m_pDoc	= nullptr;
	QOpenGLContext*	m_context;
	bool			fromInit	= false;

	virtual bool event(QEvent *event) override;
	virtual void resizeEvent(QResizeEvent *e) override;
	virtual void exposeEvent(QExposeEvent *event) override;

public:
    explicit GraphicsView(QWidget* pParent);
    virtual ~GraphicsView();
	void	setUI(Ui::GraphicsDoc* pUI);

    virtual void	initializeGL();
    virtual void	resizeGL(int width, int height);
    virtual void	paintGL();
	
    void	setPageInfo();

public slots:
	void	openPageSetup();
	void	updatePage();
    void    setScale(float scale);
    void    saveSVG();
	void	on_panelChanged(vector<Graph::GAxe*>* axes);
    void	on_panelDeleted();
	void	on_graphSettings();
	void	saveAxeArg(QXmlStreamWriter& xml);
	void	loadAxeArg(QDomElement* e, double ver);
	void	fitTime();
	void	fitPage();
	void	on_axeAdded(Graph::GAxe* pAxe);
	void	on_deleteAxes();

protected slots:
	void	teardownGL();
	void	update();
	void	updatePageBuffer();

signals:
	void	dt(int msecs);
	void	timeChanged(double time);
	void	timeLeftChanged(double time, bool use);
	void	timeRightChanged(double time, bool use);
	void	hasSelectedAxes(bool bSelected);
	void	axesMoved();
	void	axesRenamed();
	void	change_axe(Graph::GAxe* pAxe, QWidget* pDlg);
	void	delete_axe(vector<Graph::GAxe*>* pAxes);
	void	substract_axe(Graph::GAxe* pAxe, QWidget* pDlg);

protected:
	//Обработка мыши и клавиатуры
	vec2	mouseToDoc(QMouseEvent *event);
	void	SelectObject(Graph::GraphObject* pGraph);
	void	UnSelectObject(Graph::GraphObject* pGraph);
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
	void	onCustomMenuRequested(QPoint pos);
	void	onZoomMode();
	void	shiftToScroll();	//Перекачка сдвига в полосы прокрутки

	void	print();

private:
	Ui::GraphicsDoc *ui;

	//Информация о странице
	QSizeF		pageSize;
	QRectF		pageBorders;
	GLfloat		bdWidth;	//Толщина линии рамки
	QRectF		graphBorders;
	vec2		gridStep;
	vec2		m_mousePos;	//Текущее положение курсора
	vec2		m_shift;	//Сдвиг для правильного zoom
	bool		m_bOnMouse;
    float       m_scale;    //Масштаб [пиксель/мм]
	int			width, height;
	PageSetup*	pPageSetup;
	Graph::GAxeArg*				axeArg;		//Отрисовка времени
    vector<Graph::GAxe*>*		m_pPanel;	//Указатель на панель
	vector<Graph::GraphObject*>	m_GraphObjects;
	vector<Graph::GraphObject*>	m_SelectedObjects;
	Graph::GTextLabel*			m_pLabel;
	graphSettings*				m_pGraphSettings;

	bool		m_bZoomMode;	//Признак управления страницей
	QPoint		m_oldMouse;

	//Начальное время и масштаб
	double		Time0;
	double		TimeScale;
	double		curTime;
	bool		m_bTurning;
	bool		m_bPerspective;

	//Маркеры времени
	double		timeLeft;
	double		timeRight;
	bool		bDrawLeftTime;
	bool		bDrawRightTime;

	//Реальное время
	QTime	timer;
	double	modelTime;
	double	timeStep;

	// OpenGL State Information
	QOpenGLShaderProgram*	m_program;
	GLuint	pageVAO, pageVBO;
	bool	oglInited;

	//Shader Information
	int		u_modelToWorld;
	int		u_worldToCamera;
	int		u_cameraToView;
	mat4	m_proj;
	mat4	m_view;

	//Текстуры
	QOpenGLShaderProgram*	m_fbo_program;
	GLuint	fboGraphArea = 0, fboGraphAreaTexture = 0;	//Для сборки поля графиков
	bool	fboGraphAreaValid = false;

	GLuint	fboGraph = 0, fboGraphTexture = 0;	//Для отрисовки отдельных графиков

	void	drawGraphArea(const vec2& areaBL, const vec2& areaSize);
	QPrinter	printer;
};

#endif // GRAPHICS_VIEW_H
