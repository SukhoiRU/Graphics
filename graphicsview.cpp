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

//#define USE_FBO

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/
GraphicsView::GraphicsView(QWidget* parent, Qt::WindowFlags f) :QOpenGLWidget(parent, f)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);
    format.setSamples(1);
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
    pPageSetup	= nullptr;
    m_pPanel	= nullptr;

	Time0		= 200;
	TimeScale	= 20;
	curTime		= Time0;
	m_bTurning	= false;
	m_bPerspective	= false;

	axeArg		= new Graph::GAxeArg;
	oglInited	= false;
	m_mousePos	= vec2(0.f);
	m_clickPos	= vec2(0.f);

	fbo	= 0;
	qFBO	= nullptr;
	qFBO_unsamled	= nullptr;

	m_pLabel	= new Graph::GTextLabel;
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

	if(axeArg)	{delete	axeArg; axeArg = 0;}
	if(m_pLabel) {delete m_pLabel; m_pLabel = 0;}
	if(pPageSetup)	{delete pPageSetup; pPageSetup = 0;}
	teardownGL();
}

void GraphicsView::teardownGL()
{
    // Actually destroy our OpenGL information
	if(pageVBO)	{ glDeleteBuffers(1, &pageVBO); pageVBO = 0; }
	if(pageVAO)	{glDeleteVertexArrays(1, &pageVAO); pageVAO = 0;}
	if(m_program)	{delete m_program; m_program = 0;}

	if(fboVBO)	{glDeleteBuffers(1, &fboVBO); fboVBO = 0;}
	if(m_fbo_program) {delete m_fbo_program; m_fbo_program = 0;}
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
		glBindVertexArray(0);

		//Создаем программу для вывода из текстуры
		m_fbo_program = new QOpenGLShaderProgram();
		m_fbo_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/fbo.vert");
        m_fbo_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fbo.frag");
		m_fbo_program->link();

		//Создаем буфер для двух треугольников
		glGenBuffers(1, &fboVBO);
		glBindBuffer(GL_ARRAY_BUFFER, fboVBO);
		vector<vec4>	data;
		data.push_back(vec4(-1.f, +1.f, 0.f, 1.f));
		data.push_back(vec4(-1.f, -1.f, 0.f, 0.f));
		data.push_back(vec4(+1.f, +1.f, 1.f, 1.f));
		data.push_back(vec4(+1.f, -1.f, 1.f, 0.f));
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(vec4), data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Создаем поле графиков
	axeArg->initializeGL();
	m_GraphObjects.push_back(axeArg);

	m_pLabel->initializeGL();
	m_pLabel->setFont(10);
//	m_pLabel->addString("A", pageBorders.left()+graphBorders.left(), pageSize.height()-pageBorders.top()-graphBorders.top() - 5.*gridStep.height());
	m_pLabel->addString("AV/.Wpi$¡", 0, 0);
	m_pLabel->setFont(5.7);
	m_pLabel->addString("(И ещё «по-русски» можно)", 10, -10);
	m_pLabel->prepare();
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
	//Меняем матрицу проекции
	if(m_bPerspective)
	{
		GLfloat	aspect	= 1.25;
		if(height)	aspect	= width/(GLfloat)height;
		m_proj	= glm::perspective<float>(glm::radians(45.f), aspect, 0.1f, 10000.0f);
	}
	else
		m_proj	= glm::ortho<float>(0.f, width, -height, 0.f, 0.1f, 10000.0f);


/*
	//Создаем framebuffer
	if(fbo)	
	{
		//Очищаем имеюшийся буфер
		glDeleteTextures(1, fboTexture);
		glDeleteFramebuffers(1, &fbo);
	}

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	{
		//Текстурное прикрепление
		glGenTextures(2, fboTexture);
		glBindTexture(GL_TEXTURE_2D, fboTexture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture[0], 0);

		glBindTexture(GL_TEXTURE_2D, fboTexture[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboTexture[1], 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum	err	= glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(err != GL_FRAMEBUFFER_COMPLETE)
			qDebug() << "Framebuffer ERROR!";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//defaultFramebufferObject());
*/

#ifdef USE_FBO
	QOpenGLFramebufferObjectFormat	fmt;
	fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	fmt.setMipmap(true);
	fmt.setSamples(8);
	fmt.setTextureTarget(GL_TEXTURE_2D);
	fmt.setInternalTextureFormat(GL_RGBA32F_ARB);

	QOpenGLFramebufferObjectFormat	fmt2;
	fmt2.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	fmt2.setMipmap(true);
	fmt2.setTextureTarget(GL_TEXTURE_2D);
	fmt2.setInternalTextureFormat(GL_RGBA32F_ARB);

	if(qFBO)
	{
		delete qFBO;
		delete qFBO_unsamled;
	}
	qFBO	= new QOpenGLFramebufferObject(width, height, fmt);
	qFBO_unsamled	= new QOpenGLFramebufferObject(width, height, fmt2);
	if(!qFBO->isValid())
	{
		int a = 0;
	}

	drawScene();
#endif // USE_FBO

	//Меняем полосы прокрутки
    vBar->setMinimum(0);
    vBar->setMaximum(max(0.f, float(pageSize.height()-height/m_scale)));
    vBar->setPageStep(pageSize.height());
    vBar->setSingleStep(1);

    hBar->setMinimum(0);
    hBar->setMaximum(max(0.f, float(pageSize.width()-width/m_scale)));
    hBar->setPageStep(pageSize.width());
    hBar->setSingleStep(1);

    if(vBar->maximum() == 0)    vBar->hide();
    else                        vBar->show();
    if(hBar->maximum() == 0)    hBar->hide();
    else                        hBar->show();
}

void GraphicsView::paintGL()
{
	t0.start();

	//Очистка вида
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBindVertexArray(pageVAO);

#ifdef USE_FBO
	//Копирование картинки из буфера
	glBindFramebuffer(GL_READ_FRAMEBUFFER, qFBO_unsamled->handle());
	glBlitFramebuffer(0,0,width(),height(), 0,0,width(),height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
#else
	drawScene();
#endif // USE_FBO

	{
		mat4 dataModel	= mat4(1.f);
		dataModel	= translate(dataModel, vec3(pageBorders.left()+graphBorders.left(), pageSize.height()-pageBorders.top()-graphBorders.top() - 4.*gridStep.height(), 0.f));
		//m_pLabel->addString("A", , );

		m_pLabel->setMatrix(dataModel);
		m_pLabel->renderText(vec3(1., 0., 0.0f), 1.0f);
	}

	//Отрисовка мыши
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

		//glFinish();

		//Получаем мышь
		QPointF	pLocal	= mapFromGlobal(QCursor::pos());

		//Переводим мышь в координаты модели
		vec2	mouse(pLocal.x()/width()*2.-1., 1.-pLocal.y()/height()*2.);
		mat4	iView	= glm::inverse(m_proj*m_view);
		vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);
		mouse.x	= world.x;
		mouse.y	= world.y;

		//Растягиваем единичные палки мыши во всю область
		areaMat	= mat4(1.0f);
		areaMat	= translate(areaMat, vec3(mouse.x, mouse.y, 0));
		areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glDrawArrays(GL_LINES, 0, 4);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_program->release();
	}
	glBindVertexArray(0);
	//emit dt(t0.elapsed());
}

void GraphicsView::drawScene()
{
	if(!oglInited)	return;

#ifdef USE_FBO
	glViewport(0, 0, width(), height());
	qFBO->bind();

	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_LINE_SMOOTH);

	//Очистка вида
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#endif // USE_FBO

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
			if(m_SelectedObjects.size())
			{
				//Пропускаем выделенные
				for(size_t j = 0; j < m_SelectedObjects.size(); j++)
					if(m_SelectedObjects.at(j) == pGraph)
						continue;
				pGraph->Draw(Time0, TimeScale, gridStep, area, 0.3f);
			}
			else
				pGraph->Draw(Time0, TimeScale, gridStep, area, 1.0f);
		}

		//Дорисовываем выделенные
		for(size_t j = 0; j < m_SelectedObjects.size(); j++)
		{
			m_SelectedObjects.at(j)->Draw(Time0, TimeScale, gridStep, area, 1.0f);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	m_program->release();
#ifdef USE_FBO
	//Разсемплирование буфера
	qFBO->blitFramebuffer(qFBO_unsamled, qFBO, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	if(0)
	{
		//Сохранение картинок
		QImage	image	= qFBO->toImage();
		image.save("fbo.png", nullptr, 100);

		{
			QImage	image	= qFBO_unsamled->toImage();
			image.save("fbo_unsampled.png", nullptr, 100);
		}
	}

	qFBO->bindDefault();
#endif // USE_FBO
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
    GLfloat	dist	= 800. + 400.*sin(0.01*time.msecsSinceStartOfDay()/1000.*6.28);
	if(!m_bPerspective)	dist	= 400.;

    m_view  = mat4(1.0f);
    m_view	= scale(m_view, vec3(m_scale,m_scale,1.f));
    m_view	= translate(m_view, -vec3(0.f, pageSize.height(), dist));
    m_view	= translate(m_view, -vec3(hBar->value(), -vBar->value(), 0.f));

    m_view  = translate(m_view, vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));
	if(m_bTurning)
	{
		float angle	= 5.;
		GLfloat	anglez	= glm::radians(angle)*sin(0.1*time.msecsSinceStartOfDay()/1000.*6.28);
		GLfloat	anglex	= glm::radians(3.*angle)*sin(0.02*time.msecsSinceStartOfDay()/1000.*6.28);
		GLfloat	angley	= glm::radians(10.*angle)*sin(0.04*time.msecsSinceStartOfDay()/1000.*6.28);
		if(m_bPerspective)
		{
			m_view  = rotate(m_view, anglex, vec3(1.f, 0.f, 0.0f));
			m_view  = rotate(m_view, angley, vec3(0.f, 1.f, 0.0f));
		}
		m_view  = rotate(m_view, anglez, vec3(0.f, 0.f, 1.0f));
	}

	if(m_bPerspective)
		m_view  = translate(m_view, -vec3(pageSize.width(), -0*pageSize.height(), 0.f));
	else
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
		pStatus->showMessage(QString("dataVBO = %1, axeVBO = %2")
							 .arg(((Graph::GAxe*)pGraph)->dataVBO)
							.arg(((Graph::GAxe*)pGraph)->axeVBO), 10000);
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

#ifdef USE_FBO
	drawScene();
#endif // USE_FBO
	emit hasSelectedAxes(m_SelectedObjects.size() > 0);
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

#ifdef USE_FBO
	drawScene();
#endif // USE_FBO
	emit hasSelectedAxes(m_SelectedObjects.size() > 0);
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
	double time	= Time0	+ graph.x/gridStep.width()*TimeScale;
	if(time != curTime && m_bOnMouse)
	{
		curTime = time;
		emit timeChanged(curTime);
	}

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
#ifdef USE_FBO
			drawScene();
#endif // USE_FBO
			emit axesMoved();
		}
		else
		{
			//Выделенных объектов нет. Попробуем передвинуть время
			if(m_bOnMouse)
			{
				//Мышь в поле графиков
				vec2	delta	= mousePos - m_mousePos;
				Time0	-=	delta.x/gridStep.width()*TimeScale;
#ifdef USE_FBO
				drawScene();
#endif // USE_FBO
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
		Time0 += -numDegrees.x()/120.*TimeScale - numDegrees.y()/120.*TimeScale;
#ifdef USE_FBO
		drawScene();
#endif // USE_FBO
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
#ifdef USE_FBO
		drawScene();
#endif // USE_FBO
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
                            SelectObject(nullptr);
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
            SelectObject(nullptr);
		}
	}
	else if(buttons & Qt::RightButton)
	{
		onCustomMenuRequested(event->pos());
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
			}
            SelectObject(nullptr);
		}
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
    SelectObject(nullptr);

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

void	GraphicsView::on_panelDeleted(vector<Graph::GAxe *>* axes)
{
	m_pPanel	= nullptr;
}

void	GraphicsView::onCustomMenuRequested(QPoint pos)
{
	QMenu*		menu			= new QMenu(this);
	QAction*	actAngle		= new QAction("Качалка", this);
	QAction*	actPersp		= new QAction("Перспектива", this);

	actAngle->setCheckable(true);
	actAngle->setChecked(m_bTurning);
	actPersp->setCheckable(true);
	actPersp->setChecked(m_bPerspective);

	connect(actAngle, &QAction::toggled, [=](bool bCheck){m_bTurning = bCheck;});
	connect(actPersp, &QAction::toggled, [=](bool bCheck){m_bPerspective = bCheck; resizeGL(width(), height());});

	menu->addAction(actAngle);
	menu->addAction(actPersp);
	menu->popup(mapToGlobal(pos));
}
