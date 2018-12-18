#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>
using glm::vec2;
using glm::mat4;
#include <vector>
using std::vector;

class QOpenGLShaderProgram;
class QScrollBar;
class PageSetup;
namespace Graph{
class GraphObject;
class GAxe;
class GAxeArg;
}

class GraphicsView : public QOpenGLWidget//, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

// OpenGL Events
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
    void	on_panelChanged(vector<Graph::GAxe*>* axes);

protected slots:
	void	teardownGL();
	void	update();
	void	updatePageBuffer();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    void	paintOverGL(QPainter* p);

private:
	//Информация о странице
	QSize		pageSize;
	QRect		pageBorders;
	GLfloat		bdWidth;	//Толщина линии рамки
	QRect		graphBorders;
	QSize		gridStep;
	int			nGridCount;
	vec2		m_mousePos;
	bool		m_bOnMouse;
    float       m_scale;    //Масштаб [пиксель/мм]
	PageSetup*	pPageSetup;
	Graph::GAxeArg*				axeArg;		//Отрисовка времени
    vector<Graph::GAxe*>*		m_pPanel;	//Указатель на панель
	vector<Graph::GraphObject*>	m_GraphObjects;
	
	//Начальное время и масштаб
	double		Time0;
	double		TimeScale;
	
	// OpenGL State Information
	QOpenGLShaderProgram*		m_program;
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
