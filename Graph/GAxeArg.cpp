#include "stdafx.h"
#include "GAxeArg.h"
#include "GTextLabel.h"

namespace Graph{

GAxeArg::GAxeArg()
{
	m_Type		= AXEARG;
	oldTime		= 0;
	oldTimeScale	= 0;
	oldGrid		= QSize();
	oldArea		= QRect();
	nCountGrid	= 0;
	nCountAxe	= 0;

	m_program	= 0;
	gridVAO	= 0;
	gridVBO	= 0;
	axeVAO	= 0;
	axeVBO	= 0;
	textLabel	= new GTextLabel;
}

GAxeArg::~GAxeArg()
{
	if(gridVAO)	{glDeleteVertexArrays(1, &gridVAO); gridVAO = 0;}
	if(gridVBO)	{glDeleteBuffers(1, &gridVBO); gridVBO	= 0;}

	if(axeVAO)	{glDeleteVertexArrays(1, &axeVAO); axeVAO = 0;}
	if(axeVBO)	{glDeleteBuffers(1, &axeVBO); axeVBO = 0;}

	if(m_program) { delete m_program; m_program = 0;}
	delete textLabel;
}

void	GAxeArg::initializeGL()
{
	if(m_bOpenGL_inited)	return;
//	initializeOpenGLFunctions();
	m_bOpenGL_inited	= true;

	m_program	= new QOpenGLShaderProgram;
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxearg.vert");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxearg.frag");
	m_program->link();

	m_program->bind();
	u_modelToWorld	= m_program->uniformLocation("modelToWorld");
	u_worldToCamera	= m_program->uniformLocation("worldToCamera");
	u_cameraToView	= m_program->uniformLocation("cameraToView");
	m_program->release();

	textLabel->initializeGL();
}

struct Vertex
{
	vec2 pos;
	vec3 color;

	Vertex(vec2 p, vec3 c) :pos(p), color(c){}
};

void	GAxeArg::Draw(const double t0, const double TimeScale, const QSizeF& grid, const QRectF& area, const float alpha)
{
	//Рисуем шкалу и сетку
	if(!TimeScale)	return;

	//Расчет ближайшей круглой точки
	double	dt	= t0 - (int)(t0/TimeScale/5.)*5.*TimeScale;

	//Проверяем, надо ли обновлять данные
	if((t0 != oldTime) ||
		(TimeScale != oldTimeScale) ||
		(grid.width()	!= oldGrid.width()) ||
	   (area.width() != oldArea.width()))
	{
		//Запоминаем предыдущие значения
		oldTime			= t0;
		oldTimeScale	= TimeScale;
		oldGrid			= grid;
		oldArea			= area;
		textLabel->clearGL();
		textLabel->initializeGL();
		textLabel->setFont(10, vec3(0.f), m_scale);

		//Собираем новый буфер для сетки
		vector<Vertex>	dataGrid;
		vector<Vertex>	dataAxe;

		//Квадрат для трафарета
		vec3	color(1.0f,0,0);
		dataGrid.push_back(Vertex(vec2(1.f, 0.f), color));
		dataGrid.push_back(Vertex(vec2(0.f, 0.f), color));
		dataGrid.push_back(Vertex(vec2(1.f, 1.f), color));
		dataGrid.push_back(Vertex(vec2(0.f, 1.f), color));

		//Вертикальные штрихи
		int n	= 0;
		for(int x0 = - 5*grid.width(); x0 <= area.width() + 5*grid.width(); x0 += 1)
		{
			//Штрихи для оси
			dataAxe.push_back(Vertex(vec2(x0, 0.f), vec3(0.f)));
			dataAxe.push_back(Vertex(vec2(x0, -0.75f - 0.75f*((n%5)==0)), vec3(0.f)));

			if(!(n%5))
			{
				//Каждый пятый штрих оси рисуем вертикальную линию сетки
				vec3	color	= 0.85f*vec3(1.f);
				dataGrid.push_back(Vertex(vec2(x0, 0.f), color));
				dataGrid.push_back(Vertex(vec2(x0, area.height()), color));
			}

			//Метка времени
			float	t	= t0 + x0/grid.width()*TimeScale - dt;
			if(!(n%25) && t >= t0 && x0 <= area.width()+grid.width())
			{
				QString	timeStr	= QString("%1").arg(t);
				vec2	strSize	= textLabel->textSize(timeStr);
				textLabel->addString(timeStr, x0 - 0.5*strSize.x, -2. - strSize.y);
			}

			n++;
		}

		//Горизонтальная линия оси
		dataAxe.push_back(Vertex(vec2(-5*grid.width(), 0.f), vec3(0.f)));
		dataAxe.push_back(Vertex(vec2(area.width()+5*grid.width(), 0.f), vec3(0.f)));
		nCountAxe	= dataAxe.size();

		//Горизонтальные линии сетки
		for(float y0 = 0; y0 <= area.height() + grid.height(); y0 += grid.height())
		{
			vec3	color	= 0.85f*vec3(1.f);
			dataGrid.push_back(Vertex(vec2(-5*grid.width(), y0),	color));
			dataGrid.push_back(Vertex(vec2(area.width()+5*grid.width(), y0),	color));
		}

		//Жирные линии сетки
		color	= 0.7f*vec3(1.f);
		for(int x0 = 0; x0 <= area.width() + grid.width(); x0 += 25)
		{
			dataGrid.push_back(Vertex(vec2(x0, 0.f), color));
			dataGrid.push_back(Vertex(vec2(x0, area.height()), color));
		}
		for(float y0 = 0; y0 <= area.height() + grid.height(); y0 += 5.0f*grid.height())
		{
			dataGrid.push_back(Vertex(vec2(-5*grid.width(), y0), color));
			dataGrid.push_back(Vertex(vec2(area.width()+5*grid.width(), y0), color));
		}

		//Рамка зоны графиков
		nCountGrid	= dataGrid.size();
		color	= 0.85f*vec3(1.f);
		dataGrid.push_back(Vertex(vec2(0, 0.f), color));
		dataGrid.push_back(Vertex(vec2(0, area.height()), color));
		dataGrid.push_back(Vertex(vec2(area.width(), area.height()), color));
		dataGrid.push_back(Vertex(vec2(area.width(), 0), color));

		//Пересоздаем буфер
		if(gridVAO) {glDeleteVertexArrays(1, &gridVAO); gridVAO = 0;}
		if(gridVBO) {glDeleteBuffers(1, &gridVBO); gridVBO = 0;}

		glGenVertexArrays(1, &gridVAO);
		glBindVertexArray(gridVAO);
		glGenBuffers(1, &gridVBO);
		glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
		glBufferData(GL_ARRAY_BUFFER, dataGrid.size()*sizeof(Vertex), dataGrid.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Буфер для оси
		if(axeVAO) { glDeleteVertexArrays(1, &axeVAO); axeVAO = 0; }
		if(axeVBO) { glDeleteBuffers(1, &axeVBO); axeVBO = 0; }
		glGenVertexArrays(1, &axeVAO);
		glBindVertexArray(axeVAO);
		glGenBuffers(1, &axeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
		glBufferData(GL_ARRAY_BUFFER, dataAxe.size()*sizeof(Vertex), dataAxe.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		textLabel->prepare();
	}

	//Заливаем матрицы
	mat4	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x() - dt/TimeScale*grid.width(), area.y(),0.0f));
	dataModel	= scale(dataModel, vec3(grid.width()/5.0f, 1.0f, 0.f));

	m_program->bind();
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

	//Рисуем сетку
	glBindVertexArray(gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	{
		//Трафарет для сетки
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//Растягиваем прямоугольник на всю область
		mat4	areaMat(1.0f);
		areaMat	= translate(areaMat, vec3(area.x(), area.y(), 0));
		areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDrawArrays(GL_LINES, 4, nCountGrid-4);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//Рамка поля графиков
	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x(), area.y(), 0.0f));
	dataModel	= scale(dataModel, vec3(grid.width()/5.0f, 1.0f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glDrawArrays(GL_LINE_LOOP, nCountGrid, 4);
	
	//Рисуем ось
	{
		//Трафарет для оси
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//Растягиваем прямоугольник область оси
		mat4	areaMat(1.0f);
		areaMat	= translate(areaMat, vec3(area.x(), area.y()+0.5f, 0));
		areaMat	= scale(areaMat, vec3(area.width(), -2.5f, 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	glBindVertexArray(axeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);

	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x() - dt/TimeScale*grid.width(), area.y(),0.0f));
	dataModel	= scale(dataModel, vec3(grid.width()/5.0f, 1.0f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDrawArrays(GL_LINES, 0, nCountAxe);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_program->release();

	dataModel	= translate(mat4(1.f), vec3(area.x() - dt/TimeScale*grid.width(), area.y(), 0.f));
	textLabel->setMatrix(dataModel, m_view, m_proj);
	textLabel->renderText(1);
}

}