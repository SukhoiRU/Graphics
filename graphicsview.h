#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include "stdafx.h"
#include <glm/glm.hpp>
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
class Accumulation;
class QDomElement;
class graphSettings;

namespace Graph{
class GraphObject;
class GAxe;
class GAxeArg;
class GTextLabel;
}

class GTextLabel;

class GraphicsView : public QOpenGLWidget
{
	Q_OBJECT

// OpenGL Events
public:
    explicit GraphicsView(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~GraphicsView();
	void	setUI(Ui::GraphicsDoc* pUI);

    virtual void	initializeGL() override;
    virtual void	resizeGL(int width, int height) override;
    virtual void	paintGL() override;
	void	drawScene();

    void	setPageInfo();

	QScrollBar*	vBar;
	QScrollBar*	hBar;
	QStatusBar*	pStatus;

public slots:
	void	openPageSetup();
	void	updatePage();
    void    setScale(float scale);
    void    saveSVG();
	void	on_panelChanged(vector<Graph::GAxe*>* axes, std::vector<Accumulation*>* pBuffer);
    void	on_panelDeleted(vector<Graph::GAxe*>* axes);
	void	on_graphSettings();
	void	saveAxeArg(QXmlStreamWriter& xml);
	void	loadAxeArg(QDomElement* e, double ver);
protected slots:
	void	teardownGL();
	void	update();
	void	updatePageBuffer();
signals:
	void	dt(int msecs);
	void	timeChanged(double time);
	void	hasSelectedAxes(bool bSelected);
	void	axesMoved();
	void	axesRenamed();

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

	void	paintOverGL(QPainter* p);

private:
	Ui::GraphicsDoc *ui;

	//Информация о странице
	QSizeF		pageSize;
	QRectF		pageBorders;
	GLfloat		bdWidth;	//Толщина линии рамки
	QRectF		graphBorders;
	vec2		gridStep;
	vec2		m_mousePos;	//Текущее положение курсора
	vec2		m_clickPos;	//Точка щелчка мышью
	bool		m_bOnMouse;
    float       m_scale;    //Масштаб [пиксель/мм]
	PageSetup*	pPageSetup;
	Graph::GAxeArg*				axeArg;		//Отрисовка времени
    vector<Graph::GAxe*>*		m_pPanel;	//Указатель на панель
	vector<Graph::GraphObject*>	m_GraphObjects;
	vector<Graph::GraphObject*>	m_SelectedObjects;
	Graph::GTextLabel*			m_pLabel;
	vector<Accumulation*>*		m_pBuffer;
	graphSettings*				m_pGraphSettings;

	//Начальное время и масштаб
	double		Time0;
	double		TimeScale;
	double		curTime;
	bool		m_bTurning;
	bool		m_bPerspective;

	//Реальное время
	QTime	timer;
	double	modelTime;
	double	timeStep;

	float	timeMoving;

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

	QOpenGLFramebufferObject*	qFBO;
	QOpenGLFramebufferObject*	qFBO_unsamled;
	QOpenGLShaderProgram*	m_fbo_program;
	GLuint	fboVBO;
	GLuint	fbo, fboTexture[2];

};

#endif // GRAPHICS_VIEW_H
