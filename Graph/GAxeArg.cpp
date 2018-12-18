#include "stdafx.h"
#include "GAxeArg.h"

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
}

GAxeArg::~GAxeArg()
{
	glDeleteVertexArrays(1, &gridVAO);
	glDeleteBuffers(1, &gridVBO);

	glDeleteVertexArrays(1, &axeVAO);
	glDeleteBuffers(1, &axeVBO);

	delete m_program;
}

void	GAxeArg::initializeGL()
{
	if(m_bOpenGL_inited)	return;
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

	//Буфер для сетки
	glGenVertexArrays(1, &gridVAO);
	glBindVertexArray(gridVAO);
	glGenBuffers(1, &gridVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	//glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Vertex), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Буфер для оси
	glGenVertexArrays(1, &axeVAO);
	glBindVertexArray(axeVAO);
	glGenBuffers(1, &axeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	//glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Vertex), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

struct Vertex
{
	vec2 pos;
	vec3 color;

	Vertex(vec2 p, vec3 c) :pos(p), color(c){}
};

void	GAxeArg::Draw(const double t0, const double TimeScale, const QSize& grid, const QRect& area)
{
	//Рисуем шкалу и сетку
	if(!TimeScale)	return;

	//Расчет ближайшей круглой точки
	double	dt	= t0 - (int)(t0/TimeScale)*TimeScale;
	double	dt5	= t0 - (int)(t0/TimeScale/5.)*5.*TimeScale;
	
	//Расчет количества пропускаемых клеток до первой жирной линии
	int	nSkip	= int(dt5/TimeScale);

	//Проверяем, надо ли обновлять данные
	if((TimeScale != oldTimeScale) ||
		(grid.width()	!= oldGrid.width()) ||
	   (area.width() != oldArea.width()) ||
	   t0 != oldTime)
	{
		//Запоминаем предыдущие значения
		oldTimeScale	= TimeScale;
		oldGrid			= grid;
		oldArea			= area;

		//Собираем новый буфер для сетки
		vector<Vertex>	dataGrid;
		vector<Vertex>	dataAxe;

		//Вертикальные линии
		int n	= 0;
		for(int x0 = 0; x0 <= area.width(); x0 += 1)
		{
			//Штрихи для оси
			dataAxe.push_back(Vertex(vec2(x0, 0.f), vec3(0.f)));
			dataAxe.push_back(Vertex(vec2(x0, -0.5f - 0.5f*((n%5)==0)), vec3(0.f)));

			if(!(n%5))
			{
				//Каждый пятый штрих оси рисуем вертикальную линию
				vec3	color	= (0.85f - 0.55f*(((n+5*nSkip)%25) == 0))*vec3(1.f);
				dataGrid.push_back(Vertex(vec2(x0, 0.f), color));
				dataGrid.push_back(Vertex(vec2(x0, area.height()), color));
			}
			n++;
		}

		//Горизонтальная линия оси
		dataAxe.push_back(Vertex(vec2(0.f, 0.f), vec3(0.f)));
		dataAxe.push_back(Vertex(vec2(area.width(), 0.f), vec3(0.f)));
		nCountAxe	= dataAxe.size();

		n	= 0;
		//Горизонтальные линии
		for(float y0 = 0; y0 <= area.height(); y0 += grid.height())
		{
			dataGrid.push_back(Vertex(vec2(0, y0),				(0.85f - 0.0f*(n%5 == 0))*vec3(1.0f)));
			dataGrid.push_back(Vertex(vec2(area.width(), y0),	(0.85f - 0.0f*(n%5 == 0))*vec3(1.0f)));
			n++;
		}
		nCountGrid	= dataGrid.size();

		//Пересоздаем буфер
		glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
		glBufferData(GL_ARRAY_BUFFER, dataGrid.size()*sizeof(Vertex), dataGrid.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
		glBufferData(GL_ARRAY_BUFFER, dataAxe.size()*sizeof(Vertex), dataAxe.data(), GL_STATIC_DRAW);
	}


	//Заливаем матрицы
	mat4	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x() - dt/TimeScale*grid.width(), area.y(),0.0f));
//	dataModel	= scale(dataModel, vec3(grid.width(), grid.height(), 0.0f));

	m_program->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

	//Рисуем сетку
	glBindVertexArray(gridVAO);
	glDrawArrays(GL_LINES, 0, nCountGrid);

	//Рисуем ось
	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x() - dt/TimeScale*grid.width(), area.y(),0.0f));
	dataModel	= scale(dataModel, vec3(grid.width()/5.0f, 1.5f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glBindVertexArray(axeVAO);
	glDrawArrays(GL_LINES, 0, nCountAxe);
	m_program->release();
}

}