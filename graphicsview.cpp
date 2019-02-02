#include "stdafx.h"
#include "graphicsview.h"
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTime>
#include <QScrollBar>
#include <QPainter>
#include <QSvgGenerator>
#include "Dialogs/pageSetup.h"
#include "Graph/GraphObject.h"
#include "Graph/GAxe.h"
#include "Graph/GAxeArg.h"

#include <vector>
using std::max;
using namespace Graph;

#include "Graph/GTextLabel.h"

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/
GraphicsView::GraphicsView(QWidget* parent, Qt::WindowFlags f) :QOpenGLWidget(parent, f)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 3);
    format.setSamples(16);
	format.setOption(QSurfaceFormat::DebugContext);
    setFormat(format);

	pageSize.setWidth(450);
	pageSize.setHeight(297);

	pageBorders.setLeft(20);
	pageBorders.setTop(5);
	pageBorders.setRight(5);
	pageBorders.setBottom(5);

	graphBorders.setLeft(45);
	graphBorders.setTop(10);
	graphBorders.setRight(10);
	graphBorders.setBottom(12);

	gridStep.setHeight(5);
	gridStep.setWidth(5);

    m_scale = 4.0f;

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	pageSize	= settings.value("GraphicsView/pageSize", pageSize).toSize();
	pageBorders	= settings.value("GraphicsView/pageBorders", pageBorders).toRectF();
	graphBorders	= settings.value("GraphicsView/graphBorders", graphBorders).toRectF();
	gridStep	= settings.value("GraphicsView/gridStep", gridStep).toSize();
	m_scale		= settings.value("GraphicsView/m_scale", m_scale).toFloat();

	bdWidth	= 0.1f;

	setMouseTracking(true);    
	pPageSetup	= 0;
	m_pPanel	= 0;

	Time0		= 200;
	TimeScale	= 20;
	curTime		= Time0;

	axeArg		= new Graph::GAxeArg;
	oglInited	= false;
	m_mousePos	= vec2(0.f);
	m_clickPos	= vec2(0.f);
}

GraphicsView::~GraphicsView()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.setValue("GraphicsView/pageSize", pageSize);
	settings.setValue("GraphicsView/pageBorders", pageBorders);
	settings.setValue("GraphicsView/graphBorders", graphBorders);
	settings.setValue("GraphicsView/gridStep", gridStep);
	settings.setValue("GraphicsView/m_scale", m_scale);
	settings.sync();

	delete	axeArg;
	if(pPageSetup)
		delete pPageSetup;
	teardownGL();

	QOpenGLContext*	c	= this->context();
}

void GraphicsView::teardownGL()
{
    // Actually destroy our OpenGL information
	if(pageVAO)	{glDeleteVertexArrays(1, &pageVAO); pageVAO = 0;}
	if(pageVBO)	{glDeleteBuffers(1, &pageVBO); pageVBO = 0;}
    delete m_program;
}

void GraphicsView::pause(bool hold)
{
    if(hold)    disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    else        connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
}

void GraphicsView::initializeGL()
{
	//Initialize OpenGL Backend
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

	// Set global information
	gladLoadGL();
	oglInited	= true;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Application-specific initialization
	{
		//Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
		m_program->link();

		m_program->bind();
		u_modelToWorld	= m_program->uniformLocation("modelToWorld");
		u_worldToCamera	= m_program->uniformLocation("worldToCamera");
		u_cameraToView	= m_program->uniformLocation("cameraToView");
		m_program->release();

		//Create Buffer (Do not release until VAO is created)
		glGenVertexArrays(1, &pageVAO);
		glBindVertexArray(pageVAO);
		glGenBuffers(1, &pageVBO);
		updatePageBuffer();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
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
	vec3	color(1.0f);

	//Перекрестие мыши
	color	= vec3(0.0f, 0.0f, 1.0f);
	data.push_back(Vertex(vec2(-1.f, 0.f), color));
	data.push_back(Vertex(vec2(1.f, 0.f), color));
	data.push_back(Vertex(vec2(0.f, -1.f), color));
	data.push_back(Vertex(vec2(0.f, 1.f), color));

	//Белый лист
    data.push_back(Vertex(vec2(0., 0.),																		vec3(1.0f)));
    data.push_back(Vertex(vec2(pageSize.width(), 0.),														vec3(1.0f)));
    data.push_back(Vertex(vec2(0., pageSize.height()),														vec3(1.0f)));
    data.push_back(Vertex(vec2(pageSize.width(), pageSize.height()),										vec3(1.0f)));

	//Черная рамка
	data.push_back(Vertex(vec2(pageBorders.left(), pageBorders.bottom()),									vec3(0.0f)));
    data.push_back(Vertex(vec2(pageBorders.left(), pageSize.height()-pageBorders.top()),					vec3(0.0f)));    
    data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageSize.height()-pageBorders.top()),	vec3(0.0f)));    
    data.push_back(Vertex(vec2(pageSize.width()-pageBorders.right(), pageBorders.bottom()),					vec3(0.0f)));

	//Область графиков
	color	= vec3(1.0f);
	data.push_back(Vertex(vec2(1.f, 0.f), color));
	data.push_back(Vertex(vec2(0.f, 0.f), color));
	data.push_back(Vertex(vec2(1.f, 1.f), color));
	data.push_back(Vertex(vec2(0.f, 1.f), color));

	//Пересоздаем буфер
	glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Vertex), data.data(), GL_STATIC_DRAW);
}

void GraphicsView::resizeGL(int width, int height)
{
    vBar->setMinimum(0);
    vBar->setMaximum(max(0.f, float(pageSize.height()-height/m_scale)));
    vBar->setPageStep(50);
    vBar->setSingleStep(1);

    hBar->setMinimum(0);
    hBar->setMaximum(max(0.f, float(pageSize.width()-width/m_scale)));
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
	t0.start();
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_LINE_SMOOTH);

	//Очистка вида
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Устанавливаем матрицы для объектов
	Graph::GraphObject::m_proj	= m_proj;
	Graph::GraphObject::m_view	= m_view;
	Graph::GraphObject::m_scale	= m_scale;

	QRectF	area;
	area.moveBottomLeft(pageBorders.bottomLeft() + graphBorders.bottomLeft());
	area.setWidth(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right());
	area.setHeight(pageSize.height() - pageBorders.top() - pageBorders.bottom() - graphBorders.top() - graphBorders.bottom());

	//Заливаем матрицы в шейдер
	m_program->bind();

	//Модельная матрица без сдвигов
	mat4	m_model(1.0f);
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &m_model[0][0]);
    glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
	{
		glBindVertexArray(pageVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);

		//Два треугольника листа
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		//Рамка
		glDrawArrays(GL_LINE_LOOP, 8, 4);
	
		//Рисуем список графических объектов
		for(size_t i = 0; i < m_GraphObjects.size(); i++)
		{
			Graph::GraphObject*	pGraph	= m_GraphObjects.at(i);
			float	alpha	= 1.0f;
			if(m_SelectedObjects.size())
			{
				//Невыделенные объекты рисуем бледненько
				alpha	= 0.3;

				//Ищем выделенные
				for(size_t j = 0; j < m_SelectedObjects.size(); j++)
					if(m_SelectedObjects.at(j) == pGraph)
						alpha	= 1.0f;
			}
			pGraph->Draw(Time0, TimeScale, gridStep, area, alpha);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Рисуем мышь
		if(m_bOnMouse)
		{
			m_program->bind();
			//Заливаем матрицы в шейдер
			glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
			glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

			//Растягиваем прямоугольник на всю область
			QRectF	area;
			area.moveBottomLeft(pageBorders.bottomLeft() + QPointF(0, graphBorders.bottom()));
			area.setWidth(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.right());
			area.setHeight(pageSize.height() - pageBorders.top() - pageBorders.bottom() - graphBorders.top() - graphBorders.bottom());

			mat4	areaMat(1.0f);
			areaMat	= translate(areaMat, vec3(area.x(), area.y(), 0));
			areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
			glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

			//Трафарет для оси
			glBindVertexArray(pageVAO);
			glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
			glEnableVertexAttribArray(1);

			glStencilMask(0xFF);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glFinish();
			//Получаем мышь
			QPointF	pLocal	= mapFromGlobal(QCursor::pos());

			//Переводим мышь в координаты модели
			vec2	mouse(pLocal.x()/width()*2.-1., 1.-pLocal.y()/height()*2.);
			mat4	iView	= glm::inverse(m_proj*m_view);
			vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);

			//Получаем мышь в поле графиков
			glm::mat4	graphM	= glm::translate(mat4(1.0f), vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
			glm::vec4	graph	= glm::inverse(graphM)*world;
			curTime	= Time0	+ graph.x/gridStep.width()*TimeScale;

/*
			//Сохраняем в классе
			m_mousePos.x	= world.x;
			m_mousePos.y	= world.y;

			//Переключаем курсор
			if(m_mousePos.x > pageBorders.left()+graphBorders.left() &&
			   m_mousePos.x < pageSize.width()-pageBorders.right()-graphBorders.right() &&
			   m_mousePos.y > pageBorders.bottom()+graphBorders.bottom() &&
			   m_mousePos.y < pageSize.height()-pageBorders.top()-graphBorders.top())
			{
				m_bOnMouse	= true;
			}
			else
			{
				m_bOnMouse	= false;
			}
*/

			//Растягиваем единичные палки мыши во всю область
			areaMat	= mat4(1.0f);
			areaMat	= translate(areaMat, vec3(m_mousePos.x, m_mousePos.y, 0));
			areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
			glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
			glStencilFunc(GL_EQUAL, 1, 0xFF);			
			glDrawArrays(GL_LINES, 0, 4);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	m_program->release();
	//emit dt(t0.elapsed());
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
/*  QSvgGenerator generator;
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
    painter.end();*/
}

void GraphicsView::setScale(float scale)
{
    if(scale != m_scale)
    {
        m_scale = scale;
		Graph::GraphObject::m_scale	= m_scale;

        vBar->setMaximum(max(0.f, float(pageSize.height()-height()/m_scale)));
        hBar->setMaximum(max(0.f, float(pageSize.width()-width()/m_scale)));

        if(vBar->maximum() == 0)    vBar->hide();
        else                        vBar->show();
        if(hBar->maximum() == 0)    hBar->hide();
        else                        hBar->show();
    }
}

void GraphicsView::update()
{
	QTime	time	= QTime::currentTime();
	GLfloat	angle	= glm::radians(0.)*sin(0.1*time.msecsSinceStartOfDay()/1000.*6.28);
	GLfloat	anglex	= glm::radians(0.)*sin(0.2*time.msecsSinceStartOfDay()/1000.*6.28);
	GLfloat	angley	= glm::radians(0.)*sin(0.1*time.msecsSinceStartOfDay()/1000.*6.28);
    GLfloat	dist	= 400. + 0.*200.*sin(0.15*time.msecsSinceStartOfDay()/1000.*6.28);
//	Time0	= 500 + 500*sin(0.1f*time.msecsSinceStartOfDay()/1000.*6.28);
//	TimeScale	= 50;
	//QPoint	cur	= QCursor::pos();
	//cur.setX(cur.x() + 2.0f*sin(0.15*time.msecsSinceStartOfDay()/1000.*6.28));
	//QCursor::setPos(cur);

    m_view  = mat4(1.0f);
    m_view	= scale(m_view, vec3(m_scale,m_scale,1.f));
    m_view	= translate(m_view, -vec3(0.f, pageSize.height(), dist));
    m_view	= translate(m_view, -vec3(hBar->value(), -vBar->value(), 0.f));

    m_view  = translate(m_view, vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));
    m_view  = rotate(m_view, angle, vec3(0.f,0.f,1.0f));
    m_view  = translate(m_view, -vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));

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

vec2	GraphicsView::mouseToDoc(QMouseEvent *event)
{
	QPointF	pLocal	= event->pos();

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width()*2.-1., 1.-pLocal.y()/height()*2.);
	glm::mat4	iView	= glm::inverse(m_proj*m_view);
	glm::vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);

	return vec2(world.x, world.y);
}

void	GraphicsView::SelectObject(Graph::GraphObject* pGraph)
{
	if(pGraph)
	{
		//Добавляем объект в список выделенных
		pGraph->m_IsSelected	= true;
		m_SelectedObjects.push_back(pGraph);
	}
	else
	{
		//Очищаем список
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			Graph::GraphObject*	pG = m_SelectedObjects.at(i);
			pG->m_IsSelected	= false;
			pG->m_IsMoving		= false;
		}
		m_SelectedObjects.clear();
	}
}

void	GraphicsView::UnSelectObject(Graph::GraphObject* pGraph)
{
	//Убираем объект из списка
	if(pGraph)
	{
		//Ищем его
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			Graph::GraphObject*	pG = m_SelectedObjects.at(i);
			if(pG == pGraph)
			{
				//Снимаем выделение
				pG->m_IsSelected	= false;
				pG->m_IsMoving		= false;

				m_SelectedObjects.erase(m_SelectedObjects.begin()+i);
				break;
			}
		}
	}
}

void	GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF	pLocal	= event->pos();
	Qt::MouseButtons		buttons	= event->buttons();
	Qt::KeyboardModifiers	mdf		= event->modifiers();

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width()*2.-1., 1.-pLocal.y()/height()*2.);
	glm::mat4	iView	= glm::inverse(m_proj*m_view);
	glm::vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);

	//Получаем мышь в поле графиков
	glm::mat4	graphM	= glm::translate(mat4(1.0f), vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
	glm::vec4	graph	= glm::inverse(graphM)*world;
	curTime	= Time0	+ graph.x/gridStep.width()*TimeScale;

	vec2	mousePos(world.x, world.y);

	//Переключаем курсор
	if(world.x > pageBorders.left()+graphBorders.left() &&
	   world.x < pageSize.width()-pageBorders.right()-graphBorders.right() &&
	   world.y > pageBorders.bottom()+graphBorders.bottom() &&
	   world.y < pageSize.height()-pageBorders.top()-graphBorders.top())
	{
		setCursor(Qt::BlankCursor);
		m_bOnMouse	= true;
	}
	else
	{
        setCursor(Qt::ArrowCursor);
		m_bOnMouse	= false;
	}

	if(buttons & Qt::LeftButton)
	{
		//Если есть выделенный объект, переместим его
		if(m_SelectedObjects.size())
		{
			if(!(*m_SelectedObjects.begin())->m_IsMoving)
			{
				//Начало перемещения
				for(size_t i = 0; i < m_SelectedObjects.size(); i++)
				{
					GraphObject*	pGraph	= m_SelectedObjects.at(i);
					pGraph->m_IsMoving	= true;
					pGraph->OnStartMoving();
				}
			}

			//Перемещение
			vec2	delta	= mousePos - m_mousePos;
			for(size_t i = 0; i < m_SelectedObjects.size(); i++)
			{
				GraphObject*	pGraph	= m_SelectedObjects.at(i);
				pGraph->MoveOffset(delta, buttons, mdf);
			}
		}
		else
		{
			//Выделенных объектов нет. Попробуем передвинуть время
			if(m_bOnMouse)
			{
				//Мышь в поле графиков
				vec2	delta	= mousePos - m_mousePos;
				Time0	-=	delta.x/gridStep.width()*TimeScale;
			}
		}
	}

	//Сохраняем в классе положение мыши
	m_mousePos	= mousePos;

	event->accept();
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	QPoint numDegrees			= event->angleDelta();
	Qt::KeyboardModifiers	mdf	= event->modifiers();

	if(mdf.testFlag(Qt::NoModifier))
	{
		Time0 += -0.01*numDegrees.x()*TimeScale - 0.01*numDegrees.y()*TimeScale;
	}
	else if(mdf.testFlag(Qt::ControlModifier))
	{
		//Нормализуем масштаб
		double	Power	= floor(log10(TimeScale));
		double	Mantiss	= TimeScale / pow(10., Power);
		double	dLen	= (curTime - Time0)/TimeScale;

		//Изменяем масштаб в нужную сторону
		if(numDegrees.y() > 0)
		{
			if(Mantiss == 1)		TimeScale	= 0.5*pow(10., Power);
			else if(Mantiss <= 2)	TimeScale	= 1*pow(10., Power);
			else if(Mantiss <= 5)	TimeScale	= 2*pow(10., Power);
			else					TimeScale	= 5*pow(10., Power);
		}
		else
		{
			if(Mantiss == 1)		TimeScale	= 2*pow(10., Power);
			else if(Mantiss <= 2)	TimeScale	= 5*pow(10., Power);
			else if(Mantiss <= 5)	TimeScale	= 10*pow(10., Power);
			else					TimeScale	= 20*pow(10., Power);
		}

		//Двигаем ноль так, чтобы попасть в то же время
		Time0	= curTime - dLen*TimeScale;
	}

	event->accept();
}

void	GraphicsView::mousePressEvent(QMouseEvent *event)
{
	Qt::MouseButtons		buttons		= event->buttons();
	Qt::KeyboardModifiers	modifiers	= event->modifiers();
	if(buttons & Qt::LeftButton)
	{
		//Запомним эту точку
		m_clickPos	= mouseToDoc(event);

		//Определим объект, в который произошел клик
		bool	bFound	= false;
		for(size_t i = (m_GraphObjects.size()-1); i > 0; i--)
		{
			Graph::GraphObject*	pGraph	= m_GraphObjects.at(i);
			if(pGraph->HitTest(m_clickPos))
			{
				if(!m_SelectedObjects.size())
				{
					//Выделений не было, однозначно добавляем
					SelectObject(pGraph);
				}
				else
				{
					if(modifiers & Qt::ControlModifier)
					{
						//При нажатом Ctrl имеющийся объект удаляем или добавляем
						if(pGraph->m_IsSelected)	UnSelectObject(pGraph);
						else						SelectObject(pGraph);
					}
					else
					{
						if(!pGraph->m_IsSelected)
						{
							//Новый объект оставляем единственным
							SelectObject(0);
							SelectObject(pGraph);
						}
					}
				}
				bFound = true;
				break;
			}
		}

		//Если ни в один объект не попали, выполняем действия по клику в окно
		if(!bFound)
		{
			SelectObject(NULL);
		}
	}

	event->accept();
}

void	GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	Qt::MouseButtons		buttons		= event->buttons();
	Qt::KeyboardModifiers	modifiers	= event->modifiers();

	//Если есть выделенные объекты, закончим перемещение
	if(m_SelectedObjects.size())
	{
		if((*m_SelectedObjects.begin())->m_IsMoving)
		{
			//Конец перемещения
			for(size_t i = 0; i < m_SelectedObjects.size(); i++)
			{
				GraphObject*	pGraph	= m_SelectedObjects.at(i);
				pGraph->m_IsMoving	= false;
				pGraph->OnStopMoving();
			}

			//SelectObject(0);
		}
	}

	event->accept();
}

void	GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
	return QOpenGLWidget::mouseDoubleClickEvent(event);
}

void	GraphicsView::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Escape:
	{
		//Если есть выделенные объекты, сбросим их
		if(m_SelectedObjects.size()) SelectObject(0);
		event->accept();
	}break;

	default:
		break;
	}

	return QOpenGLWidget::keyPressEvent(event);
}

void	GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
	return QOpenGLWidget::keyReleaseEvent(event);
}

void	GraphicsView::on_panelChanged(vector<Graph::GAxe*>* axes, std::vector<Accumulation*>* pBuffer)
{
	if(m_pPanel)
	{
		//Очищаем текущий список осей
		for(size_t i = 0; i < m_pPanel->size(); i++)
		{
			Graph::GAxe*	pAxe	= m_pPanel->at(i);
			pAxe->clearGL();
		}
	}

	//Сбрасываем выделение
	SelectObject(0);

	//Запоминаем новый список осей
    m_pPanel	= axes;
	if(!oglInited)	return;

	//Перестраиваем список графических объектов
	m_GraphObjects.clear();
	axeArg->initializeGL();
	m_GraphObjects.push_back(axeArg);
	for(size_t i = 0; i < axes->size(); i++)
	{
		Graph::GAxe*	pAxe	= axes->at(i);
		pAxe->initializeGL();
		pAxe->UpdateRecord(pBuffer);
		m_GraphObjects.push_back(pAxe);
	}
}

