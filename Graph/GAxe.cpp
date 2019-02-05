#include "stdafx.h"
#include "GAxe.h"
#include <QDomElement>
#include "../Accumulation.h"
#include "GTextLabel.h"
#include "glm/gtx/color_space.hpp"

QColor	GetColor(int n);
int		GetMarker(int n);

namespace Graph{

double	GAxe::m_FontScale 	= 0.75;	//Просто коэффициент
double	GAxe::m_TickSize  	= 1.5;	//Миллиметры
double	GAxe::m_Width		= 30;
double	GAxe::m_SelectedWidth	= 60;

QOpenGLShaderProgram*	GAxe::m_program	= 0;
int		GAxe::u_modelToWorld	= 0;
int		GAxe::u_worldToCamera	= 0;
int		GAxe::u_cameraToView	= 0;
int		GAxe::u_color			= 0;
int		GAxe::u_alpha			= 0;
int		GAxe::u_round			= 0;
int		GAxe::u_lineType		= 0;

QOpenGLShaderProgram*	GAxe::m_data_program	= 0;
int		GAxe::u_data_modelToWorld	= 0;
int		GAxe::u_data_worldToCamera	= 0;
int		GAxe::u_data_cameraToView	= 0;
int		GAxe::u_data_color			= 0;
int		GAxe::u_data_alpha			= 0;
int		GAxe::u_data_round			= 0;
int		GAxe::u_data_lineType		= 0;

QOpenGLShaderProgram*	GAxe::m_cross_program	= 0;
int		GAxe::u_cross_modelToWorld	= 0;
int		GAxe::u_cross_worldToCamera	= 0;
int		GAxe::u_cross_cameraToView	= 0;
GLuint	GAxe::cross_texture;
ivec2	GAxe::cross_texSize;

GAxe::GAxe()
{
	m_Type		= AXE;
	m_DataType	= Double;
	m_nMarker	= 0;
	m_nAcc		= 0;
	m_AccName	= "Данные №1";

	m_Record	= -1;
	m_OldPoint	= {0,0};
	m_nSubTicks	= 5;
	m_Offset	= -1;
	m_Data_Len	= 0;
	m_bShowNum	= false;
	m_SpecWidth	= m_Width;
	m_bSRK		= false;
	m_MaskSRK	= 0;
	m_nBitSRK	= 0;
	m_K_short	= 0;

	m_TextFormat	= "%g";

	m_pOrionData	= 0;
	m_pOrionTime	= 0;

	m_bAperiodic	= false;
	m_bOscill		= false;
	m_Aperiodic_T	= 0.01;
	m_Oscill_T		= 0.01;
	m_Oscill_Ksi	= 0.7;
	m_bInterpol		= true;
	m_AxeLength		= 0;

	textLabel	= new GTextLabel;
	m_program	= 0;
	dataVAO		= 0;
	dataVBO		= 0;
	axeVAO		= 0;
	axeVBO		= 0;
	oldGrid		= QSizeF(5.0f, 5.0f);
}

GAxe::~GAxe()
{
	clearGL();
	delete textLabel;
}

void	GAxe::initializeGL()
{
	m_bOpenGL_inited	= true;
	if(m_program == 0)
	{
		//Программа для шкалы и трафарета
		m_program	= new QOpenGLShaderProgram;
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe.vert");
		m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe.frag");
		m_program->link();

		m_program->bind();
		u_modelToWorld	= m_program->uniformLocation("modelToWorld");
		u_worldToCamera	= m_program->uniformLocation("worldToCamera");
		u_cameraToView	= m_program->uniformLocation("cameraToView");
		u_color			= m_program->uniformLocation("color");
		u_alpha			= m_program->uniformLocation("alpha");
		u_round			= m_program->uniformLocation("round");
		m_program->release();

		m_data_program	= new QOpenGLShaderProgram;
		m_data_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe_data.vert");
		m_data_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe_data.frag");
		m_data_program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/gaxe_data.geom");
		m_data_program->link();

		m_data_program->bind();
		u_data_modelToWorld		= m_data_program->uniformLocation("modelToWorld");
		u_data_worldToCamera	= m_data_program->uniformLocation("worldToCamera");
		u_data_cameraToView		= m_data_program->uniformLocation("cameraToView");
		u_data_color			= m_data_program->uniformLocation("color");
		u_data_alpha			= m_data_program->uniformLocation("alpha");
		u_data_round			= m_data_program->uniformLocation("round");
		u_data_lineType			= m_data_program->uniformLocation("lineType");
		m_data_program->release();

		//Программа для креста на оси
		m_cross_program	= new QOpenGLShaderProgram;
		m_cross_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe_cross.vert");
		m_cross_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe_cross.frag");
		m_cross_program->link();
		  
		m_cross_program->bind();
		u_cross_modelToWorld	= m_cross_program->uniformLocation("modelToWorld");
		u_cross_worldToCamera	= m_cross_program->uniformLocation("worldToCamera");
		u_cross_cameraToView	= m_cross_program->uniformLocation("cameraToView");
		m_cross_program->release();

		// Prepare texture
		QOpenGLTexture *gl_texture = new QOpenGLTexture(QImage(":/Resources/images/delete.png"));
		cross_texSize.x	= gl_texture->width();
		cross_texSize.y	= gl_texture->height();
		cross_texture	= gl_texture->textureId();

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate texture
		glBindTexture(GL_TEXTURE_2D, cross_texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	textLabel->initializeGL();
	setAxeLength(m_AxeLength);
}

void	GAxe::clearGL()
{
	if(dataVAO)	{ glDeleteVertexArrays(1, &dataVAO); dataVAO	= 0; }
	if(dataVBO)	{glDeleteBuffers(1, &dataVBO); dataVBO	= 0; }

	if(axeVAO)	{ glDeleteVertexArrays(1, &axeVAO); axeVAO	= 0; }
	if(axeVBO)	{ glDeleteBuffers(1, &axeVBO); axeVBO	= 0; }
	textLabel->clearGL();
}

void	GAxe::setAxeLength(int len)
{
	//Установка длины оси
	m_AxeLength	= len;
	m_FrameBR	= m_BottomRight;
	if(!m_bOpenGL_inited)	return;

	//Заливка данных в видеопамять
	vector<vec2>	data;

	//Прямоугольник трафарета
	data.push_back(vec2(1.f, 0.f));
	data.push_back(vec2(0.f, 0.f));
	data.push_back(vec2(1.f, 1.f));
	data.push_back(vec2(0.f, 1.f));

	//Штрихи оси
	for(int i = 0; i < m_AxeLength; i++)
	{
		data.push_back(vec2(-1.0f, 0 + 5.f*i));	data.push_back(vec2(0.f, 0 + 5.f*i));
		data.push_back(vec2(-0.5f, 1 + 5.f*i));	data.push_back(vec2(0.f, 1 + 5.f*i));
		data.push_back(vec2(-0.5f, 2 + 5.f*i));	data.push_back(vec2(0.f, 2 + 5.f*i));
		data.push_back(vec2(-0.5f, 3 + 5.f*i));	data.push_back(vec2(0.f, 3 + 5.f*i));
		data.push_back(vec2(-0.5f, 4 + 5.f*i));	data.push_back(vec2(0.f, 4 + 5.f*i));
	}

	//Верхний штрих
	data.push_back(vec2(-1.0f, 5.f*m_AxeLength));	data.push_back(vec2(0.f, 5.f*m_AxeLength));

	//Вертикальная линия
	data.push_back(vec2(0.f, 0.f));	data.push_back(vec2(0.f, 5.f*m_AxeLength));
	m_Axe_nCount	= data.size();

	//Черточки для обрамления
	{
		float	dx	= 0.25*oldGrid.width();
		float	dy	= 0.15*oldGrid.height();

		data.push_back(vec2(-dx, -dy));
		data.push_back(vec2(-dx, dy + oldGrid.height()*m_AxeLength));
		data.push_back(vec2(0.5*dx, dy + oldGrid.height()*m_AxeLength));
		data.push_back(vec2(0.5*dx, -dy));
	}

	//Данные для креста
	{
		//Координаты и текстурные координаты
		data.push_back(vec2(1.f, -1.f));	data.push_back(vec2(1.f, 1.f));
		data.push_back(vec2(-1.f, -1.f));	data.push_back(vec2(0.f, 1.f));
		data.push_back(vec2(1.f, 1.f));		data.push_back(vec2(1.f, 0.f));
		data.push_back(vec2(-1.f, 1.f));	data.push_back(vec2(0.f, 0.f));
	}

	//Буфер для оси
	if(axeVAO)	{ glDeleteVertexArrays(1, &axeVAO); axeVAO	= 0; }
	if(axeVBO)	{ glDeleteBuffers(1, &axeVBO); axeVBO	= 0; }

	glGenVertexArrays(1, &axeVAO);
	glBindVertexArray(axeVAO);
	glGenBuffers(1, &axeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(vec2), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//Текстовые метки
	QSizeF grid	= oldGrid;
	textLabel->clearGL();
	textLabel->initializeGL();
	textLabel->setFont(m_scale*3.5f, m_scale);
	textLabel->addString(m_Name, -textLabel->textSize(m_Name).x, m_AxeLength*grid.height() + 1.5);
	for(int i = 0; i <= m_AxeLength; i++)
	{
		QString	txt		= QString("%1").arg(m_Min + i*m_AxeScale);
		vec2	size	= textLabel->textSize(txt);
		textLabel->addString(txt, -size.x - 2., i*grid.height() - textLabel->midLine());
	}

	textLabel->prepare();
}

void	GAxe::Save(QDomElement* node)
{/*
	IXMLDOMNamedNodeMap*	map		= 0;
	pXml->GetNodeMap(node, &map);
	pXml->SetNodeItem(map, "Название",	m_Name);
	pXml->SetNodeItem(map, "Путь",		m_Path);
	pXml->SetNodeItem(map, "Накопление",ToString(m_nAcc));
//	pXml->SetNodeItem(map, "Имя_накопления", m_AccName);
	pXml->SetNodeItem(map, "Цвет",		ToString(m_Color));
	pXml->SetNodeItem(map, "Маркер",	ToString(m_nMarker));
	pXml->SetNodeItem(map, "Минимум",	ToString(m_Min));
	pXml->SetNodeItem(map, "Шаг",		ToString(m_Scale));
	pXml->SetNodeItem(map, "Длина",		ToString(m_Length));
	pXml->SetNodeItem(map, "X_мм", 		ToString(m_BottomRight.x));
	pXml->SetNodeItem(map, "Y_мм", 		ToString(m_BottomRight.y));
	pXml->SetNodeItem(map, "Тип", 		ToString(m_DataType));
	pXml->SetNodeItem(map, "Толщина", 	ToString(m_SpecWidth));
	pXml->SetNodeItem(map, "СРК", 		ToString(m_bSRK));
	pXml->SetNodeItem(map, "Бит_СРК",	ToString(m_nBitSRK));
	pXml->SetNodeItem(map, "Формат",	m_TextFormat);
	pXml->SetNodeItem(map, "Интерполяция",	ToString(m_bInterpol));
	if(m_bAperiodic)
	{
		pXml->SetNodeItem(map, "Апериодика",	ToString(m_bAperiodic));
		pXml->SetNodeItem(map, "T_Aperiodic",	ToString(m_Aperiodic_T));
	}
	if(m_bOscill)
	{
		pXml->SetNodeItem(map, "Колебательное",	ToString(m_bOscill));
		pXml->SetNodeItem(map, "T_Oscill",	ToString(m_Oscill_T));
		pXml->SetNodeItem(map, "Ksi_Oscill",	ToString(m_Oscill_Ksi));
	}*/

}
void	GAxe::Load(QDomElement* node)
{
	//Получаем набор полей
	if(node->hasAttribute("Название"))		m_Name			= node->attribute("Название");
	if(node->hasAttribute("Путь"))			m_Path			= node->attribute("Путь");
	if(node->hasAttribute("Накопление"))	m_nAcc			= node->attribute("Накопление").toInt();
	if(node->hasAttribute("Имя_накопления"))m_AccName		= node->attribute("Имя_накопления");
	if(node->hasAttribute("Цвет"))			
	{
		//Разбираем текст на компоненты
		QString	colorText	= node->attribute("Цвет");
		colorText.remove('(');
		colorText.remove(')');
		QStringList	colors	= colorText.split(",");
		if(colors.size() == 3)
		{
			m_Color.r	= colors.at(0).toInt()/255.;
			m_Color.g	= colors.at(1).toInt()/255.;
			m_Color.b	= colors.at(2).toInt()/255.;
		}
	}
	if(node->hasAttribute("Маркер"))		m_nMarker		= node->attribute("Маркер").toInt();
	if(node->hasAttribute("Минимум"))		m_Min			= node->attribute("Минимум").toDouble();
	if(node->hasAttribute("Шаг"))			m_AxeScale			= node->attribute("Шаг").toDouble();
	if(node->hasAttribute("Длина"))			setAxeLength(node->attribute("Длина").toInt());
	if(node->hasAttribute("X_мм"))			m_BottomRight.x	= node->attribute("X_мм").toDouble();
	if(node->hasAttribute("Y_мм"))			m_BottomRight.y	= 297 + node->attribute("Y_мм").toDouble();
	if(node->hasAttribute("Тип"))			m_DataType		= (DataType)node->attribute("Тип").toInt();
	if(node->hasAttribute("Толщина"))		m_SpecWidth		= node->attribute("Толщина").toDouble();
	if(node->hasAttribute("СРК"))			m_bSRK			= node->attribute("СРК").toInt();
	if(node->hasAttribute("Бит_СРК"))		m_nBitSRK		= node->attribute("Бит_СРК").toInt();
	if(node->hasAttribute("Формат"))		m_TextFormat	= node->attribute("Формат");
	if(node->hasAttribute("Апериодика"))	m_bAperiodic	= true;
	if(node->hasAttribute("T_Aperiodic"))	m_Aperiodic_T	= node->attribute("T_Aperiodic").toDouble();
	if(node->hasAttribute("Колебательное"))	m_bOscill		= true;
	if(node->hasAttribute("T_Oscill"))		m_Oscill_T		= node->attribute("T_Oscill").toDouble();
	if(node->hasAttribute("Ksi_Oscill"))	m_Oscill_Ksi	= node->attribute("Ksi_Oscill").toDouble();
	if(node->hasAttribute("Интерполяция"))	m_bInterpol		= node->attribute("Интерполяция").toInt();

	if(m_bAperiodic || m_bOscill)	UpdateFiltering();
}

void	GAxe::SetPosition(double x, double y)
{
	m_BottomRight	= vec2(x,y);
	m_FrameBR		= vec2(x,y);
}

void	GAxe::SetPosition(vec2 pt)
{
	m_BottomRight	= pt;
	m_FrameBR		= pt;
}

void	GAxe::Draw(const double t0, const double TimeScale, const QSizeF& grid, const QRectF& area, const float alpha)
{
	//Контроль деления на ноль
	if(!TimeScale)	return;
	if(!grid.height())	return;
	if(oldGrid != grid)
	{
		oldGrid	= grid;
		setAxeLength(m_AxeLength);
	}
	if(oldScale != m_scale)
	{
		oldScale	= m_scale;
		setAxeLength(m_AxeLength);
	}
	oldArea	= area;

	//Отрисовка только double
	if(m_DataType != Double)	return;

	//Определяем диапазон индексов
	int	nMin	= 0;
	int	nMax	= m_data.size()-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(m_data.at(n).x <= t0)	nMin	= n;
		else						nMax	= n;
	}

	int	nStartIndex	= max(0, nMin);

	nMin	= 0;
	nMax	= m_data.size()-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(m_data.at(n).x <= t0 + TimeScale*(area.width()/grid.width()))	nMin	= n;
		else																nMax	= n;
	}
	int	nStopIndex	= min(int(m_data.size()-1), nMax);

	//Смешиваем цвет с белым
	vec3 color	= m_Color*alpha + vec3(1.0f)*(1.0f-alpha);

	//Заливаем матрицы в шейдер
	m_program->bind();

	glUniform3fv(u_color, 1, &color.r);
	glUniform1f(u_alpha, 1.0f);//alpha);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
	glUniform1i(u_round, 1);
	
	//Рисуем шкалу
	glBindVertexArray(axeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	mat4 dataModel	= mat4(1.0f);
	dataModel		= translate(dataModel, vec3(m_BottomRight, 0.f));
	dataModel		= scale(dataModel, vec3(1.5f, grid.height()/5.0f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glDrawArrays(GL_LINES, 4, m_Axe_nCount-4);

	//Рисуем обрамление шкалы
	if(m_IsSelected)
	{
		vec3 color2(0.7f);
		glUniform3fv(u_color, 1, &color2.r);
		mat4 dataModel	= mat4(1.0f);
		dataModel		= translate(dataModel, vec3(m_FrameBR, 0.f));
		dataModel		= scale(dataModel, vec3(1.5f, grid.height()/5.0f, 0.f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
		glDrawArrays(GL_LINE_LOOP, m_Axe_nCount, 4);
		glUniform3fv(u_color, 1, &color.r);
	}

	//Крест на оси без данных
	if(m_Record == -1)
	{
		m_cross_program->bind();
		glUniformMatrix4fv(u_cross_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_cross_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		mat4	cross(1.0f);
		cross	= translate(cross, vec3(m_BottomRight.x, m_BottomRight.y + 0.5f*m_AxeLength*grid.height(), 0));
		cross	= scale(cross, vec3(0.6*grid.width(), 0.6*grid.width(), 1.0f));
		glUniformMatrix4fv(u_cross_modelToWorld, 1, GL_FALSE, &cross[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cross_texture);

		//Меняем описание данных на два последовательных vec2
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_STRIP, (m_Axe_nCount+4)/2, 4);

		//Возвращаем настройки буфера
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_cross_program->release();
	}

	//Область графиков для трафарета
	{
		m_program->bind();
		glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		mat4	areaMat(1.0f);
		areaMat	= translate(areaMat, vec3(area.x(), area.y(), 0));
		areaMat	= scale(areaMat, vec3(area.width(), area.height(), 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	glBindVertexArray(0);

	dataModel	= translate(mat4(1.f), vec3(m_BottomRight, 0.f));
	textLabel->setMatrix(dataModel, m_view, m_proj);
	textLabel->renderText(color, alpha);

	//График с нулевым масштабом не рисуем
	if(!m_AxeScale)	return;
	if(!m_data.size())	return;

	m_data_program->bind();
	glUniform3fv(u_data_color, 1, &color.r);
	glUniform1f(u_data_alpha, 1.0f);//alpha);
	glUniformMatrix4fv(u_data_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_data_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
	glUniform1i(u_data_round, 0);
	glUniform1i(u_data_lineType, 1);

	//Подключаем буфер графика
	glBindVertexArray(dataVAO);
	glBindBuffer(GL_ARRAY_BUFFER, dataVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	//Формируем модельную матрицу
	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(area.x(), m_BottomRight.y, 0.f));
	dataModel	= scale(dataModel, vec3(grid.width()/TimeScale, grid.height()/m_AxeScale, 0.f));
	dataModel	= translate(dataModel, vec3(-t0, -m_Min, 0.f));

	if(m_IsSelected)
	{
		//Рисуем график со смещением
		vec3	color2	= 1.0f*m_Color + 0.0f*vec3(1.);
		glUniform3fv(u_data_color, 1, &color2.r);
		mat4	data2	= mat4(1.0f);
		data2	= translate(data2, vec3(area.x()+1.0f/m_scale, m_BottomRight.y-1.0f/m_scale, 0.f));
		data2	= scale(data2, vec3(grid.width()/TimeScale, grid.height()/m_AxeScale, 0.f));
		data2	= translate(data2, vec3(-t0, -m_Min, 0.f));
		glUniformMatrix4fv(u_data_modelToWorld, 1, GL_FALSE, &data2[0][0]);
		glDrawArrays(GL_LINE_STRIP, nStartIndex, nStopIndex - nStartIndex + 1);
//		glDrawArrays(GL_POINTS, nStartIndex, nStopIndex - nStartIndex + 1);

/*
		data2	= mat4(1.0f);
		data2	= translate(data2, vec3(area.x()-1.0f/m_scale, m_BottomRight.y+1.0f/m_scale, 0.f));
		data2	= scale(data2, vec3(grid.width()/TimeScale, grid.height()/m_AxeScale, 0.f));
		data2	= translate(data2, vec3(-t0, -m_Min, 0.f));
		glUniformMatrix4fv(u_data_modelToWorld, 1, GL_FALSE, &data2[0][0]);
		glDrawArrays(GL_LINE_STRIP, nStartIndex, nStopIndex - nStartIndex + 1);
*/
	}

	//Рисуем основной график
	glUniformMatrix4fv(u_data_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	glUniform3fv(u_data_color, 1, &m_Color.r);
	glDrawArrays(GL_LINE_STRIP, nStartIndex, nStopIndex - nStartIndex + 1);
//	glDrawArrays(GL_POINTS, nStartIndex, nStopIndex - nStartIndex + 1);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_data_program->release();
}

void	GAxe::Draw_DEC_S()
{
}

void	GAxe::OnDoubleClick()
{
/*
	Dialog::CAxeParam	dlg(this, 0);
	if(IDOK	== dlg.DoModal())
	{
		UpdateRecord(false);
		m_pDoc->CCS_UpdateReserv(this);
		m_pDoc->UpdateAllViews(0);
	}
*/
}

bool	GAxe::HitTest(const vec2& pt)
{
	//Определяем попадание в ось
	if(pt.x < m_BottomRight.x+1 &&
	   pt.x > m_BottomRight.x-3 &&
	   pt.y < m_BottomRight.y+oldGrid.height()*m_AxeLength+1 &&
	   pt.y > m_BottomRight.y-1)
		return true;
	else
		return false;
}
/*
HCURSOR GAxe::GetCursorHandle(const GPoint& pt, UINT nFlags)
{
	const vector2D& GridStep = m_pDoc->m_pField->m_GridStep;
	GRect	rc	= GetFrameRect();

	int x = pt.x;
	int y = pt.y;
	
	if	((x >= rc.left)	&& (x <= rc.right))
		//Попадание по Х
		if((y >= rc.bottom)&& (y <= rc.bottom + 0.5*GridStep.x*m_Zoom))
		{
			//Попадание в нижнюю часть
			m_Direction = BOTTOM;
			if(m_DataType == Bool){	m_Direction	= ALL; return m_hCurALL;}
			return m_hCurBOTTOM;
		}
		else if((y >= rc.top - 0.5*GridStep.y*m_Zoom)&& (y <= rc.top))
		{
			//Попадание в верхнюю часть
			m_Direction = TOP;
			if(m_DataType == Bool){	m_Direction	= ALL; return m_hCurALL;}
			return m_hCurTOP;
		}
		else if((y >= rc.bottom)&& (y <= rc.top))
		{
			//Попадание в середину
			m_Direction = ALL;
			return m_hCurALL;
		}
		else

			return NULL;
		
	else
		//Не попали
		return NULL;
}
*/

void	GAxe::OnStopMoving()
{
	m_FrameBR	= m_BottomRight;
}

bool	GAxe::MoveOffset(const vec2& delta, const Qt::MouseButtons& buttons, const Qt::KeyboardModifiers& mdf)
{
	//Переместим рамку
	m_FrameBR		+= delta;
	m_BottomRight.x	= m_FrameBR.x;

	//Положение оси по высоте округлим до сетки
	float	step	= oldGrid.height();
	if(m_DataType == Bool)	step	= 0.5f*step;
	if(mdf & Qt::ControlModifier)	m_BottomRight.y	= m_FrameBR.y;
	else							m_BottomRight.y	= int((m_FrameBR.y - oldArea.bottom())/step - 0.5f)*step + oldArea.bottom();

	bool Res = false;
/*
	//Переместим рамку
	m_FrameBR.x	+= pt.x;
	m_FrameBR.y	+= pt.y;

	m_BottomRight.x	= m_FrameBR.x;
	
	//Дальше в зависимости от типа перетаскивания
	switch(m_Direction)
	{
	case ALL:
		{
			//Перемещение без изменения размеров			
			if(m_DataType == Bool)
			{
				double N;	//Количество целых шагов сетки
				double rem	= modf((m_FrameBR.y - m_pDoc->m_pField->m_Rect.top)/
					m_pDoc->m_pField->m_GridStep.y*2.,&N);

				//Запомним старое положение оси по высоте
				double yOld	= m_BottomRight.y;

				if(rem > 0.5) N++;
				m_BottomRight.y = m_pDoc->m_pField->m_Rect.top + N*0.5*m_pDoc->m_pField->m_GridStep.y;
				if(nFlags & MK_CONTROL)
				{
					m_BottomRight.y = m_FrameBR.y;
				}


				Res = (m_BottomRight.y != yOld);
			}
			else
			{
				double N;	//Количество целых шагов сетки
				double rem	= modf((m_FrameBR.y - m_pDoc->m_pField->m_Rect.top)/
					m_pDoc->m_pField->m_GridStep.y,&N);

				//Запомним старое положение оси по высоте
				double yOld	= m_BottomRight.y;

				if(rem > 0.5) N++;
				m_BottomRight.y = m_pDoc->m_pField->m_Rect.top + N*m_pDoc->m_pField->m_GridStep.y;
				if (nFlags & MK_CONTROL)
				{
					m_BottomRight.y = m_FrameBR.y;
				}


				Res = (m_BottomRight.y != yOld);
			}
		}break;
	
	case TOP:
		{	
			//Растяжение вверх
			if((m_FrameBR.y - m_BottomRight.y) > m_pDoc->m_pField->m_GridStep.y)
			{
				//Передвинули вверх более чем на шаг				
				m_Length++;
				m_FrameBR.y -= m_pDoc->m_pField->m_GridStep.y;
			}
			else if((m_FrameBR.y - m_BottomRight.y) < -m_pDoc->m_pField->m_GridStep.y)
			{
				//Передвинули вниз более чем на шаг
				if(m_Length > 1)
				{
					m_Length--;
					m_FrameBR.y += m_pDoc->m_pField->m_GridStep.y;
				}
			}
			Res = false;
		}break;

	case BOTTOM:
		{			
			//Растяжение вниз
			if((m_FrameBR.y - m_BottomRight.y) > m_pDoc->m_pField->m_GridStep.y)
			{
				//Передвинули вверх более чем на шаг
				if(m_Length > 1)
				{
					m_Length--;
					m_Min += m_Scale;
					m_BottomRight.y += m_pDoc->m_pField->m_GridStep.y;
				}
			}
			else if((m_FrameBR.y - m_BottomRight.y) < -m_pDoc->m_pField->m_GridStep.y)
			{
				//Передвинули вниз более чем на шаг
				m_Length++;
				m_Min -= m_Scale;
				m_BottomRight.y -= m_pDoc->m_pField->m_GridStep.y;
			}

			Res = false;
		}break;	
	}

	m_pDoc->SetModifiedFlag();*/
	return Res;
}
/*
GRect	GAxe::GetFrameRect()
{
	const vector2D& GridStep = m_pDoc->m_pField->m_GridStep;
	GRect	rc;

	//В зависимости от типа перетаскивания изменим рамку
	switch(m_Direction)
	{
	case TOP:{
		rc.left		= (m_FrameBR.x - 0.25*GridStep.x)*m_Zoom;
		rc.right	= (m_FrameBR.x + 0.25*GridStep.x)*m_Zoom;
		rc.top		= (m_FrameBR.y + m_Length*GridStep.y + 0.25*GridStep.y)*m_Zoom;
		rc.bottom	= (m_BottomRight.y - 0.25*GridStep.y)*m_Zoom;
		}break;

	case BOTTOM:{
		rc.left		= (m_FrameBR.x - 0.25*GridStep.x)*m_Zoom;
		rc.right	= (m_FrameBR.x + 0.25*GridStep.x)*m_Zoom;
		rc.top		= (m_BottomRight.y + m_Length*GridStep.y + 0.25*GridStep.y)*m_Zoom;
		rc.bottom	= (m_FrameBR.y - 0.25*GridStep.y)*m_Zoom;
		}break;
	
	default:{
		rc.left		= (m_FrameBR.x - 0.25*GridStep.x)*m_Zoom;
		rc.right	= (m_FrameBR.x + 0.25*GridStep.x)*m_Zoom;
		rc.top		= (m_FrameBR.y + m_Length*GridStep.y + 0.25*GridStep.y)*m_Zoom;
		rc.bottom	= (m_FrameBR.y - 0.25*GridStep.y)*m_Zoom;
			 }break;	
	}

	//Для Bool рамка - название оси
	if(m_DataType == Bool)
	{
		rc.bottom	= (m_FrameBR.y-2)*m_Zoom;
		rc.right	= m_FrameBR.x*m_Zoom;
		rc.top		= rc.bottom + 6*m_Zoom;
		int	len		= m_Name.GetLength();
		if(len < 2)	len	= 2;
		rc.left		= rc.right	- len*2.1*m_Zoom;
	}
	return rc;
}
*/
void	GAxe::GetLimits(double* pMin, double* pMax)
{/*
	UpdateRecord(false);
	if(m_Record == -1)
	{
		*pMax = 0;
		*pMin = 0;
		return;
	}

	if(m_pDoc->GetBufArray().empty())	return;

	const Accumulation*				pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);
	const BYTE*						pData		= pBuffer->GetData();
	const int						RecCount	= pBuffer->GetRecCount();
	const int						RecSize		= pBuffer->GetRecSize();

	//Определяем смещение в векторе данных
	const int						Offset		= m_Offset;
	switch(pBuffer->GetType())
	{
	case Acc_KARP:
		{
			//Найдем минимум и максимум для КАРП
			float*	ptr	= (float*)(pData+m_Offset);
			float	Min	= *(ptr+1);
			double	Max = Min;
			for(int i = 0; i < m_KARP_Len; i++)
			{
				//Берем запись
				float	f	= *(ptr+2*i+1);
				if(f < Min) Min = f;
				if(f > Max)	Max = f;
			}

			*pMax = Max;
			*pMin = Min;
		}break;

	case Acc_Orion:
		{
			//Найдем минимум и максимум для Ориона
			if(m_pOrionData)
			{
				double	f	= 0;
				switch(m_DataType)
				{
				case Bool:		f	= *(bool*)(m_pOrionData + 0*sizeof(bool));	break;
				case Int:		f	= *(int*)(m_pOrionData + 0*sizeof(int));	break;
				case Double:	f	= *(double*)(m_pOrionData + 0*sizeof(double));	break;
				};

				double	Min	= f;
				double	Max = Min;
				for(int i = 0; i < m_KARP_Len; i++)
				{
					//Берем запись
					double	f	= 0;
					switch(m_DataType)
					{
					case Bool:		f	= *(bool*)(m_pOrionData + i*sizeof(bool));	break;
					case Int:		f	= *(int*)(m_pOrionData + i*sizeof(int));	break;
					case Double:	f	= *(double*)(m_pOrionData + i*sizeof(double));	break;
					};

					if(f < Min) Min = f;
					if(f > Max)	Max = f;
				}

				*pMax = Max;
				*pMin = Min;
			}
		}break;

	default:
		{
			if(RecCount)
			{
				//Найдем минимум и максимум
				double	Min	= GetValue(pData);
				double	Max = Min;

				for(int i = 0; i < RecCount; i++)
				{
					//Берем запись
					double	f		= GetValue(pData + i*RecSize);

					if(f < Min) Min = f;
					if(f > Max)	Max = f;
				}

				*pMax = Max;
				*pMin = Min;
			}
		}
	}*/
}

void	GAxe::GetLimits(double t0, double t1, double* pMin, double* pMax)
{/*
	UpdateRecord(false);
	if(m_Record == -1)
	{
		*pMax = 0;
		*pMin = 0;
		return;
	}

	if(m_pDoc->GetBufArray().empty())	return;

	const Accumulation*				pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);
	const BYTE*						pData		= pBuffer->GetData();
	const int						RecCount	= pBuffer->GetRecCount();
	const int						RecSize		= pBuffer->GetRecSize();

	//Определяем смещение в векторе данных
	const int	Offset		= m_Offset;
	switch(pBuffer->GetType())
	{
	case Acc_KARP:
		{
			//Найдем минимум и максимум для КАРП
			float*	ptr	= (float*)(pData+m_Offset);
			float	Min	= *(ptr+1);
			double	Max = Min;
			for(int i = 0; i < m_KARP_Len; i++)
			{
				//Берем запись
				float	f	= *(ptr+2*i+1);
				float	t	= *(ptr+2*i);

				if(t < t0) Min	= f;
				if(t > t1) break;

				if(f < Min) Min = f;
				if(f > Max)	Max = f;
			}

			*pMax = Max;
			*pMin = Min;
		}break;

	case Acc_Orion:
		{
			//Найдем минимум и максимум для Ориона
			if(m_pOrionData)
			{
				double	f	= 0;
				switch(m_DataType)
				{
				case Bool:		f	= *(bool*)(m_pOrionData + 0*sizeof(bool));	break;
				case Int:		f	= *(int*)(m_pOrionData + 0*sizeof(int));	break;
				case Double:	f	= *(double*)(m_pOrionData + 0*sizeof(double));	break;
				};

				double	Min	= f;
				double	Max = Min;
				for(int i = 0; i < m_KARP_Len; i++)
				{
					//Берем запись
					double	f	= 0;
					switch(m_DataType)
					{
					case Bool:		f	= *(bool*)(m_pOrionData + i*sizeof(bool));	break;
					case Int:		f	= *(int*)(m_pOrionData + i*sizeof(int));	break;
					case Double:	f	= *(double*)(m_pOrionData + i*sizeof(double));	break;
					};

					double	t	= m_pOrionTime[i];

					if(t < t0) Min	= f;
					if(t > t1) break;

					if(f < Min) Min = f;
					if(f > Max)	Max = f;
				}
				*pMax = Max;
				*pMin = Min;
			}
		}break;

	default:
		{
			if(RecCount)
			{
				//Найдем минимум и максимум
				double	Min	= GetValue(pData);
				double	Max = Min;

				for(int i = 0; i < RecCount; i++)
				{
					//Берем запись
					double	f		= GetValue(pData + i*RecSize);
					double	t		= GetTime(pData + i*RecSize);

					if(t < t0)	Min	= f;
					if(t > t1)	break;

					if(f < Min) Min = f;
					if(f > Max)	Max = f;
				}

				*pMax = Max;
				*pMin = Min;
			}
		}break;
	}*/
}

void	GAxe::FitToScale(double t0 /* = 0 */, double t1 /* = 0 */)
{/*
	//Если буфер пустой...
	if(m_nAcc == -1)	return;
	if(m_pDoc->GetBufArray().empty())	return;

	const Accumulation*		pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);

	if(!pBuffer->GetRecCount() && pBuffer->GetType() != Acc_Orion) 
		return;	

	//Найдем минимум и максимум
	double	Min;
	double	Max;

	if(t0 == 0 && t1 == 0)
		GetLimits(&Min, &Max);
	else
		GetLimits(t0, t1, &Min, &Max);

	//Зная минимум и максимум, определим диапазон
	double Step = (Max - Min)/m_Length;
	
	//Округлим его до нормализованного значения
	if(Step)
	{
		double Power	= floor(log10(Step));
		double Mantiss	= Step / pow(10., Power);
		
		if(Mantiss == 1)		m_Scale = 1*pow(10.,Power);
		else if(Mantiss <= 2)	m_Scale = 2*pow(10.,Power);
		else if(Mantiss <= 5)	m_Scale	= 5*pow(10.,Power);
		else					m_Scale	= 10*pow(10.,Power);
		
		//Итак, получили масштаб. Считаем минимальное значение
		m_Min = floor(Min/m_Scale)*m_Scale;		
	}
	else
	{
		m_Scale = 1;
		m_Min	= Min - m_Scale;
		m_Length= 2;
	}
	if(m_DataType == Bool)
	{
		m_Min	= 0;
		m_Scale	= 1;
		m_Length= 1;
	}*/
}

void	GAxe::UpdateRecord(std::vector<Accumulation*>* pData)
{
	//Необходимо уточнить номер колонки накопления в соответствии с прописанным путем
	if(m_nAcc == -1 || m_nAcc >= pData->size())
	{
		m_Record	= -1;
		return;
	}

	const Accumulation*				pBuffer		= pData->at(m_nAcc);
	const Accumulation::HeaderList&	Head		= pBuffer->GetHeader();

	//Перебираем все элементы заголовка накопления
	int	AccIndex	= 0;
	for(size_t posHead = 0; posHead < Head.size(); posHead++)
	{
		const Accumulation::HeaderElement&	H	= Head[posHead];
		
		//Для каждого элемента собираем путь
		QString	Path;
		for(size_t pos = 0; pos < H.Desc.size(); pos++)
		{
			const Accumulation::Level& L	= H.Desc[pos];
			Path += L.Name + "\\";
		}

		//Если нашли такой путь
		if(m_Path == Path)
		{
			m_Record	= AccIndex;
			m_Offset	= H.Offset;
			m_Data_Len	= H.Length;
			m_K_short	= H.K;

			//Тут же уточняем тип данных
			switch(H.Desc.back().nIcon)
			{
			case 0:		m_DataType		= Bool;		break;
			case 1:		m_DataType		= Int;		break;
			case 2:		m_DataType		= Double;	break;
			case 12:	m_DataType		= Float;	break;
			case 13:	m_DataType		= Int;		break;
			case 14:	m_DataType		= Short;	break;
			default:	throw;
			};

			//Принудительно для СРК выставляем признак
			if(m_bSRK)	
			{
				m_DataType		= Bool;
				m_MaskSRK		= 0x1 << (m_nBitSRK-1);
				
				//В записях КСУ смещение на 1 бит
				if(pBuffer->GetType() == Acc_CCS)	m_MaskSRK	= m_MaskSRK << 1;
			}

			if(m_DataType == Bool)
			{
				setAxeLength(1);
				m_Min		= 0;
				m_AxeScale		= 1;
			}
		
			//Для Ориона подгружаем данные из большого файла
			if(pBuffer->GetType() == Acc_Orion && m_DataType == Double)
			{
				m_pOrionTime	= pBuffer->GetOrionTime(H);
				m_pOrionData	= pBuffer->GetOrionData(H);

				m_data.clear();
				for(int i = 0; i < m_Data_Len; i++)
				{
					m_data.push_back(vec2(m_pOrionTime[i], (float)(*(double*)(m_pOrionData + i*sizeof(double)))));
				}
				if(!m_bOpenGL_inited)	return;

				if(dataVAO)	{ glDeleteVertexArrays(1, &dataVAO); dataVAO = 0; }
				if(dataVBO)	{ glDeleteBuffers(1, &dataVBO); dataVBO	= 0; }

				glGenVertexArrays(1, &dataVAO);
				glBindVertexArray(dataVAO);
				glGenBuffers(1, &dataVBO);
				glBindBuffer(GL_ARRAY_BUFFER, dataVBO);
				glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(vec2), m_data.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);

				//Обновляем VAO оси
				setAxeLength(m_AxeLength);
			}

			UpdateFiltering();
			return;
		}
		else
			AccIndex++;
	}

	//Раз не нашли, то...
	m_Record	= -1;
}

//Отрисовка маркера в заданных координатах
void	GAxe::DrawMarker(int x, int y)
{/*
	if(m_DataType == Bool)	return;

	pDC->SaveDC();
//	pDC->SelectStockObject(NULL_BRUSH);
	const double& GridStep = m_pDoc->m_pField->m_GridStep.y;

	switch(m_nMarker)
	{
	case 0:
		{
			//Окружность
			int	d = 0.2*m_Zoom*GridStep;
			pDC->Ellipse(x-d, y-d, x+d, y+d);
			pDC->SetPixel(x, y, m_Color);
		}break;
	case 1:
		{
			//Квадрат
			int	d = 0.15*m_Zoom*GridStep;
			pDC->Rectangle(x-d, y-d, x+d, y+d);
			pDC->SetPixel(x, y, m_Color);
		}break;
	case 2:
		{
			//Сплошной квадрат
			int	d = 0.15*m_Zoom*GridStep;
			CRect	rc(x-d, y-d, x+d, y+d);
			pDC->FillRect(rc, &CBrush(m_Color));
			pDC->SetPixel(x, y, m_Color);
		}break;
	case 3:
		{
			//Треугольник вниз
			int	d = 0.2*m_Zoom*GridStep;
			pDC->MoveTo(x - d*0.86	, y + d/2);
			pDC->LineTo(x + d*0.86	, y + d/2);
			pDC->LineTo(x			, y - d);
			pDC->LineTo(x - d*0.86	, y + d/2);
			pDC->SetPixel(x, y, m_Color);
		}break;
	case 4:
		{
			//Треугольник вверх
			int	d = 0.2*m_Zoom*GridStep;
			pDC->MoveTo(x - d*0.86	, y - d/2);
			pDC->LineTo(x			, y + d);
			pDC->LineTo(x + d*0.86	, y - d/2);
			pDC->LineTo(x - d*0.86	, y - d/2);
			pDC->SetPixel(x, y, m_Color);
		}break;
	case 5:
		{
			//Х
			int	d = 0.15*m_Zoom*GridStep;
			pDC->MoveTo(x - d	, y - d);
			pDC->LineTo(x + d	, y + d);
			pDC->MoveTo(x - d	, y + d);
			pDC->LineTo(x + d	, y - d);
		}break;
	case 6:
		{
			//Ромб
			int	d = 0.15*m_Zoom*GridStep;
			pDC->MoveTo(x - d	, y);
			pDC->LineTo(x		, y + d);
			pDC->LineTo(x + d	, y);
			pDC->LineTo(x		, y - d);
			pDC->LineTo(x - d	, y);
		}break;
	}

	pDC->RestoreDC(-1);*/
}

double	GAxe::GetValueAtTime(const double Time) const
{
	return 0;
	/*
	//Определяем смещение в векторе данных
	if(m_Record == -1)					return 0;
	if(m_nAcc == -1)					return 0;	
	if(m_pDoc->GetBufArray().empty())	return 0;

	const Accumulation*	pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);
	const BYTE*			pData		= pBuffer->GetData();
	const int			RecCount	= pBuffer->GetRecCount();
	const int			RecSize		= pBuffer->GetRecSize();
	const AccType		Type		= pBuffer->GetType();

	switch(Type)
	{
	case Acc_KARP:
		{
			//Ищем спец. для КАРП'а
			int	nMin	= 0;
			int	nMax	= m_KARP_Len;
			float*	ptr	= (float*)(pData+m_Offset);

			while(nMax - nMin > 1)
			{
				int n	= (nMin+nMax)/2;
				if(*(ptr+2*n) <= Time)	nMin	= n;
				else					nMax	= n;
			}

			double	f1	= *(ptr+2*nMin+1);	double	t1	= *(ptr+2*nMin);
			double	f2	= *(ptr+2*nMax+1);	double	t2	= *(ptr+2*nMax);
			if(m_bInterpol)	return	f1+(f2-f1)/(t2-t1)*(Time-t1);
			else			return	f1;
		}break;

	case Acc_Orion:
		{
			if(!m_pOrionTime || !m_pOrionData)	return 0;

			//Ищем спец. для Ориона
			int	nMin	= 0;
			int	nMax	= m_KARP_Len;

			while(nMax - nMin > 1)
			{
				int n	= (nMin+nMax)/2;
				if(m_pOrionTime[n] <= Time)	nMin	= n;
				else						nMax	= n;
			}

			double	t1	= m_pOrionTime[nMin];
			double	t2	= m_pOrionTime[nMax];

			double	f1;
			double	f2;
			switch(m_DataType)
			{
			case Bool:		
				{
					f1	= *(bool*)(m_pOrionData + nMin*sizeof(bool));
					return f1;
					//f2	= *(bool*)(m_pOrionData + nMax*sizeof(bool));
				}break;
			case Int:
				{
					f1	= *(int*)(m_pOrionData + nMin*sizeof(int));
					return f1;
					//f2	= *(int*)(m_pOrionData + nMax*sizeof(int));
				}break;
			case Double:	
				{
					f1	= *(double*)(m_pOrionData + nMin*sizeof(double));
					f2	= *(double*)(m_pOrionData + nMax*sizeof(double));
				}break;
			};
			
			if(m_bInterpol)	return	f1+(f2-f1)/(t2-t1)*(Time-t1);
			else			return	f1;
		}break;
	}

	//Ищем заданное время методом половинного деления
	int	nMin	= 0;
	int	nMax	= RecCount;
	while(nMax - nMin > 1)
	{
		int			n		= (nMin+nMax)/2;
		const BYTE*	pRec	= pData + n*RecSize;

		double	t	= 0;
		switch(Type)
		{
		case Acc_Excell:
		case Acc_MIG:
		case Acc_SAPR:	t	= *(double*)(pRec);		break;
		case Acc_TRF:	t	= *(float*) (pRec + 1);	break;
		case Acc_MIG_4:
		case Acc_CCS:	t	= *(float*) (pRec);		break;
		}

		if(t <= Time)	nMin	= n;
		else			nMax	= n;
	}

	//Получаем запись
	const BYTE*	pRec	= pData + nMin*RecSize;
	double		f		= 0;
	
	switch(m_DataType)
	{
	case Bool:		f		= *(bool*)	(pRec + m_Offset);	break;
	case Int:		f		= *(int*)	(pRec + m_Offset);	break;
	case Double:	f		= *(double*)(pRec + m_Offset);	break;
	case Float:		f		= *(float*) (pRec + m_Offset);	break;
	case Short:		
		{
			f		= *(short*) (pRec + m_Offset);
			if(m_K_short)	f *= m_K_short;
		}break;
	};

	//Особый случай для СРК
	if(m_bSRK)
	{
		switch(pBuffer->GetType())
		{
		case Acc_MIG:
		case Acc_MIG_4:	
			{
				DWORD	a	= *(DWORD*)(pRec + m_Offset);	
				f	= (a & m_MaskSRK) != 0;
			}break;
		default:
			{
				float	fl	= *(DWORD*)(pRec + m_Offset);
				DWORD	a	= *(DWORD*)&fl;
				if(a < 0)	a = ~a;
				f	= (a & m_MaskSRK) != 0;
			}
		}
	}

	return f;*/
}

double GAxe::GetTopPosition()
{
	return m_BottomRight.y;// + m_Length*m_pDoc->m_pField->m_GridStep.y;
}

bool	GAxe::IsBoolean() const
{
	return m_DataType	== Bool;
}

void	GAxe::GetStatistic() const
{/*
	//Получаем диапазон времени
	double	T0;
	double	T1;
	if(!m_pDoc->GetSelectedTime(T0, T1))	
	{
		AfxMessageBox("Нет выделенного участка!");
		return;
	}

	//Перебор по диапазону
	double	Min;
	double	Max;
	double	MO;		//Матожидание
	double	D;		//Дисперсия
	double	Sigma;	//СКО
	int		nPoints;	//Количество точек
	
	if(m_Record == -1)
	{
		Min 	= 0;
		Max 	= 0;
		MO		= 0;
		D		= 0;
		Sigma	= 0;
		nPoints	= 0;

		return;
	}

	if(m_pDoc->GetBufArray().empty())	return;

	const Accumulation*				pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);
	const BYTE*						pData		= pBuffer->GetData();
	const int						RecCount	= pBuffer->GetRecCount();
	const int						RecSize		= pBuffer->GetRecSize();
	const int						Offset		= m_Offset;

	if(pBuffer->GetType() == Acc_Orion && (!m_pOrionTime || !m_pOrionData))	return;

	//Определяем диапазон записей
	int	nStart	= 0;
	int	nStop	= 0;
	int	len		= RecCount;
	if(pBuffer->GetType() == Acc_Orion)	len	= m_KARP_Len;
	for(int i = 0; i < len; i++)
	{
		//Берем запись
		double	t;
		if(pBuffer->GetType() == Acc_Orion)	t	= m_pOrionTime[i];
		else								t	= GetTime(pData + i*RecSize);

		if(t <= T0)	
		{
			nStart	= nStop	= i;
		}
		if(t >= T1)	
		{
			nStop	= i;
			break;
		}
	}

	nPoints	= nStop - nStart;
	if(!nPoints)	return;

	//Расчет матожидания и пределов
	double	Sum	= 0;
	double	f_Start;
	if(pBuffer->GetType() == Acc_Orion)
	{
		switch(m_DataType)
		{
		case Bool:	{f_Start	= *(bool*)(m_pOrionData + nStart*sizeof(bool));}		break;
		case Int:	{f_Start	= *(int*)(m_pOrionData + nStart*sizeof(int));}			break;
		case Double:{f_Start	= *(double*)(m_pOrionData + nStart*sizeof(double));}	break;
		};
	}
	else	
		f_Start	= GetValue(pData + nStart*RecSize);

	Min	= Max	= f_Start;
	for(int i = nStart; i < nStop; i++)
	{
		double	f;
		if(pBuffer->GetType() == Acc_Orion)
		{
			switch(m_DataType)
			{
			case Bool:	{f	= *(bool*)(m_pOrionData + i*sizeof(bool));}		break;
			case Int:	{f	= *(int*)(m_pOrionData + i*sizeof(int));}		break;
			case Double:{f	= *(double*)(m_pOrionData + i*sizeof(double));}	break;
			};
		}
		else	
			f	= GetValue(pData + i*RecSize);

		Sum	+= f;
		if(f < Min)	Min	= f;
		if(f > Max)	Max	= f;
	}
	MO	= Sum/nPoints;

	//Расчет СКО
	int	Res	= MessageBox(GetFocus(), "СКО от среднего?", "Статистика", MB_YESNO | MB_ICONQUESTION);
	if(Res == IDNO)	MO	= f_Start;
	Sum	= 0;
	for(int i = nStart; i < nStop; i++)
	{
		double	f;
		if(pBuffer->GetType() == Acc_Orion)
		{
			switch(m_DataType)
			{
			case Bool:	{f	= *(bool*)(m_pOrionData + i*sizeof(bool));}		break;
			case Int:	{f	= *(int*)(m_pOrionData + i*sizeof(int));}		break;
			case Double:{f	= *(double*)(m_pOrionData + i*sizeof(double));}	break;
			};
		}
		else	
			f	= GetValue(pData + i*RecSize);

		Sum	+= (f-MO)*(f-MO);
	}
	D		= Sum/nPoints;
	Sigma	= sqrt(D);

	//Выдача в диалог
	CString	msg;
	msg.AppendFormat("Статистика для сигнала \"%s\" по выборке\n", m_Name);
	msg.AppendFormat("T0 = %g, dT = %g сек. (%d точек).\n\n", T0, T1-T0, nPoints);
	msg.AppendFormat("Минимум:\t%g\n", Min);
	msg.AppendFormat("Максимум:\t%g\n", Max);
	msg.AppendFormat("Матожидание:\t%g\n", MO);
	msg.AppendFormat("Дисперсия:\t%g\n", D);
	msg.AppendFormat("Sigma:\t\t%g\n", Sigma);

	//Копируем полученный текст
	HANDLE	hText	= GlobalAlloc(GMEM_MOVEABLE, msg.GetLength()+1);
	char*	cText	= (char*)GlobalLock(hText);
	strcpy(cText, msg);
	GlobalUnlock(hText);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hText);
	CloseClipboard();

	AfxMessageBox(msg);*/
}

void	GAxe::ErrorsFilter() const
{/*
	//Фильтруем одиночные сбои
	if(m_DataType == Float)
	{
		//Получаем диапазон времени
		double	T0;
		double	T1;
		if(!m_pDoc->GetSelectedTime(T0, T1))	
		{
			AfxMessageBox("Нет выделенного участка!");
			return;
		}

		//Перебор по диапазону
		if(m_Record == -1)	return;
		if(m_pDoc->GetBufArray().empty())	return;

		const Accumulation*				pBuffer		= m_pDoc->GetBufArray().at(m_nAcc);
		const BYTE*						pData		= pBuffer->GetData();
		const int						RecCount	= pBuffer->GetRecCount();
		const int						RecSize		= pBuffer->GetRecSize();
		const int						Offset		= m_Offset;

		//Определяем диапазон записей
		int	nStart	= 0;
		int	nStop	= 0;
		for(int i = 0; i < RecCount; i++)
		{
			//Берем запись
			double	t		= GetTime(pData + i*RecSize);

			if(t <= T0)	
			{
				nStart	= nStop	= i;
			}
			if(t >= T1)	
			{
				nStop	= i;
				break;
			}
		}

		for(int i = nStart; i < nStop-4; i++)
		{
			//Получаем 5 записей
			float*	pV1	= (float*)(pData + m_Offset + i*RecSize);
			float*	pV2	= (float*)(pData + m_Offset + (i+1)*RecSize);
			float*	pV3	= (float*)(pData + m_Offset + (i+2)*RecSize);
			float*	pV4	= (float*)(pData + m_Offset + (i+3)*RecSize);
			float*	pV5	= (float*)(pData + m_Offset + (i+4)*RecSize);

			//Признак сбоя
			if(abs((*pV5 + *pV1)/2.) > abs(10.*(*pV3)) && ((*pV1) * (*pV5) > 0) && (*pV1 > 0.1))
			{
				*pV2	= *pV3 = *pV1;
				*pV4	= *pV5;
			}
		}
	}*/
}

void	GAxe::UpdateFiltering()
{/*
	//Фильтрация сигнала
	if(!m_KARP_Len)	return;
	if(!m_pOrionTime || !m_pOrionData)	return;
	if(m_DataType != Double)	return;
	double	dt	= m_pOrionTime[1]-m_pOrionTime[0];
	if(!dt)	return;

	//Создаем копию сигнала
	if(m_pOriginal)
	{
		memcpy(m_pOrionData, m_pOriginal, m_KARP_Len*sizeof(double));		
		delete[] m_pOriginal;
		m_pOriginal	= 0;
	}
	m_pOriginal	= new BYTE[m_KARP_Len*sizeof(double)];
	memcpy(m_pOriginal, m_pOrionData, m_KARP_Len*sizeof(double));

	//Перезабиваем исходный массив фильтрованным сигналом
	double F	= *(double*)(m_pOriginal);
	double P	= 0;
	double Y	= 0;
	for(int i = 0; i < m_KARP_Len; i++)
	{
		double	t	= m_pOrionTime[i];
		double	u	= *(double*)(m_pOriginal + i*sizeof(double));
		double&	f	= *(double*)(m_pOrionData + i*sizeof(double));

		if(m_bAperiodic)
		{
			F	= F + (u-F)/m_Aperiodic_T*dt;
			f	= F;
		}
		else if(m_bOscill)
		{
			//колебательное звено (W = 1/(Ap2+Bp+1)): A=0.04*0.04  B = 2*0.5*0.04
			//P = x - Y * B - f
			//f = f + Y * dt
			//Y = Y + P * (dt/A)
			P	= u - Y*2.*m_Oscill_T*m_Oscill_Ksi - F;
			F	= F + Y*dt;
			Y	= Y + P/m_Oscill_T/m_Oscill_T*dt;

			f	= F;
		}
		else
		{
			f	= u;
		}
	}

//	m_pDoc->UpdateAllViews(0);*/
}

void	GAxe::Zoom(bool bUp)
{/*
	//Растяжение/сжатие масштаба
	if(m_DataType == Bool)	return;

	double Power	= floor(log10(m_Scale));
	double Mantiss	= m_Scale / pow(10., Power);

	//Изменяем масштаб в нужную сторону
	double	Scale;
	if(bUp)	
	{
		if(Mantiss == 1)		Scale	= 0.5*pow(10.,Power);
		else if(Mantiss <= 2)	Scale	= 1*pow(10.,Power);
		else if(Mantiss <= 5)	Scale	= 2*pow(10.,Power);
		else					Scale	= 5*pow(10.,Power);
	}
	else
	{
		if(Mantiss == 1)		Scale	= 2*pow(10.,Power);
		else if(Mantiss <= 2)	Scale	= 5*pow(10.,Power);
		else if(Mantiss <= 5)	Scale	= 10*pow(10.,Power);
		else					Scale	= 20*pow(10.,Power);
	}

	//Сохраняем значение в зависимости от того, где стоит курсор
	switch(m_Direction)
	{
	case TOP:
		{
			//Должно сохраниться верхнее значение
			m_Min	+= m_Length*(m_Scale - Scale);
			m_Scale	= Scale;
		}break;

	case BOTTOM:
		{
			//Должно сохраниться нижнее значение
			m_Scale	= Scale;
		}break;

	case ALL:
		{
			if(m_Min == -m_Scale)
			{
				//Должна сохраниться пропорция
				m_Scale	= Scale;
				m_Min	= -Scale;
			}
			else
			{
				//Должно сохраниться среднее значение
				m_Min	+= m_Length*(m_Scale - Scale)/2.;
				m_Scale	= Scale;
			}
		}break;
	}*/
}

}
