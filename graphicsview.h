#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <glm/glm.hpp>
using glm::vec2;
using glm::mat4;
#include <vector>
using std::vector;

class QOpenGLShaderProgram;
class QScrollBar;
class PageSetup;
class Accumulation;

namespace Graph{
class GraphObject;
class GAxe;
class GAxeArg;
}

class GTextLabel;

class GraphicsView : public QOpenGLWidget//, QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

// OpenGL Events
QTime	t0;
public:
	explicit GraphicsView(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~GraphicsView();

    virtual void	initializeGL();
    virtual void	resizeGL(int width, int height);
    virtual void	paintGL();

    void	setPageInfo();

	QScrollBar*	vBar;
	QScrollBar*	hBar;

public slots:
	void	openPageSetup();
	void	updatePage();
    void    setScale(float scale);
    void    saveSVG();
    void	on_panelChanged(vector<Graph::GAxe*>* axes, std::vector<Accumulation*>* pBuffer);
    void    pause(bool hold);

protected slots:
	void	teardownGL();
	void	update();
	void	updatePageBuffer();
signals:
	void	dt(int msecs);

protected:
	//Обработка мыши и клавиатуры
	vec2	mouseToDoc(QMouseEvent *event);
	void	SelectObject(Graph::GraphObject* pGraph);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

	void	paintOverGL(QPainter* p);

private:
	//Информация о странице
	QSizeF		pageSize;
	QRectF		pageBorders;
	GLfloat		bdWidth;	//Толщина линии рамки
	QRectF		graphBorders;
	QSizeF		gridStep;
	vec2		m_mousePos;	//Текущее положение курсора
	vec2		m_clickPos;	//Точка щелчка мышью
	bool		m_bOnMouse;
    float       m_scale;    //Масштаб [пиксель/мм]
	PageSetup*	pPageSetup;
	Graph::GAxeArg*				axeArg;		//Отрисовка времени
    vector<Graph::GAxe*>*		m_pPanel;	//Указатель на панель
	vector<Graph::GraphObject*>	m_GraphObjects;
	Graph::GraphObject*			m_pSelectedObject;

	//Начальное время и масштаб
	double		Time0;
	double		TimeScale;
	double		curTime;
	
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
};

#endif // GRAPHICS_VIEW_H
