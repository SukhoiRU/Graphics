#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QOpenGLWidget>
#include <glm/glm.hpp>

class QOpenGLShaderProgram;
class QScrollBar;
class PageSetup;

class GraphicsView : public QOpenGLWidget
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
	QSize	pageSize;
	QRect	pageBorders;
	GLfloat	bdWidth;	//Толщина линии рамки
	QRect	graphBorders;
	QSize	gridStep;
	int		nGridCount;
	glm::vec2	m_mousePos;
	bool		m_bOnMouse;
    float       m_scale;    //Масштаб [пиксель/мм]
	PageSetup*	pPageSetup;
	
	// OpenGL State Information
	QOpenGLShaderProgram*		m_program;
	GLuint	pageVAO, pageVBO;

	// Shader Information
	int u_modelToWorld;
	int u_worldToCamera;
	int u_cameraToView;
	glm::mat4	m_proj;
	glm::mat4	m_view;
};

#endif // GRAPHICS_VIEW_H
