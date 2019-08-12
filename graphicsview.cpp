#include "stdafx.h"
#include "graphicsview.h"
#include "ui_graphicsdoc.h"
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTime>
#include <QScrollBar>
#include <QDomDocument>
#include <QPrintDialog>
#include "Dialogs/pageSetup.h"
#include "Graph/GraphObject.h"
#include "Graph/GAxe.h"
#include "Graph/GAxeArg.h"
#include "Dialogs/gaxe_dialog.h"
#include "Dialogs/graphSettings.h"

#include <vector>
using std::max;
using namespace Graph;

extern Q_GUI_EXPORT QImage qt_gl_read_framebuffer(const QSize &size, bool alpha_format, bool include_alpha);

#include "Graph/GTextLabel.h"

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/
GraphicsView::GraphicsView()
{
	setSurfaceType(QWindow::OpenGLSurface);

	QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 3);
    format.setSamples(1);
	format.setSwapInterval(0);
    setFormat(format);

	m_context = new QOpenGLContext(this);
	m_context->setFormat(requestedFormat());
	if(!m_context->create()) 
	{
		delete m_context;
		m_context = nullptr;
	}

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

	gridStep	= vec2(5.);

    m_scale = 4.0f;

	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GraphicsView");
	pageSize		= settings.value("pageSize", pageSize).toSize();
	pageBorders		= settings.value("pageBorders", pageBorders).toRectF();
	graphBorders	= settings.value("graphBorders", graphBorders).toRectF();
	QSizeF	gS		= settings.value("gridStep", QSizeF(5., 5.)).toSizeF();
	gridStep		= vec2(gS.width(), gS.height());
	m_scale			= settings.value("m_scale", m_scale).toFloat();
	settings.endGroup();

	bdWidth	= 0.1f;

    pPageSetup	= nullptr;
    m_pPanel	= nullptr;
	m_bZoomMode	= false;

	Time0		= 200;
	TimeScale	= 20;
	curTime		= Time0;
	m_bTurning	= false;
	m_bPerspective	= false;

	bDrawLeftTime	= false;
	bDrawRightTime	= false;
	timeLeft		= 0;
	timeRight		= 0;

	axeArg		= new Graph::GAxeArg;
	oglInited	= false;
	m_mousePos	= vec2(0.f);
	m_shift		= vec2(0.f);

	m_pLabel	= new Graph::GTextLabel;

	m_pGraphSettings	= nullptr;

	modelTime	= 0;
	timeStep	= 0;
}

GraphicsView::~GraphicsView()
{
	QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
	settings.beginGroup("GraphicsView");
	settings.setValue("pageSize", pageSize);
	settings.setValue("pageBorders", pageBorders);
	settings.setValue("graphBorders", graphBorders);
	settings.setValue("gridStep", QSizeF(gridStep.x, gridStep.y));
	settings.setValue("m_scale", m_scale);
	settings.endGroup();
	settings.sync();

	if(axeArg)	{delete	axeArg; axeArg = 0;}
	if(m_pLabel) {delete m_pLabel; m_pLabel = 0;}
	if(pPageSetup)	{delete pPageSetup; pPageSetup = 0;}
	teardownGL();
}

void	GraphicsView::setUI(Ui::GraphicsDoc* pUI)
{
	ui	= pUI;
	connect(ui->actionPageInfo, &QAction::triggered, this, &GraphicsView::openPageSetup);
	connect(ui->actionGraphSettings, &QAction::triggered, this, &GraphicsView::on_graphSettings);

	connect(ui->actionScaleUp, &QAction::triggered, [this]
	{
		//Нормализуем масштаб
		double	Power	= floor(log10(TimeScale));
		double	Mantiss	= TimeScale / pow(10., Power);
		double	dLen	= (curTime - Time0)/TimeScale;

		//Изменяем масштаб
		if(Mantiss == 1)		TimeScale	= 0.5*pow(10., Power);
		else if(Mantiss <= 2)	TimeScale	= 1*pow(10., Power);
		else if(Mantiss <= 5)	TimeScale	= 2*pow(10., Power);
		else					TimeScale	= 5*pow(10., Power);

		//Двигаем ноль так, чтобы попасть в то же время
		Time0	= curTime - dLen*TimeScale;	
	});

	connect(ui->actionScaleDown, &QAction::triggered, [this]
	{
		//Нормализуем масштаб
		double	Power	= floor(log10(TimeScale));
		double	Mantiss	= TimeScale / pow(10., Power);
		double	dLen	= (curTime - Time0)/TimeScale;

		//Изменяем масштаб
		if(Mantiss == 1)		TimeScale	= 2*pow(10., Power);
		else if(Mantiss <= 2)	TimeScale	= 5*pow(10., Power);
		else if(Mantiss <= 5)	TimeScale	= 10*pow(10., Power);
		else					TimeScale	= 20*pow(10., Power);

		//Двигаем ноль так, чтобы попасть в то же время
		Time0	= curTime - dLen*TimeScale;
	});

	//Обработка скролла
	connect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);
	connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);
	
	connect(ui->actionFitTime, &QAction::triggered, this, &GraphicsView::fitTime);
	connect(ui->actionFitPage, &QAction::triggered, this, &GraphicsView::fitPage);
	connect(ui->actionDelAxe, &QAction::triggered, this, &GraphicsView::on_deleteAxes);
	connect(ui->actionZoom, &QAction::triggered, this, &GraphicsView::onZoomMode);
	connect(ui->action_SavePNG, &QAction::triggered, this, &GraphicsView::saveSVG);
	connect(ui->actionPrint, &QAction::triggered, this, &GraphicsView::print);
}

void GraphicsView::teardownGL()
{
    // Actually destroy our OpenGL information
	if(pageVBO)			{glDeleteBuffers(1, &pageVBO); pageVBO = 0;}
	if(pageVAO)			{glDeleteVertexArrays(1, &pageVAO); pageVAO = 0;}
	if(m_program)		{delete m_program; m_program = 0;}
	if(m_fbo_program)	{delete m_fbo_program; m_fbo_program= 0;}

	if(fboGraphArea)	
	{
		glDeleteTextures(1, &fboGraphAreaTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fboGraphArea); 
		fboGraphArea = 0;
	}

	if(fboGraph)
	{
		glDeleteTextures(1, &fboGraphTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fboGraph);
		fboGraph = 0;
	}
}

void	GraphicsView::saveAxeArg(QXmlStreamWriter& xml)
{
	xml.writeTextElement("Начало", QString::number(Time0));
	xml.writeTextElement("Шаг", QString::number(TimeScale));
	xml.writeTextElement("Положение", QString::number(axeArg->m_y));
}

void	GraphicsView::loadAxeArg(QDomElement* e, double ver)
{
	QDomElement	sub;
	sub	= e->firstChildElement("Начало");
	if(sub.isElement())
		Time0		= sub.text().toDouble();
	sub	= e->firstChildElement("Шаг");			if(sub.isElement())	TimeScale	= sub.text().toDouble();
	sub	= e->firstChildElement("Положение");	if(sub.isElement())	axeArg->m_y	= sub.text().toDouble();
	
	if(ver < 2.0) axeArg->m_y += 270;
}

void GraphicsView::initializeGL()
{
	//Initialize OpenGL Backend
	gladLoadGL();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClearStencil(0);

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
	}

	//Создаем поле графиков
	axeArg->initializeGL();
	m_GraphObjects.push_back(axeArg);

	m_pLabel->initializeGL();
	m_pLabel->setFont(10);
//	m_pLabel->addString("A", pageBorders.left()+graphBorders.left(), pageSize.height()-pageBorders.top()-graphBorders.top() - 5.*gridStep.height());
	m_pLabel->addString("AV/.Wpi$¡", 0, 0);
	m_pLabel->setFont(5.7f);
	m_pLabel->addString("(И ещё «по-русски» можно)", 10, -10);
	m_pLabel->prepare();

	oglInited	= true;

	//Выставляем размеры окна
	resizeGL(width, height);

	//Восстанавливаем загруженную панель
	vector<Graph::GAxe*>*	pPanel	= m_pPanel;
	m_pPanel	= nullptr;
	fromInit	= true;
	on_panelChanged(pPanel);
	fromInit	= false;
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
	data.push_back(Vertex(vec2(0.f, 0.f), color));
	data.push_back(Vertex(vec2(1.f, 0.f), color));
	data.push_back(Vertex(vec2(0.f, 0.f), color));
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

	//Маркер времени
	color	= vec3(185.f/256.f, 185.f/256.f, 0.0f);
	data.push_back(Vertex(vec2(0.f, 0.f), color));
	data.push_back(Vertex(vec2(0.f, 1.f), color));
	
	color	= vec3(185.f/256.f, 128.f/256.f, 0.0f);
	data.push_back(Vertex(vec2(0.f, 0.f), color));
	data.push_back(Vertex(vec2(0.f, 1.f), color));

	//Два треугольника для fbo
	data.push_back(Vertex(vec2(+1.f, -1.f), vec3(1.f, 0.f, 0.0f)));
	data.push_back(Vertex(vec2(-1.f, -1.f), vec3(0.f, 0.f, 0.0f)));
	data.push_back(Vertex(vec2(+1.f, +1.f), vec3(1.f, 1.f, 0.0f)));
	data.push_back(Vertex(vec2(-1.f, +1.f), vec3(0.f, 1.f, 0.0f)));

	//И еще раз для текстур графиков
	data.push_back(Vertex(vec2(+1.f, -1.f), vec3(1.f, 0.f, 0.0f)));
	data.push_back(Vertex(vec2(-1.f, -1.f), vec3(0.f, 0.f, 0.0f)));
	data.push_back(Vertex(vec2(+1.f, +1.f), vec3(1.f, 1.f, 0.0f)));
	data.push_back(Vertex(vec2(-1.f, +1.f), vec3(0.f, 1.f, 0.0f)));

	//Пересоздаем буфер
	glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Vertex), data.data(), GL_STATIC_DRAW);
}

void	GraphicsView::resizeEvent(QResizeEvent *e)
{
	QSize	sz	= e->size();
	resizeGL(sz.width(), sz.height());
	update();
}

void GraphicsView::resizeGL(int Width, int Height)
{
	width	= Width;
	height	= Height;

	//Меняем матрицу проекции
	if(m_bPerspective)
	{
		GLfloat	aspect	= 1.25;
		if(height)	aspect	= width/(GLfloat)height;
		m_proj	= glm::perspective<float>(glm::radians(45.f), aspect, 0.1f, 10000.0f);
	}
	else
		m_proj	= glm::ortho<float>(0.f, width, -height, 0.f, 0.1f, 10000.0f);

	if(oglInited)
	{
		//////////////////////////////////////////////////////////////////////////
		//Создаем framebuffer для области графиков
		if(fboGraphArea)
		{
			//Очищаем имеюшийся буфер
			glDeleteTextures(1, &fboGraphAreaTexture);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &fboGraphArea);
			fboGraphArea		= 0;
			fboGraphAreaValid	= false;
		}

		glGenFramebuffers(1, &fboGraphArea);
		glBindFramebuffer(GL_FRAMEBUFFER, fboGraphArea);
		{
			//Текстурное прикрепление
			glGenTextures(1, &fboGraphAreaTexture);
			glBindTexture(GL_TEXTURE_2D, fboGraphAreaTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboGraphAreaTexture, 0);

			GLenum	err	= glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(err != GL_FRAMEBUFFER_COMPLETE)
				qDebug() << "Framebuffer ERROR!";
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//////////////////////////////////////////////////////////////////////////
		//Создаем framebuffer для области графиков
		if(fboGraph)
		{
			//Очищаем имеюшийся буфер
			glDeleteTextures(1, &fboGraphTexture);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &fboGraph);
			fboGraph		= 0;
		}

		glGenFramebuffers(1, &fboGraph);
		glBindFramebuffer(GL_FRAMEBUFFER, fboGraph);
		{
			//Текстурное прикрепление
			glGenTextures(1, &fboGraphTexture);
			glBindTexture(GL_TEXTURE_2D, fboGraphTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboGraphTexture, 0);

			GLenum	err	= glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(err != GL_FRAMEBUFFER_COMPLETE)
				qDebug() << "Framebuffer ERROR!";
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//////////////////////////////////////////////////////////////////////////
	//Меняем полосы прокрутки
    ui->verticalScrollBar->setMinimum(0);
    ui->verticalScrollBar->setMaximum(max(0, int(pageSize.height()*m_scale-height)));
    ui->verticalScrollBar->setPageStep(pageSize.height());
    ui->verticalScrollBar->setSingleStep(1);

    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(max(0, int(pageSize.width()*m_scale-width)));
    ui->horizontalScrollBar->setPageStep(pageSize.width());
    ui->horizontalScrollBar->setSingleStep(1);

    if(ui->verticalScrollBar->maximum() == 0)	ui->verticalScrollBar->hide();
    else										ui->verticalScrollBar->show();
    if(ui->horizontalScrollBar->maximum() == 0)	ui->horizontalScrollBar->hide();
    else										ui->horizontalScrollBar->show();
}

bool	GraphicsView::event(QEvent* event)
{
	switch(event->type())
	{
		case QEvent::UpdateRequest:
		{
			paintGL();
			return true;
		}
		default:
			return QWindow::event(event);
	}
}

void	GraphicsView::exposeEvent(QExposeEvent* /*event*/)
{
	if(isExposed())	update();
}

void	GraphicsView::paintGL()
{
	if(!m_context->makeCurrent(this)) return;
	if(!isExposed())	return;
	if(!oglInited)	initializeGL();

	//Реальное время
	timer.start();

	//Устанавливаем матрицы для объектов
	Graph::GraphObject::m_proj	= m_proj;
	Graph::GraphObject::m_view	= m_view;
	Graph::GraphObject::m_scale	= m_scale;

	QRectF	area;
	area.moveBottomLeft(pageBorders.bottomLeft() + graphBorders.bottomLeft());
	area.setWidth(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right());
	area.setHeight(pageSize.height() - pageBorders.top() - pageBorders.bottom() - graphBorders.top() - graphBorders.bottom());
	vec2	areaBL(area.x(), area.y());
	vec2	areaSize(area.width(), area.height());

	//Биндим VAO
	glBindVertexArray(pageVAO);

	//При необходимости обновляем текстуру поля графиков
//	if(!fboGraphAreaValid)
	{
		drawGraphArea(areaBL, areaSize);
	}

	//Очистка вида
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//Биндим буфер
	glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);

	//Отрисовка листа
	m_program->bind();
	{
		//Модельная матрица без сдвигов
		mat4	m_model(1.0f);
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &m_model[0][0]);
		glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		//Два треугольника листа
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		//Рамка
		glDrawArrays(GL_LINE_LOOP, 8, 4);
	}
	m_program->release();

	//Отрисовка поля графиков из текстуры
	m_fbo_program->bind();
	{
		glEnable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboGraphAreaTexture);

		//Обновляем координаты поля графиков в NDC
		std::vector<Vertex>	data;
		mat4	mvp	= m_proj*m_view;
		vec4	BL	= mvp*vec4(areaBL.x, areaBL.y, 0.0f, 1.0f);
		vec4	BR	= mvp*vec4(areaBL.x + areaSize.x, areaBL.y, 0.0f, 1.0f);
		vec4	TR	= mvp*vec4(areaBL.x + areaSize.x, areaBL.y + areaSize.y, 0.0f, 1.0f);
		vec4	TL	= mvp*vec4(areaBL.x, areaBL.y + areaSize.y, 0.0f, 1.0f);

		data.push_back(Vertex(vec2(BR.x, BR.y), vec3(0.5f*(BR.x+1.0f), 0.5f*(BR.y+1.0f), 0.0f)));
		data.push_back(Vertex(vec2(BL.x, BL.y), vec3(0.5f*(BL.x+1.0f), 0.5f*(BL.y+1.0f), 0.0f)));
		data.push_back(Vertex(vec2(TR.x, TR.y), vec3(0.5f*(TR.x+1.0f), 0.5f*(TR.y+1.0f), 0.0f)));
		data.push_back(Vertex(vec2(TL.x, TL.y), vec3(0.5f*(TL.x+1.0f), 0.5f*(TL.y+1.0f), 0.0f)));

		//data.push_back(Vertex(vec2(+1.f, -1.f), vec3(1.f, 0.f, 0.0f)));
		//data.push_back(Vertex(vec2(-1.f, -1.f), vec3(0.f, 0.f, 0.0f)));
		//data.push_back(Vertex(vec2(+1.f, +1.f), vec3(1.f, 1.f, 0.0f)));
		//data.push_back(Vertex(vec2(-1.f, +1.f), vec3(0.f, 1.f, 0.0f)));

		glBufferSubData(GL_ARRAY_BUFFER, 20*sizeof(Vertex), 4*sizeof(Vertex), data.data());
		glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
//		glEnable(GL_BLEND);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_fbo_program->release();

	//Оси графиков
	for(size_t i = 0; i < m_pPanel->size(); i++)
	{
		Graph::GAxe*	pAxe	= m_pPanel->at(i);
		if(m_SelectedObjects.size())
		{
			//Пропускаем выделенные
			for(size_t j = 0; j < m_SelectedObjects.size(); j++)
				if(m_SelectedObjects.at(j) == pAxe)
					continue;
			pAxe->drawFrame(Time0, TimeScale, gridStep, areaBL, areaSize, 0.3f);
		}
		else
			pAxe->drawFrame(Time0, TimeScale, gridStep, areaBL, areaSize, 1.0f);
	}

	//Дорисовываем выделенные
	for(size_t j = 0; j < m_SelectedObjects.size(); j++)
	{
		Graph::GraphObject*	pGraph	= m_SelectedObjects.at(j);
		if(pGraph->m_Type == AXE)
			pGraph->drawFrame(Time0, TimeScale, gridStep, areaBL, areaSize, 1.0f);
	}

	//Надпись
	if(0)
	{
		mat4 dataModel	= mat4(1.f);
		dataModel	= translate(dataModel, vec3(pageBorders.left()+graphBorders.left(), pageSize.height()-pageBorders.top()-graphBorders.top() - 4.*gridStep.y, 0.f));
		//m_pLabel->addString("A", , );

		m_pLabel->setMatrix(dataModel);
		m_pLabel->renderText(vec3(1., 0., 0.0f), 1.0f);
	}

	//Отрисовка мыши
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

		//Растягиваем единичные палки мыши во всю область
		areaMat	= mat4(1.0f);
		areaMat	= translate(areaMat, vec3(pageBorders.left(), m_mousePos.y, 0.0f));
		areaMat	= scale(areaMat, vec3(area.width(), 1.0f, 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
		if(m_bOnMouse && !m_bZoomMode)
			glDrawArrays(GL_LINES, 0, 2);

		areaMat	= mat4(1.0f);
		areaMat	= translate(areaMat, vec3(m_mousePos.x, pageBorders.bottom() + graphBorders.bottom(), 0));
		areaMat	= scale(areaMat, vec3(1.0f, area.height(), 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
		if(m_bOnMouse && !m_bZoomMode)
			glDrawArrays(GL_LINES, 2, 2);

		//Рисуем левый и правый маркер
		if(bDrawLeftTime && (timeLeft > Time0) && (timeLeft < Time0 + (area.width() - graphBorders.left())/gridStep.x*TimeScale))
		{
			areaMat	= mat4(1.0f);
			areaMat	= translate(areaMat, vec3(pageBorders.left() + graphBorders.left() + (timeLeft-Time0)/TimeScale*gridStep.x, pageBorders.bottom() + graphBorders.bottom(), 0));
			areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
			glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
			glDrawArrays(GL_LINES, 16, 2);
		}

		if(bDrawRightTime && (timeRight > Time0) && (timeRight < Time0 + (area.width() - graphBorders.left())/gridStep.x*TimeScale))
		{
			areaMat	= mat4(1.0f);
			areaMat	= translate(areaMat, vec3(pageBorders.left() + graphBorders.left() + (timeRight-Time0)/TimeScale*gridStep.x, pageBorders.bottom() + graphBorders.bottom(), 0));
			areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
			glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);
			glDrawArrays(GL_LINES, 18, 2);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_program->release();
	}
	glBindVertexArray(0);
//	emit dt(timer.elapsed());

	//Переключаем буферы
	m_context->swapBuffers(this);
}

void	GraphicsView::drawGraphArea(const vec2& areaBL, const vec2& areaSize)
{
	//Перерисовка поля графиков
	glBindFramebuffer(GL_FRAMEBUFFER, fboGraphArea);
	glViewport(0, 0, width, height);

	//Заливаем белым
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Рисуем разлиновку
	axeArg->draw(Time0, TimeScale, gridStep, areaBL, areaSize, 1.0f);

	//Рисуем графики
	for(size_t i = 0; i < m_pPanel->size(); i++)
	{
		Graph::GAxe*	pAxe	= m_pPanel->at(i);
		if(m_SelectedObjects.size())
		{
			//Пропускаем выделенные
			for(size_t j = 0; j < m_SelectedObjects.size(); j++)
				if(m_SelectedObjects.at(j) == pAxe)
					continue;
			
			//Рисуем в отдельную текстуру
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraph);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBlendFunc(GL_ONE, GL_ZERO);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

			pAxe->draw(Time0, TimeScale, gridStep, areaBL, areaSize, 0.3f);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			//QImage img = qt_gl_read_framebuffer(size() * devicePixelRatio(), false, false);
			//img.setDevicePixelRatio(devicePixelRatio());
			//img.save("c:\\0.png");

			//Переносим ее в основную
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraphArea);
			m_fbo_program->bind();
			glEnable(GL_BLEND);
			glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
			glEnableVertexAttribArray(1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboGraphTexture);
			glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_fbo_program->release();
		}
		else
		{
			//Рисуем в отдельную текстуру
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraph);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBlendFunc(GL_ONE, GL_ZERO);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

			pAxe->draw(Time0, TimeScale, gridStep, areaBL, areaSize, 1.0f);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			//QImage img = qt_gl_read_framebuffer(size() * devicePixelRatio(), false, false);
			//img.setDevicePixelRatio(devicePixelRatio());
			//img.save("c:\\0.png");
			
			//Переносим ее в основную
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraphArea);
			m_fbo_program->bind();
			glEnable(GL_BLEND);
			glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
			glEnableVertexAttribArray(1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboGraphTexture);
			glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_fbo_program->release();
		}
	}

	//Дорисовываем выделенные
	for(size_t j = 0; j < m_SelectedObjects.size(); j++)
	{
		Graph::GraphObject*	pGraph	= m_SelectedObjects.at(j);
		if(pGraph->m_Type == AXE)
		{
			//Рисуем в отдельную текстуру
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraph);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBlendFunc(GL_ONE, GL_ZERO);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

			pGraph->draw(Time0, TimeScale, gridStep, areaBL, areaSize, 1.0f);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			//QImage img = qt_gl_read_framebuffer(size() * devicePixelRatio(), false, false);
			//img.setDevicePixelRatio(devicePixelRatio());
			//img.save("c:\\0.png");

			//Переносим ее в основную
			glBindFramebuffer(GL_FRAMEBUFFER, fboGraphArea);
			m_fbo_program->bind();
			glEnable(GL_BLEND);
			glBindBuffer(GL_ARRAY_BUFFER, pageVBO);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
			glEnableVertexAttribArray(1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboGraphTexture);
			glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_fbo_program->release();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	fboGraphAreaValid	= true;
}

void GraphicsView::saveSVG()
{
	QString	filename	= QFileDialog::getSaveFileName(nullptr, "Сохранение графика", "", "*.png");
	m_context->makeCurrent(this);
	QImage img = qt_gl_read_framebuffer(size() * devicePixelRatio(), false, false);
	img.setDevicePixelRatio(devicePixelRatio());
	img.save(filename);

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

void	GraphicsView::print()
{
	//Запоминаем настройки экрана
	float	oldScale	= m_scale;
	float	oldWidth	= width;
	float	oldHeight	= height;
	vec2	oldShift	= m_shift;
	int		oldHbar		= ui->horizontalScrollBar->value();
	int		oldVbar		= ui->verticalScrollBar->value();

	QPrinter	printer(QPrinter::PrinterMode::PrinterResolution);
	QPrintDialog	printDialog(&printer, nullptr);
	if(printDialog.exec() == QDialog::Accepted)
	{
		printer.setResolution(300);
		printer.setPageOrientation(QPageLayout::Landscape);

		QPainter	painter(&printer);
//		QRect		rect(0, 0, 297*600/25.4, 210*600/25.4);//painter.viewport();
		QRect		rect	= painter.viewport();

		QOpenGLFramebufferObject	fbo(rect.size(), QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
		bool	res	= fbo.isValid();
		resizeGL(rect.width(), rect.height());

		//Подстраиваем лист под окно
		ui->horizontalScrollBar->setValue(0);
		ui->horizontalScrollBar->hide();
		ui->verticalScrollBar->setValue(0);
		ui->verticalScrollBar->hide();
		QSize	sz	= rect.size();
		int	w	= sz.width();
		int	h	= sz.height();
		float	scaleW	= w/pageSize.width();
		float	scaleH	= h/pageSize.height();
		m_shift	= vec2(0.f);
		setScale(std::min(scaleW, scaleH));

		fbo.bind();
		update();
		QImage	im	= fbo.toImage();
		fbo.release();

		QString	filename	= QFileDialog::getSaveFileName(nullptr, "Сохранение графика", "", "*.png");
		im.save(filename, nullptr, 100);

		painter.drawImage(0, 0, im);
		
		//Восстанавливаем настройки
		m_scale		= oldScale;
		width		= oldWidth;
		height		= oldHeight;
		m_shift		= oldShift;
		ui->horizontalScrollBar->setValue(oldHbar);
		ui->verticalScrollBar->setValue(oldVbar);
		resizeGL(width, height);
		setScale(m_scale);
		update();
	}

}

void GraphicsView::setScale(float scale)
{
    if(scale != m_scale)
    {
		ui->verticalScrollBar->setMinimum(0);
        ui->verticalScrollBar->setMaximum(max(0, int(pageSize.height()*scale-height)));
		ui->horizontalScrollBar->setMinimum(0);
		ui->horizontalScrollBar->setMaximum(max(0, int(pageSize.width()*scale-width)));

        if(ui->verticalScrollBar->maximum() == 0)	ui->verticalScrollBar->hide();
        else										ui->verticalScrollBar->show();
        if(ui->horizontalScrollBar->maximum() == 0) ui->horizontalScrollBar->hide();
        else										ui->horizontalScrollBar->show();

		m_scale = scale;
		Graph::GraphObject::m_scale	= m_scale;
		fboGraphAreaValid	= false;
    }
	update();
}

void GraphicsView::update()
{
    GLfloat	dist	= 800. + 400.*sin(0.01*modelTime*6.28);
	if(!m_bPerspective)	dist	= 400.;

    m_view  = mat4(1.0f);
    m_view	= scale(m_view, vec3(m_scale,m_scale,1.f));
    m_view	= translate(m_view, -vec3(0.f, pageSize.height(), dist));
    m_view	= translate(m_view, -vec3(ui->horizontalScrollBar->value()/m_scale, -ui->verticalScrollBar->value()/m_scale, 0.f));
	m_view	= translate(m_view, -vec3(m_shift.x/m_scale, -m_shift.y/m_scale, 0.f));

    m_view  = translate(m_view, vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));
	if(m_bTurning)
	{
		float angle	= 500.;
		GLfloat	anglez	= glm::radians(angle)*sin(0.1*modelTime*6.28);
		GLfloat	anglex	= glm::radians(3.*angle)*sin(0.02*modelTime*6.28);
		GLfloat	angley	= glm::radians(10.*angle)*sin(0.04*modelTime*6.28);
		if(m_bPerspective)
		{
			m_view  = rotate(m_view, anglex, vec3(1.f, 0.f, 0.0f));
			m_view  = rotate(m_view, angley, vec3(0.f, 1.f, 0.0f));
		}
//		m_view  = rotate(m_view, anglez, vec3(0.f, 0.f, 1.0f));
		m_view	= translate(m_view, vec3(anglex, anglez, 0.f));
	}

	if(m_bPerspective)
		m_view  = translate(m_view, -vec3(pageSize.width(), -0*pageSize.height(), 0.f));
	else
		m_view  = translate(m_view, -vec3(0.5*pageSize.width(), 0.5*pageSize.height(), 0.f));

	// Schedule a redraw
	requestUpdate();
//	paintGL();
}

void	GraphicsView::openPageSetup()
{
	if(!pPageSetup)
	{
		pPageSetup	= new PageSetup(ui->splitter);
		pPageSetup->pageSize		= pageSize;
		pPageSetup->pageBorders		= pageBorders;
		pPageSetup->graphBorders	= graphBorders;
		pPageSetup->gridStep		= QSizeF(gridStep.x, gridStep.y);
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
		gridStep		= vec2(pPageSetup->gridStep.width(), pPageSetup->gridStep.height());

		updatePageBuffer();
	}
	update();
}

vec2	GraphicsView::mouseToDoc(QMouseEvent *event)
{
	QPointF	pLocal	= event->pos();

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width*2.-1., 1.-pLocal.y()/height*2.);
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
		//pStatus->showMessage(QString("dataVBO = %1, axeVBO = %2")
		//					 .arg(((Graph::GAxe*)pGraph)->dataVBO)
		//					.arg(((Graph::GAxe*)pGraph)->axeVBO), 10000);
	}
	else
	{
		//Очищаем список
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			Graph::GraphObject*	pG = m_SelectedObjects.at(i);
			pG->m_IsSelected	= false;
			pG->m_IsMoving		= false;

			//Отмена выделения шрифтом
			if(pG->m_Type == AXE)
			{
				GAxe*	pAxe	= (GAxe*)pG;
				pAxe->setAxeLength(pAxe->getAxeLength());
			}
		}
		m_SelectedObjects.clear();
	}

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

	emit hasSelectedAxes(m_SelectedObjects.size() > 0);
}

void	GraphicsView::on_axeAdded(Graph::GAxe* pAxe)
{
	SelectObject(NULL);
	SelectObject(pAxe);
}

void	GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF	pLocal	= event->pos();
	Qt::MouseButtons		buttons	= event->buttons();
	Qt::KeyboardModifiers	mdf		= event->modifiers();

	//Отдельные действия в режиме перетаскивания
	if(m_bZoomMode)
	{
		QPoint	mouse	= event->globalPos();
		if(buttons & Qt::LeftButton)
		{
			QPoint	delta	= mouse - m_oldMouse;
			m_shift -= vec2(delta.x(), delta.y());
			shiftToScroll();
			//ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - delta.x());
			//ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() - delta.y());
		}

		//Сохраняем в классе положение мыши
		m_oldMouse	= mouse;
		update();

		return;
	}

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width*2.-1., 1.-pLocal.y()/height*2.);
	glm::mat4	iView	= glm::inverse(m_proj*m_view);
	glm::vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);

	//Получаем мышь в поле графиков
	glm::mat4	graphM	= glm::translate(mat4(1.0f), vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
	glm::vec4	graph	= glm::inverse(graphM)*world;
	double time	= Time0	+ graph.x/gridStep.x*TimeScale;
	if(time != curTime && m_bOnMouse)
	{
		curTime = time;
		emit timeChanged(curTime);
	}

	vec2	mousePos(world.x, world.y);

	//Определим объект, на который попала мышь
	bool	bFound	= false;
	for(size_t i = m_GraphObjects.size(); i > 0; i--)
	{
		Graph::GraphObject*	pGraph	= m_GraphObjects.at(i-1);
		if(pGraph->hitTest(mousePos))
		{
			Qt::CursorShape	shape;
			if(pGraph->getCursor(mousePos, shape))
				setCursor(shape);
			bFound	= true;
			break;
		}
	}

	if(!bFound)
	{
		//Раз ни в один объект не попали, действия по окну
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
					pGraph->onStartMoving();
				}
			}

			//Перемещение
			vec2	delta	= mousePos - m_mousePos;
			for(size_t i = 0; i < m_SelectedObjects.size(); i++)
			{
				GraphObject*	pGraph	= m_SelectedObjects.at(i);
				pGraph->moveOffset(delta, buttons, mdf);
				if(pGraph->m_Type == AXEARG)
				{
					//Для оси дополнительно двигаем время
					if(m_bOnMouse)
					{
						Time0	-=	delta.x/gridStep.x*TimeScale;
					}
				}
			}
			fboGraphAreaValid	= false;
			emit axesMoved();
		}
		else
		{
			if(mdf & Qt::ShiftModifier)
			{
				//Таскаем лист
				vec2	delta	= mousePos - m_mousePos;
				ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() + delta.y*m_scale);
				ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - delta.x*m_scale);
				event->accept();
				update();
				return;
			}
			else
			{
				//Выделенных объектов нет. Попробуем передвинуть время
				if(m_bOnMouse)
				{
					//Мышь в поле графиков
					vec2	delta	= mousePos - m_mousePos;
					Time0	-=	delta.x/gridStep.x*TimeScale;
					fboGraphAreaValid	= false;
				}
				else
				{
					//Вертикальная прокрутка
					vec2	delta	= mousePos - m_mousePos;
					ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() + delta.y*m_scale);
					ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - delta.x*m_scale);
					event->accept();
					update();
					return;
				}
			}
		}
	}

	if(mdf & Qt::ControlModifier)
	{
		//При нажатом Ctrl и выделенных осях подсветим центры
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			GraphObject*	pGraph	= m_SelectedObjects.at(i);
			if(pGraph->m_Type == AXE)
			{
				GAxe*	pAxe	= (GAxe*)pGraph;
				int	index	= int((m_mousePos.y - pAxe->getPosition().y)/gridStep.y + 0.5);
				pAxe->setAxeLength(pAxe->getAxeLength(), index);
			}
		}
	}

	//Сохраняем в классе положение мыши
	m_mousePos	= mousePos;

	event->accept();
	update();
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	QPointF					pLocal		= event->pos();
	QPoint					numDegrees	= event->angleDelta();
	Qt::KeyboardModifiers	mdf			= event->modifiers();

	//Переводим мышь в координаты модели
	glm::vec2	mouse(pLocal.x()/width*2.-1., 1.-pLocal.y()/height*2.);
	glm::mat4	iView	= glm::inverse(m_proj*m_view);
	glm::vec4	world	= iView*glm::vec4(mouse, 0.f, 1.f);
	vec2	mousePos(world.x, world.y);

	//Отдельные действия в режиме перетаскивания
	if(m_bZoomMode)
	{
		if(mdf.testFlag(Qt::ControlModifier))
		{
			//Расстояние от мыши до верхнего левого угла в миллиметрах
			vec2	delta		= vec2(mousePos.x, pageSize.height() - mousePos.y);
			float	oldScale	= m_scale;

			if(numDegrees.y() > 0)	setScale(m_scale*(1.1*numDegrees.y()/120.f));
			else					setScale(m_scale/(-1.1*numDegrees.y()/120.f));

			//Сдвигаем прокрутку, чтобы точка осталась на месте
			m_shift	+= delta*(m_scale - oldScale);
			shiftToScroll();
			//ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() + delta.x*(m_scale - oldScale));
			//ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() + delta.y*(m_scale - oldScale));
		}
		else if(mdf.testFlag(Qt::ShiftModifier))
		{
			ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - numDegrees.y());
		}
		else
		{
			ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - numDegrees.x());
			ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() - numDegrees.y());
		}
		event->accept();
		update();
		return;
	}


	if(m_SelectedObjects.size())
	{
		//Есть выделенные объекты
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			Graph::GraphObject*	pGraph	= m_SelectedObjects.at(i);
			pGraph->onWheel(mousePos, mdf, vec2(numDegrees.x(), numDegrees.y()));
		}
	}
	else
	{
		//Определим объект, на который попала мышь
		bool	bFound	= false;
		for(size_t i = m_GraphObjects.size(); i > 0; i--)
		{
			Graph::GraphObject*	pGraph	= m_GraphObjects.at(i-1);
			if(pGraph->hitTest(mousePos))
			{
				//pGraph->onWheel(mousePos, mdf, vec2(numDegrees.x(), numDegrees.y()));
				bFound	= true;
				break;
			}
		}

		if(!bFound)
		{
			//Раз ни в один объект не попали, действия по окну
			if(mdf.testFlag(Qt::NoModifier))
			{
				if(m_bOnMouse)
				{
					//Двигаем время
					Time0 += -numDegrees.x()/120.*TimeScale - numDegrees.y()/120.*TimeScale;
					fboGraphAreaValid	= false;
				}
				else
				{
					//Двигаем лист
					ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - numDegrees.x());
					ui->verticalScrollBar->setValue(ui->verticalScrollBar->value() - numDegrees.y());
				}
			}
			else if(mdf.testFlag(Qt::ShiftModifier))
			{
				//Двигаем лист
				ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value() - numDegrees.y());
			}
			else if(mdf.testFlag(Qt::ControlModifier))
			{
				if(m_bOnMouse)
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
					fboGraphAreaValid	= false;
				}
				else
				{
					//Крутим масштаб листа
					vec2	delta		= vec2(mousePos.x, pageSize.height() - mousePos.y);
					float	oldScale	= m_scale;

					if(numDegrees.y() > 0)	setScale(m_scale*(1.1*numDegrees.y()/120.f));
					else					setScale(m_scale/(-1.1*numDegrees.y()/120.f));

					//Сдвигаем прокрутку, чтобы точка осталась на месте
					m_shift	+= delta*(m_scale - oldScale);
					fboGraphAreaValid	 = false;
					shiftToScroll();
				}
			}
		}
	}
	event->accept();
	update();
}

void	GraphicsView::mousePressEvent(QMouseEvent *event)
{
	Qt::MouseButtons		buttons		= event->buttons();
	Qt::KeyboardModifiers	modifiers	= event->modifiers();
	if(m_bZoomMode)
	{
		return;
	}

	if(buttons & Qt::LeftButton)
	{
		//Запомним эту точку
		vec2 clickPos	= mouseToDoc(event);

		//Определим объект, в который произошел клик
		bool	bFound	= false;
		for(size_t i = m_GraphObjects.size(); i > 0; i--)
		{
			Graph::GraphObject*	pGraph	= m_GraphObjects.at(i-1);
			if(pGraph->hitTest(clickPos))
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
		if(clickPos.x > pageBorders.left()+graphBorders.left() &&
		   clickPos.x < pageSize.width()-pageBorders.right()-graphBorders.right() &&
		   clickPos.y > pageBorders.bottom()+graphBorders.bottom() &&
		   clickPos.y < pageSize.height()-pageBorders.top()-graphBorders.top())
		{
			//Щелчок в поле графиков. Ставим левый маркер

			//Получаем мышь в поле графиков
			glm::mat4	graphM	= glm::translate(mat4(1.0f), vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
			glm::vec4	graph	= glm::inverse(graphM)*glm::vec4(clickPos, 0.f, 1.f);
			double time	= Time0	+ graph.x/gridStep.x*TimeScale;
			
			bDrawLeftTime	= true;
			if(time != timeLeft)
			{
				timeLeft		= time;
				emit timeLeftChanged(timeLeft, true);
			}
		}

		if(!bFound)
		{
            SelectObject(nullptr);
		}
	}
	else if(buttons & Qt::RightButton)
	{
		//Запомним эту точку
		vec2 clickPos	= mouseToDoc(event);
		if(clickPos.x > pageBorders.left()+graphBorders.left() &&
		   clickPos.x < pageSize.width()-pageBorders.right()-graphBorders.right() &&
		   clickPos.y > pageBorders.bottom()+graphBorders.bottom() &&
		   clickPos.y < pageSize.height()-pageBorders.top()-graphBorders.top())
		{
			//Щелчок в поле графиков. Ставим правый маркер

			//Получаем мышь в поле графиков
			glm::mat4	graphM	= glm::translate(mat4(1.0f), vec3(pageBorders.left()+graphBorders.left(), pageBorders.bottom()+graphBorders.bottom(), 0.f));
			glm::vec4	graph	= glm::inverse(graphM)*glm::vec4(clickPos, 0.f, 1.f);
			double time	= Time0	+ graph.x/gridStep.x*TimeScale;

			bDrawRightTime	= true;
			if(time != timeRight)
			{
				timeRight		= time;
				emit timeRightChanged(timeRight, true);
			}
		}
		else
			onCustomMenuRequested(event->pos());
	}

	event->accept();
	update();
}

void	GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
//	Qt::MouseButtons		buttons		= event->buttons();
//	Qt::KeyboardModifiers	modifiers	= event->modifiers();

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
				pGraph->onStopMoving();
			}

			//SelectObject(0);
		}
	}

	event->accept();
	update();
}

void	GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(m_SelectedObjects.size())
	{
		//При наличии списка объектов собираем только оси
		vector<GAxe*>	axes;
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			GraphObject*	pAxe	= m_SelectedObjects.at(i);
			if(pAxe->m_Type == AXE)
				axes.push_back((GAxe*)pAxe);
		}

		if(axes.size())
		{
			GAxe_dialog*	dlg	= new GAxe_dialog(&axes, ui->centralwidget);
			connect(dlg, &GAxe_dialog::change_axe, [=](GAxe* pAxe){emit change_axe(pAxe, dlg); });
			connect(dlg, &GAxe_dialog::substract_axe, [=](GAxe* pAxe){emit substract_axe(pAxe, dlg);});
			dlg->exec();
			emit axesRenamed();
			delete	dlg;
		}
	}
	else
		return QWindow::mouseDoubleClickEvent(event);
}

void	GraphicsView::on_deleteAxes()
{
	if(m_SelectedObjects.size())
	{
		//При наличии списка объектов собираем только оси
		vector<GAxe*>	axes;
		for(size_t i = 0; i < m_SelectedObjects.size(); i++)
		{
			GraphObject*	pAxe	= m_SelectedObjects.at(i);
			if(pAxe->m_Type == AXE)
				axes.push_back((GAxe*)pAxe);
		}

		if(axes.size())
		{
			emit delete_axe(&axes);
			setCursor(Qt::ArrowCursor);
			fboGraphAreaValid	 = false;
		}
	}
}

void	GraphicsView::keyPressEvent(QKeyEvent *event)
{
//	Qt::KeyboardModifiers	mdf		= event->modifiers();
	if(m_bZoomMode)	QWindow::keyPressEvent(event);

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
						pGraph->onStopMoving();
					}
				}
				SelectObject(nullptr);
			}

			//Отключим маркеры времени
			if(bDrawLeftTime)
			{
				bDrawLeftTime	= false;
				emit timeLeftChanged(timeLeft, false);
			}

			if(bDrawRightTime)
			{
				bDrawRightTime	= false;
				emit timeRightChanged(timeRight, false);
			}

			event->accept();
		}break;

		case Qt::Key_Control:
		{
			//При нажатом Ctrl и выделенных осях подсветим центры
			for(size_t i = 0; i < m_SelectedObjects.size(); i++)
			{
				GraphObject*	pGraph	= m_SelectedObjects.at(i);
				if(pGraph->m_Type == AXE)
				{
					GAxe*	pAxe	= (GAxe*)pGraph;
					int	index	= int((m_mousePos.y - pAxe->getPosition().y)/gridStep.y + 0.5);

					pAxe->setAxeLength(pAxe->getAxeLength(), index);
				}
			}
		}break;

		//Двигаем время
		//case Qt::Key_Left:		timeMoving	= -10.*TimeScale;	break;
		//case Qt::Key_Right:		timeMoving	= +10.*TimeScale;	break;
		//case Qt::Key_PageUp:	timeMoving	= -50.*TimeScale; break;//0.5*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale;	break;
		//case Qt::Key_PageDown:	timeMoving	= 50.*TimeScale; break;//0.5*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale;	break;
		case Qt::Key_Left:		{Time0	-= TimeScale; curTime -= TimeScale;}	break;
		case Qt::Key_Right:		{Time0	+= TimeScale; curTime += TimeScale;}	break;
		case Qt::Key_PageUp:	{Time0	-= 0.7*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale; curTime	-= 0.7*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale;}	break;
		case Qt::Key_PageDown:	{Time0	+= 0.7*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale;	curTime	+= 0.7*(pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x*TimeScale;}	break;
		
		default:
			break;
	}

	QWindow::keyPressEvent(event);
	update();
}

void	GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Control:
		{
			//При отпускании Ctrl восстановим надписи осей
			for(size_t i = 0; i < m_SelectedObjects.size(); i++)
			{
				GraphObject*	pGraph	= m_SelectedObjects.at(i);
				if(pGraph->m_Type == AXE)
				{
					GAxe*	pAxe	= (GAxe*)pGraph;
					pAxe->setAxeLength(pAxe->getAxeLength());
				}
			}
			event->accept();
		}break;
	
		default:
			break;
	}
	
	QWindow::keyReleaseEvent(event);
	update();
}

void	GraphicsView::on_panelChanged(vector<Graph::GAxe*>* axes)
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
		m_GraphObjects.push_back(pAxe);
	}

	//Сбрасываем выделение
	for(size_t i = 0; i < m_SelectedObjects.size(); i++)
	{
		Graph::GraphObject*	pG = m_SelectedObjects.at(i);
		pG->m_IsSelected	= false;
		pG->m_IsMoving		= false;
	}
	m_SelectedObjects.clear();
	modelTime	= 0;
	if(!fromInit)
		update();
}

void	GraphicsView::on_panelDeleted(vector<Graph::GAxe *>* /*axes*/)
{
	m_pPanel	= nullptr;
	update();
}

void	GraphicsView::onCustomMenuRequested(QPoint pos)
{
	QMenu*		menu			= new QMenu(ui->centralwidget);
	QAction*	actAngle		= new QAction("Качалка", this);
	QAction*	actPersp		= new QAction("Перспектива", this);

	actAngle->setCheckable(true);
	actAngle->setChecked(m_bTurning);
	actPersp->setCheckable(true);
	actPersp->setChecked(m_bPerspective);

	connect(actAngle, &QAction::toggled, [=](bool bCheck){m_bTurning = bCheck; modelTime = 0;});
	connect(actPersp, &QAction::toggled, [=](bool bCheck){m_bPerspective = bCheck; resizeGL(QWindow::width(), QWindow::height()); modelTime = 0;});

	menu->addAction(ui->actionScaleUp);
	menu->addAction(ui->actionScaleDown);
	menu->addSeparator();
	menu->addAction(actAngle);
	menu->addAction(actPersp);
	menu->popup(mapToGlobal(pos));
}

void	GraphicsView::on_graphSettings()
{
	if(!m_pGraphSettings)
		m_pGraphSettings	= new graphSettings(ui->centralwidget);
	m_pGraphSettings->show();
	connect(m_pGraphSettings, &QDialog::accepted, this, &GraphicsView::update);
}

void	GraphicsView::fitTime()
{
	//По звездочке выставляем полное время полета
	double	tMin	= 0;
	double	tMax	= 0;
	bool	bHasCorrect	= false;
	for(size_t i = 0; i < m_pPanel->size(); i++)
	{
		GAxe*	pAxe	= m_pPanel->at(i);

		//Получаем диапазон времени от оси
		double t0, t1;
		pAxe->getTime(t0, t1);
		if(t0 == 0 && t1 == 0)
		{
			//Время некорректно, пропускаем
			continue;
		}
		else if(!bHasCorrect)
		{
			//Первый раз нашли непустое время
			bHasCorrect	= true;
			tMin	= t0;
			tMax	= t1;
		}
		else
		{
			//Последующие непустые времена сравниваем
			if(t0 < tMin)	tMin	= t0;
			if(t1 > tMax)	tMax	= t1;
		}
	}

	if(!bHasCorrect)	return;

	//Подбираем под диапазон
	Time0	= tMin;
	int	nGrids	= (pageSize.width() - pageBorders.left() - pageBorders.right() - graphBorders.left() - graphBorders.right())/gridStep.x;
	TimeScale	= (tMax - tMin)/(nGrids);

	//Нормализуем масштаб
	double	Power	= floor(log10(TimeScale));
	double	Mantiss	= TimeScale / pow(10., Power);

	if(Mantiss == 1)		TimeScale	= 1*pow(10., Power);
	else if(Mantiss <= 2)	TimeScale	= 2*pow(10., Power);
	else if(Mantiss <= 5)	TimeScale	= 5*pow(10., Power);
	else					TimeScale	= 10*pow(10., Power);

	//Центруем время
	Time0 -= 0.5*(Time0 + nGrids*TimeScale - tMax);
	update();
}

void	GraphicsView::onZoomMode()
{
	//Переключение режимов работы мыши
	m_bZoomMode	= !m_bZoomMode;
	ui->actionZoom->setChecked(m_bZoomMode);
	if(m_bZoomMode)	setCursor(Qt::SizeAllCursor);
	else			setCursor(Qt::ArrowCursor);
}

void	GraphicsView::fitPage()
{
	//Подстраиваем лист под окно
	ui->horizontalScrollBar->setValue(0);
	ui->horizontalScrollBar->hide();
	ui->verticalScrollBar->setValue(0);
	ui->verticalScrollBar->hide();
	QSize	sz	= ui->splitter->widget(0)->size();
	int	w	= sz.width();
	int	h	= sz.height();
	float	scaleW	= w/pageSize.width();
	float	scaleH	= h/pageSize.height();
	m_shift	= vec2(0.f);
	setScale(std::min(scaleW, scaleH));
	update();
}

void	GraphicsView::shiftToScroll()
{			
	//Перекачка в scrollBar
	disconnect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);
	int	hValue	= ui->horizontalScrollBar->value();
	int	hMax	= ui->horizontalScrollBar->maximum();
	if(hValue + m_shift.x > 0)
	{
		if(hValue + m_shift.x <= hMax)
		{
			ui->horizontalScrollBar->setValue(hValue + m_shift.x);
			m_shift.x	= 0;
		}
		else
		{
			ui->horizontalScrollBar->setValue(hMax);
			m_shift.x	+= hValue - hMax;
		}
	}
	else
	{
		ui->horizontalScrollBar->setValue(0);
		m_shift.x	= hValue + m_shift.x;
	}
	connect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);

	disconnect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);
	int	vValue	= ui->verticalScrollBar->value();
	int	vMax	= ui->verticalScrollBar->maximum();
	if(vValue + m_shift.y > 0)
	{
		if(vValue + m_shift.y <= vMax)
		{
			ui->verticalScrollBar->setValue(vValue + m_shift.y);
			m_shift.y	= 0;
		}
		else
		{
			ui->verticalScrollBar->setValue(vMax);
			m_shift.y	+= vValue - vMax;
		}
	}
	else
	{
		ui->verticalScrollBar->setValue(0);
		m_shift.y	= vValue + m_shift.y;
	}
	connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GraphicsView::shiftToScroll);
	fboGraphAreaValid	= false;
	update();
}

