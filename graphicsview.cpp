#include "stdafx.h"
#include "graphicsview.h"
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTime>
#include <QScrollBar>
#include <QPainter>
#include <QSvgGenerator>
#include "Dialogs/pageSetup.h"

/*
#ifdef __gl_h_
#undef __gl_h_
#undef GL_INVALID_INDEX
#undef GL_TIMEOUT_IGNORED

#undef GL_VERSION_1_1
#undef GL_VERSION_1_2
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0
#undef GL_VERSION_2_1

#undef GL_VERSION_3_0
#undef GL_VERSION_3_1
#undef GL_VERSION_3_2
#undef GL_VERSION_3_3
#endif

#include <glad/glad.h>
*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::scale;
using glm::translate;
using glm::rotate;
using glm::ortho;

#include <vector>
using std::max;

//#include <ft2build.h>
//#include FT_FREETYPE_H

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

GraphicsView::GraphicsView(QWidget* parent, Qt::WindowFlags f) :QOpenGLWidget(parent, f)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);
    format.setSamples(16);
    setFormat(format);

	pageSize.setWidth(450);
	pageSize.setHeight(297);

	pageBorders.setLeft(20);
	pageBorders.setTop(5);
	pageBorders.setRight(5);
	pageBorders.setBottom(5);

	graphBorders.setLeft(25);
	graphBorders.setTop(22);
	graphBorders.setRight(10);
	graphBorders.setBottom(20);

	gridStep.setHeight(5);
	gridStep.setWidth(5);

    m_scale = 3.5f;
	bdWidth	= 0.1f;

	setMouseTracking(true);    
	pPageSetup	= 0;
	m_pPanel	= 0;
}

GraphicsView::~GraphicsView()
{
	teardownGL();
}

void GraphicsView::teardownGL()
{
    // Actually destroy our OpenGL information
    glDeleteVertexArrays(1, &pageVAO);
    glDeleteBuffers(1, &pageVBO);
    delete m_program;
}

void GraphicsView::initializeGL()
{
	//Initialize OpenGL Backend
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

//    FT_Library  ft;
//    FT_Init_FreeType(&ft);

	// Set global information
//	gladLoadGL();
    initializeOpenGLFunctions();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glDisable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
		m_program->link();
		m_program->bind();

		// Cache Uniform Locations
		u_modelToWorld	= m_program->uniformLocation("modelToWorld");
		u_worldToCamera	= m_program->uniformLocation("worldToCamera");
		u_cameraToView	= m_program->uniformLocation("cameraToView");

		// Create Buffer (Do not release until VAO is created)
		glGenVertexArrays(1, &pageVAO);
		glBindVertexArray(pageVAO);
		glGenBuffers(1, &pageVBO);
		updatePageBuffer();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_program->release();
    }
}

struct Vertex
{
    vec2 pos;
    vec3 color;

    Vertex(vec2 p, vec3 c):pos(p),color(c){}
};
QColor  getColor(vec3 c)    {return QColor(c.r*255, c.g*255, c.b*255);}

void	GraphicsView::updatePageBuffer()
{
	std::vector<Vertex>	data;

	//Перекрестие мыши
	data.push_back(Vertex(vec2(m_mousePos.x, pageBorders.bottom()+graphBorders.bottom()), vec3(0.0f, 0.0f, 1.0f)));
	data.push_back(Vertex(vec2(m_mousePos.x, pageSize.height()-pageBorders.top()-graphBorders.top()), vec3(0.0f, 0.0f, 1.0f)));
	data.push_back(Vertex(vec2(pageBorders.left()+graphBorders.left(), m_mousePos.y), vec3(0.0f, 0.0f, 1.0f)));
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-graphBorders.right(), m_mousePos.y), vec3(0.0f, 0.0f, 1.0f)));

	//Белый лист
    data.push_back(Vertex(vec2(0., 0.),																		vec3(1.0f, 1.0f, 1.0f)));
    data.push_back(Vertex(vec2(pageSize.width(), 0.),														vec3(1.0f, 1.0f, 1.0f)));
    data.push_back(Vertex(vec2(0., pageSize.height()),														vec3(1.0f, 1.0f, 1.0f)));
    data.push_back(Vertex(vec2(pageSize.width(), pageSize.height()),										vec3(1.0f, 1.0f, 1.0f)));

	//Черная рамка
/*
	data.push_back(Vertex(vec2(pageBorders.left()-0.5f*bdWidth, pageBorders.bottom()-0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageBorders.left()+0.5f*bdWidth, pageBorders.bottom()+0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageBorders.left()-0.5f*bdWidth, pageSize.height()-pageBorders.top()+0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageBorders.left()+0.5f*bdWidth, pageSize.height()-pageBorders.top()-0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()+0.5f*bdWidth, pageSize.height()-pageBorders.top()+0.5f*bdWidth),	vec3(0.0f, 0.0f, 0.0f)));    
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-0.5f*bdWidth, pageSize.height()-pageBorders.top()-0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));	
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()+0.5f*bdWidth, pageBorders.bottom()-0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-0.5f*bdWidth, pageBorders.bottom()+0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));	
	data.push_back(Vertex(vec2(pageBorders.left()-0.5f*bdWidth, pageBorders.bottom()-0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageBorders.left()+0.5f*bdWidth, pageBorders.bottom()+0.5f*bdWidth), vec3(0.0f, 0.0f, 0.0f)));
*/

	data.push_back(Vertex(vec2(pageBorders.left(), pageBorders.bottom()),									vec3(0.0f, 0.0f, 0.0f)));
    data.push_back(Vertex(vec2(pageBorders.left(), pageSize.height()-pageBorders.top()),					vec3(0.0f, 0.0f, 0.0f)));    
	data.push_back(Vertex(vec2(pageBorders.left(), pageSize.height()-pageBorders.top()),					vec3(0.0f, 0.0f, 0.0f)));    
    data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageSize.height()-pageBorders.top()),	vec3(0.0f, 0.0f, 0.0f)));    
    data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageSize.height()-pageBorders.top()),	vec3(0.0f, 0.0f, 0.0f)));    
    data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageBorders.bottom()),					vec3(0.0f, 0.0f, 0.0f)));
	data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageBorders.bottom()),					vec3(0.0f, 0.0f, 0.0f)));	
	data.push_back(Vertex(vec2(pageBorders.left(), pageBorders.bottom()),									vec3(0.0f, 0.0f, 0.0f)));


	//Сетка
	nGridCount	= data.size();
	int n	= 0;
	for(float x0 = pageBorders.left()+graphBorders.left(); x0 <= (pageSize.width()-pageBorders.right()-graphBorders.right()); x0 += gridStep.width())
	{
		//Вертикальные линии
		if(!(n%5))
		{
            data.push_back(Vertex(vec2(x0, pageBorders.bottom()+graphBorders.bottom()),				0.85f*vec3(1.f)));
            data.push_back(Vertex(vec2(x0, pageSize.height()-pageBorders.top()-graphBorders.top()), 0.85f*vec3(1.f)));
		}
		else
		{
			data.push_back(Vertex(vec2(x0, pageBorders.bottom()+graphBorders.bottom()),				0.9f*vec3(1.f)));
			data.push_back(Vertex(vec2(x0, pageSize.height()-pageBorders.top()-graphBorders.top()), 0.9f*vec3(1.f)));
		}
		n++;
	}

	n	= 0;
	for(float y0 = pageBorders.bottom()+graphBorders.bottom(); y0 <= (pageSize.height()-pageBorders.top()-graphBorders.top()); y0 += gridStep.height())
	{
		//Горизонтальные линии
		if(!(n%5))
		{
            data.push_back(Vertex(vec2(pageBorders.left()+graphBorders.left(), y0),						0.85f*vec3(1.0f)));
            data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-graphBorders.right(), y0),	0.85f*vec3(1.0f)));
		}
		else
		{
            data.push_back(Vertex(vec2(pageBorders.left()+graphBorders.left(), y0),						0.9f*vec3(1.0f)));
            data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-graphBorders.right(), y0),	0.9f*vec3(1.0f)));
		}
		n++;
	}
	nGridCount	= data.size()-nGridCount;

	//Пересоздаем буфер
	glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Vertex), data.data(), GL_STATIC_DRAW);
}

void GraphicsView::resizeGL(int width, int height)
{
    vBar->setMinimum(0);
    vBar->setMaximum(max(0.f, pageSize.height()-height/m_scale));
    vBar->setPageStep(50);
    vBar->setSingleStep(1);

    hBar->setMinimum(0);
    hBar->setMaximum(max(0.f, pageSize.width()-width/m_scale));
    hBar->setPageStep(50);
    hBar->setSingleStep(1);

    if(vBar->maximum() == 0)    vBar->hide();
    else                        vBar->show();
    if(hBar->maximum() == 0)    hBar->hide();
    else                        hBar->show();

    m_proj	= glm::ortho<float>(0.f, width, -height, 0.f, 0.1f, 10000.0f);

//    GLfloat	aspect	= 1.25;
//    if(height)	aspect	= width/(GLfloat)height;
//    m_proj	= glm::perspective<float>(glm::radians(45.f), aspect, 0.1f, 1000.0f);
}

void GraphicsView::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT);

	// Render using our shader
	m_program->bind();
	glm::mat4	m_model(1.0f);
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &m_model[0][0]);
    glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
	{
		glBindVertexArray(pageVAO);

		//Два треугольника листа
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		//Рамка и сетка
		glDrawArrays(GL_LINES, 8, 8+nGridCount);
		
		//Перекрестие мыши
		if(m_bOnMouse)
			glDrawArrays(GL_LINES, 0, 4);

		glBindVertexArray(0);
	}
	m_program->release();

/*
    QPainter    painter(this);
    paintOverGL(&painter);
*/
}

void GraphicsView::paintOverGL(QPainter* p)
{
    p->translate(0, pageSize.height()*m_scale);
    p->scale(m_scale, -m_scale);

    //Получаем доступ к буферу
    glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
    GLint   bufSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufSize);
    char*   pBuf    = new char[bufSize];
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufSize, pBuf);

    //Читаем данные из буфера
    Vertex* pData   = (Vertex*)pBuf;
    int dataSize    = bufSize/(5*sizeof(float));

    //Два треугольника листа
    p->setPen(Qt::NoPen);
    p->setBrush(QBrush(getColor(pData[4].color)));
    p->drawRect(pData[4].pos.x, pData[4].pos.y, pData[7].pos.x-pData[4].pos.x, pData[7].pos.y-pData[4].pos.y);

	//Рамка
    p->setPen(QPen(getColor(pData[8].color), 0));
    p->setBrush(Qt::NoBrush);
    p->drawRect(pData[8].pos.x, pData[8].pos.y, pData[11].pos.x-pData[8].pos.x, pData[11].pos.y-pData[8].pos.y);

    //Сетка
    for(int i = 16; i < dataSize; i += 2)
    {
        p->setPen(QPen(getColor(pData[i].color), 0));
        p->drawLine(pData[i].pos.x, pData[i].pos.y, pData[i+1].pos.x, pData[i+1].pos.y);
    }

    //Перекрестие мыши
    if(m_bOnMouse)
        for(int i = 0; i < 4; i += 2)
        {
            p->setPen(QPen(getColor(pData[i].color), 0));
            p->drawLine(pData[i].pos.x, pData[i].pos.y, pData[i+1].pos.x, pData[i+1].pos.y);
        }

    delete[] pBuf;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GraphicsView::saveSVG()
{
    QSvgGenerator generator;
    generator.setFileName("c:\\Users\\zasovin\\Documents\\Visual Studio 2015\\Projects\\MFC_GLFW\\3b_CameraControl_07.10.2018\\my_svg.svg");
    generator.setSize(pageSize*m_scale);
//    generator.setViewBox(QRect(0, 0, pageSize.width(), pageSize.height()));
    generator.setViewBox(QRect(1, 1, pageSize.width()*m_scale, pageSize.height()*m_scale));
    generator.setTitle(tr("SVG Generator Example Drawing"));
    generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                       "Example provided with Qt."));

    QPainter painter;
    painter.begin(&generator);
    paintOverGL(&painter);
    painter.end();
}

void GraphicsView::setScale(float scale)
{
    if(scale != m_scale)
    {
        m_scale = scale;
        vBar->setMaximum(max(0.f, pageSize.height()-height()/m_scale));
        hBar->setMaximum(max(0.f, pageSize.width()-width()/m_scale));

        if(vBar->maximum() == 0)    vBar->hide();
        else                        vBar->show();
        if(hBar->maximum() == 0)    hBar->hide();
        else                        hBar->show();
    }
}

void GraphicsView::update()
{
	QTime	time	= QTime::currentTime();
    GLfloat	angle	= glm::radians(0.)*sin(0.3*time.msecsSinceStartOfDay()/1000.*6.28);
    GLfloat	anglex	= glm::radians(0.)*sin(0.2*time.msecsSinceStartOfDay()/1000.*6.28);
    GLfloat	angley	= glm::radians(0.)*sin(0.1*time.msecsSinceStartOfDay()/1000.*6.28);
    GLfloat	dist	= 400. + 0.*200.*sin(0.15*time.msecsSinceStartOfDay()/1000.*6.28);

    m_view  = glm::mat4(1.0f);
    m_view	= glm::scale(m_view, glm::vec3(m_scale,m_scale,1.f));
    m_view	= glm::translate(m_view, -glm::vec3(0.f, pageSize.height(), dist));
    m_view	= glm::translate(m_view, -glm::vec3(hBar->value(), -vBar->value(), 0.f));

    m_view  = translate(m_view, glm::vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));
    m_view  = rotate(m_view, angle, glm::vec3(0.f,0.f,1.0f));
    m_view  = translate(m_view, -glm::vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));

	// Schedule a redraw
	QOpenGLWidget::update();
}

void	GraphicsView::openPageSetup()
{
	if(!pPageSetup)
	{
		pPageSetup	= new PageSetup(this);
		pPageSetup->pageSize		= pageSize;
		pPageSetup->pageBorders		= pageBorders;
		pPageSetup->graphBorders	= graphBorders;
		pPageSetup->gridStep		= gridStep;
        pPageSetup->zoom            = m_scale;
		pPageSetup->load_data();
		connect(pPageSetup, &PageSetup::onApply, this, &GraphicsView::updatePage);
        connect(pPageSetup, &PageSetup::onZoomChanged, this, &GraphicsView::setScale);
	}
	
	pPageSetup->show();
}

void	GraphicsView::updatePage()
{
	if(pPageSetup && pPageSetup->isVisible())
	{
		pageSize		= pPageSetup->pageSize;
		pageBorders		= pPageSetup->pageBorders;
		graphBorders	= pPageSetup->graphBorders;
		gridStep		= pPageSetup->gridStep;

		updatePageBuffer();
	}
}

void	GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF	pLocal	= event->pos();

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width()*2.-1., 1.-pLocal.y()/height()*2.);
	glm::mat4	iView	= glm::inverse(m_proj*m_view);
	glm::vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);

	//Получаем мышь в поле графиков
	glm::mat4	graphM	= glm::translate(glm::mat4(1), glm::vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
	glm::vec4	graph	= glm::inverse(graphM)*world;

	//Сохраняем в классе
	m_mousePos.x	= world.x;
	m_mousePos.y	= world.y;

	//Переключаем курсор
	if(m_mousePos.x > pageBorders.left()+graphBorders.left() &&
	   m_mousePos.x < pageSize.width()-pageBorders.right()-graphBorders.right() &&
	   m_mousePos.y > pageBorders.bottom()+graphBorders.bottom() &&
	   m_mousePos.y < pageSize.height()-pageBorders.top()-graphBorders.top())
	{
		setCursor(Qt::BlankCursor);
		m_bOnMouse	= true;

		//Перекрестие мыши
		std::vector<Vertex>	data;
        data.push_back(Vertex(vec2(m_mousePos.x, pageBorders.bottom()+graphBorders.bottom()),					vec3(0.0f, 0.0f, 1.0f)));
        data.push_back(Vertex(vec2(m_mousePos.x, pageSize.height()-pageBorders.top()-graphBorders.top()),		vec3(0.0f, 0.0f, 1.0f)));
        data.push_back(Vertex(vec2(pageBorders.left()+graphBorders.left()*0, m_mousePos.y),						vec3(0.0f, 0.0f, 1.0f)));
        data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right()-graphBorders.right(), m_mousePos.y),	vec3(0.0f, 0.0f, 1.0f)));

		glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(Vertex), data.data());
	}
	else
	{
        setCursor(Qt::ArrowCursor);
		m_bOnMouse	= false;
	}
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	QPoint numDegrees = event->angleDelta();
	event->accept();
}

void	GraphicsView::on_panelChanged(vector<GAxe*>* axes)
{
    m_pPanel	= axes;
}
