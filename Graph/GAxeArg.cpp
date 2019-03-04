#include "stdafx.h"
#include "GAxeArg.h"
#include "GTextLabel.h"

namespace Graph{

GAxeArg::GAxeArg()
{
	m_Type		= AXEARG;
	oldTime		= 0;
	oldTimeScale	= 0;
	oldGrid		= vec2(0.);
	oldAreaBL	= vec2(0.);
	oldAreaSize	= vec2(0.);
	nCountGrid	= 0;
	nCountAxe	= 0;

	m_program	= 0;
	gridVBO	= 0;
	axeVBO	= 0;
	textLabel	= new GTextLabel;
}

GAxeArg::~GAxeArg()
{
	if(gridVBO)	{glDeleteBuffers(1, &gridVBO); gridVBO	= 0;}
	if(axeVBO)	{glDeleteBuffers(1, &axeVBO); axeVBO = 0;}

	if(m_program) { delete m_program; m_program = 0;}
	if(textLabel) { delete textLabel; textLabel = 0;}
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

void	GAxeArg::Draw(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float /*alpha*/)
{
	//Рисуем шкалу и сетку
	if(!TimeScale)	return;

	//Расчет ближайшей круглой точки
	double	dt	= t0 - (int)(t0/TimeScale/5.)*5.*TimeScale;

	//Проверяем, надо ли обновлять данные
	if((t0 != oldTime) ||
		(TimeScale != oldTimeScale) ||
	   (oldScale != m_scale) || 
		(grid != oldGrid) ||
	   (areaBL != oldAreaBL) ||
	   (areaSize != oldAreaSize)
	   )
	{
		//Запоминаем предыдущие значения
		oldTime			= t0;
		oldTimeScale	= TimeScale;
		oldGrid			= grid;
		oldAreaBL		= areaBL;
		oldAreaSize		= areaSize;
		oldScale		= m_scale;
		textLabel->clearGL();
		textLabel->initializeGL();
		textLabel->setFont(3.8f);

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
		for(int x0 = - 5*grid.x; x0 <= areaSize.x + 5*grid.x; x0 += 1)
		{
			//Штрихи для оси
			dataAxe.push_back(Vertex(vec2(x0, 0.f), vec3(0.f)));
			dataAxe.push_back(Vertex(vec2(x0, -0.75f - 0.75f*((n%5)==0)), vec3(0.f)));

			if(!(n%5))
			{
				//Каждый пятый штрих оси рисуем вертикальную линию сетки
				vec3	color	= 0.85f*vec3(1.f);
				dataGrid.push_back(Vertex(vec2(x0, 0.f), color));
				dataGrid.push_back(Vertex(vec2(x0, areaSize.y), color));
			}

			//Метка времени
			float	t	= t0 + x0/grid.x*TimeScale - dt;
			if(!(n%25) && t >= t0 && x0 <= areaSize.x+grid.x)
			{
				QString	timeStr	= QString("%1").arg(t);
				vec2	strSize	= textLabel->textSize(timeStr);
				textLabel->addString(timeStr, x0 - 0.5*strSize.x, -1.5f - textLabel->topLine());
			}

			n++;
		}

		//Горизонтальная линия оси
		dataAxe.push_back(Vertex(vec2(-5*grid.x, 0.f), vec3(0.f)));
		dataAxe.push_back(Vertex(vec2(areaSize.x+5*grid.x, 0.f), vec3(0.f)));
		nCountAxe	= dataAxe.size();

		//Горизонтальные линии сетки
		for(float y0 = 0; y0 <= areaSize.y + grid.y; y0 += grid.y)
		{
			vec3	color	= 0.85f*vec3(1.f);
			dataGrid.push_back(Vertex(vec2(-5*grid.x, y0),	color));
			dataGrid.push_back(Vertex(vec2(areaSize.x+5*grid.x, y0),	color));
		}

		//Жирные линии сетки
		color	= 0.7f*vec3(1.f);
		for(int x0 = 0; x0 <= areaSize.x + grid.x; x0 += 25)
		{
			dataGrid.push_back(Vertex(vec2(x0, 0.f), color));
			dataGrid.push_back(Vertex(vec2(x0, areaSize.y), color));
		}
		for(float y0 = 0; y0 <= areaSize.y + grid.y; y0 += 5.0f*grid.y)
		{
			dataGrid.push_back(Vertex(vec2(-5*grid.x, y0), color));
			dataGrid.push_back(Vertex(vec2(areaSize.x+5*grid.x, y0), color));
		}

		//Рамка зоны графиков
		nCountGrid	= dataGrid.size();
		color	= 0.85f*vec3(1.f);
		dataGrid.push_back(Vertex(vec2(0, 0.f), color));
		dataGrid.push_back(Vertex(vec2(0, areaSize.y), color));
		dataGrid.push_back(Vertex(vec2(areaSize), color));
		dataGrid.push_back(Vertex(vec2(areaSize.x, 0), color));

		//Пересоздаем буфер
		if(gridVBO)
		{
			glDeleteBuffers(1, &gridVBO);
		}
		glGenBuffers(1, &gridVBO);
		glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
		glBufferData(GL_ARRAY_BUFFER, dataGrid.size()*sizeof(Vertex), dataGrid.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Буфер для оси
		if(axeVBO) 
		{
			glDeleteBuffers(1, &axeVBO);
		}
		glGenBuffers(1, &axeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
		glBufferData(GL_ARRAY_BUFFER, dataAxe.size()*sizeof(Vertex), dataAxe.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		textLabel->prepare();
	}

	//Заливаем матрицы
	mat4	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(areaBL.x - dt/TimeScale*grid.x, areaBL.y,0.0f));
	dataModel	= scale(dataModel, vec3(grid.x/5.0f, 1.0f, 0.f));

	m_program->bind();
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

	//Рисуем сетку
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
		areaMat	= translate(areaMat, vec3(areaBL, 0.));
		areaMat	= scale(areaMat, vec3(areaSize, 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilMask(0x00);
	}
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDrawArrays(GL_LINES, 4, nCountGrid-4);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//Рамка поля графиков
	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(areaBL, 0.0f));
	dataModel	= scale(dataModel, vec3(grid.x/5.0f, 1.0f, 0.f));
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
		areaMat	= translate(areaMat, vec3(areaBL.x, areaBL.y+0.5f, 0));
		areaMat	= scale(areaMat, vec3(areaSize.x, -2.5f, 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);

	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(areaBL.x - dt/TimeScale*grid.x, areaBL.y,0.0f));
	dataModel	= scale(dataModel, vec3(grid.x/5.0f, 1.0f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glDrawArrays(GL_LINES, 0, nCountAxe);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_program->release();

	dataModel	= translate(mat4(1.f), vec3(areaBL.x - dt/TimeScale*grid.x, areaBL.y, 0.f));
	textLabel->setMatrix(dataModel);
	textLabel->renderText(vec3(0.f), 1);
}

}
