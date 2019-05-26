#include "stdafx.h"
#include "GAxe.h"
#include <QDomElement>
#include "GTextLabel.h"
using std::min;
using std::max;

namespace Graph{

QOpenGLShaderProgram*	GAxe::m_axe_program	= nullptr;
int		GAxe::u_modelToWorld	= 0;
int		GAxe::u_worldToCamera	= 0;
int		GAxe::u_cameraToView	= 0;
int		GAxe::u_color			= 0;
int		GAxe::u_alpha			= 0;
int		GAxe::u_round			= 0;
int		GAxe::u_lineType		= 0;

QOpenGLShaderProgram*	GAxe::m_data_program	= nullptr;
int		GAxe::u_data_modelToWorld	= 0;
int		GAxe::u_data_worldToCamera	= 0;
int		GAxe::u_data_cameraToView	= 0;
int		GAxe::u_data_color			= 0;
int		GAxe::u_data_lineType		= 0;
int		GAxe::u_data_linewidth		= 0;
int		GAxe::u_data_antialias		= 0;

QOpenGLShaderProgram*	GAxe::m_bool_program	= nullptr;
int		GAxe::u_bool_modelToWorld	= 0;
int		GAxe::u_bool_worldToCamera	= 0;
int		GAxe::u_bool_cameraToView	= 0;
int		GAxe::u_bool_color			= 0;
int		GAxe::u_bool_lineType		= 0;

QOpenGLShaderProgram*	GAxe::m_marker_program	= nullptr;
int		GAxe::u_marker_ortho		= 0;
int		GAxe::u_marker_size			= 0;
int		GAxe::u_marker_orientation	= 0;
int		GAxe::u_marker_linewidth	= 0;
int		GAxe::u_marker_antialias	= 0;
int		GAxe::u_marker_fg_color		= 0;
int		GAxe::u_marker_bg_color		= 0;
int		GAxe::u_marker_type			= 0;

QOpenGLShaderProgram*	GAxe::m_cross_program	= nullptr;
int		GAxe::u_cross_modelToWorld	= 0;
int		GAxe::u_cross_worldToCamera	= 0;
int		GAxe::u_cross_cameraToView	= 0;
GLuint	GAxe::cross_texture;

QOpenGLShaderProgram*	GAxe::m_select_program;
int		GAxe::u_select_modelToWorld	= 0;
int		GAxe::u_select_worldToCamera	= 0;
int		GAxe::u_select_cameraToView	= 0;
int		GAxe::u_select_tick			= 0;
int		GAxe::u_select_toc			= 0;
int		GAxe::u_select_dL			= 0;
int		GAxe::u_select_color		= 0;
int		GAxe::u_select_round		= 0;

QOpenGLShaderProgram*	GAxe::m_fbo_program	= nullptr;
GLuint	GAxe::fbo					= 0;
GLuint	GAxe::rbo					= 0;
GLuint	GAxe::fboVBO				= 0;
GLuint	GAxe::fboTexture			= 0;
int		GAxe::fboWidth				= 0;
int		GAxe::fboHeight				= 0;

GLfloat	GAxe::m_width		= 1.0f;
GLfloat	GAxe::m_selWidth	= 1.0f;
GLfloat	GAxe::m_interpWidth	= 1.0f;
GLfloat	GAxe::m_alias		= 0.5f;
GLfloat	GAxe::m_selAlias	= 0.5f;
GLfloat	GAxe::m_interpAlias	= 0.5f;

GAxe::GAxe()
{
	m_Type		= AXE;
	m_Data_Type	= DataType::Double;
	m_nMarker	= 0;

	m_Data_Length	= 0;
	m_bShowNum	= false;
	m_bSRK		= false;
	m_MaskSRK	= 0;
	m_nBitSRK	= 0;
	m_TextFormat	= "%g";
	m_bAperiodic	= false;
	m_bOscill		= false;
	m_Aperiodic_T	= 0.01;
	m_Oscill_T		= 0.01;
	m_Oscill_Ksi	= 0.7;
	m_bInterpol		= true;
	m_Axe_Length	= 0;

	textLabel	= new GTextLabel;
	dataVBO		= 0;
	axeVBO		= 0;
	oldGrid		= vec2(5.0f);
	oldAreaBL	= vec2(65., 20.);
	oldTime0	= 0;
	oldTimeStep	= 20;
	m_markersCount	= 0;

	ph_dL		= 0;
	ph_dX		= 0;
	y_zad		= 0;
	ph_Y		= vec2(0.,0.);
	m_BottomRight	= vec2(0.,0.);
}

//Конструктор копирования
GAxe::GAxe(GAxe& axe):GAxe()
{
	m_Name			= axe.m_Name;
	m_Path			= axe.m_Path;
	m_DeltaPath		= axe.m_DeltaPath;
	m_nMarker		= axe.m_nMarker;
	m_Color			= axe.m_Color;
	m_TextFormat	= axe.m_TextFormat;
	m_bInterpol		= axe.m_bInterpol;
	m_AxeMin		= axe.m_AxeMin;
	m_AxeScale		= axe.m_AxeScale;
	m_Axe_Length	= axe.m_Axe_Length;
	m_BottomRight	= axe.m_BottomRight;
	m_FrameBR		= axe.m_FrameBR;
	m_Data_Type		= axe.m_Data_Type;
	oldGrid			= axe.oldGrid;
	oldAreaBL		= axe.oldAreaBL;
	oldAreaSize		= axe.oldAreaSize;
	oldScale		= axe.oldScale;
	oldTime0		= axe.oldTime0;
	oldTimeStep		= axe.oldTimeStep;
//	clearGL();
}

GAxe::~GAxe()
{
	clearGL();
	if(dataVBO)	{glDeleteBuffers(1, &dataVBO); dataVBO = 0;}
	if(axeVBO)	{glDeleteBuffers(1, &axeVBO); axeVBO = 0;}
	if(textLabel) {delete textLabel; textLabel = 0;}
}

void	GAxe::finalDelete()
{
	//Удаляем статические переменные
	if(cross_texture)		glDeleteTextures(1, &cross_texture);
	if(m_axe_program)		delete m_axe_program;
	if(m_data_program)		delete m_data_program;
	if(m_bool_program)		delete m_bool_program;
	if(m_marker_program)	delete m_marker_program;
	if(m_cross_program)		delete m_cross_program;
	if(m_select_program)	delete m_select_program;
	if(m_fbo_program)		delete m_fbo_program;
	if(fbo)					glDeleteFramebuffers(1, &fbo);
	if(rbo)					glDeleteRenderbuffers(1, &rbo);
	if(fboVBO)				glDeleteBuffers(1, &fboVBO);
	if(fboTexture)			glDeleteTextures(1, &fboTexture);
}

void	GAxe::initializeGL()
{
	m_bOpenGL_inited	= true;
	if(m_axe_program == 0)
	{
		//Программа для шкалы и трафарета
		m_axe_program	= new QOpenGLShaderProgram;
		m_axe_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe.vert");
		m_axe_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe.frag");
		m_axe_program->link();

		m_axe_program->bind();
		u_modelToWorld	= m_axe_program->uniformLocation("modelToWorld");
		u_worldToCamera	= m_axe_program->uniformLocation("worldToCamera");
		u_cameraToView	= m_axe_program->uniformLocation("cameraToView");
		u_color			= m_axe_program->uniformLocation("color");
		u_alpha			= m_axe_program->uniformLocation("alpha");
		u_round			= m_axe_program->uniformLocation("round");
		m_axe_program->release();

		//Программа для графиков линиями
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
		u_data_lineType			= m_data_program->uniformLocation("lineType");
		u_data_linewidth		= m_data_program->uniformLocation("linewidth");
		u_data_antialias		= m_data_program->uniformLocation("antialias");
		m_data_program->release();

		//Программа для bool
		m_bool_program	= new QOpenGLShaderProgram;
		m_bool_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe_data.vert");
		m_bool_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe_data.frag");
		m_bool_program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/gaxe_data_bool.geom");
		m_bool_program->link();

		m_bool_program->bind();
		u_bool_modelToWorld		= m_bool_program->uniformLocation("modelToWorld");
		u_bool_worldToCamera	= m_bool_program->uniformLocation("worldToCamera");
		u_bool_cameraToView		= m_bool_program->uniformLocation("cameraToView");
		u_bool_color			= m_bool_program->uniformLocation("color");
		u_bool_lineType			= m_bool_program->uniformLocation("lineType");
		m_bool_program->release();

		//Программа для маркеров
		m_marker_program	= new QOpenGLShaderProgram;
		m_marker_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/markers.vert");
		m_marker_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/markers.frag");
		m_marker_program->link();

		m_marker_program->bind();
		u_marker_ortho			= m_marker_program->uniformLocation("ortho");
		u_marker_size			= m_marker_program->uniformLocation("size");
		u_marker_orientation	= m_marker_program->uniformLocation("orientation");
		u_marker_linewidth		= m_marker_program->uniformLocation("linewidth");
		u_marker_antialias		= m_marker_program->uniformLocation("antialias");
		u_marker_fg_color		= m_marker_program->uniformLocation("fg_color");
		u_marker_bg_color		= m_marker_program->uniformLocation("bg_color");
		u_marker_type			= m_marker_program->uniformLocation("type");
		m_marker_program->release();

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

		//Программа для выделения
		m_select_program	= new QOpenGLShaderProgram;
		m_select_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/gaxe_selection.vert");
		m_select_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gaxe_selection.frag");
		m_select_program->link();
		  
		m_select_program->bind();
		u_select_modelToWorld	= m_select_program->uniformLocation("modelToWorld");
		u_select_worldToCamera	= m_select_program->uniformLocation("worldToCamera");
		u_select_cameraToView	= m_select_program->uniformLocation("cameraToView");
		u_select_tick			= m_select_program->uniformLocation("tick");
		u_select_toc			= m_select_program->uniformLocation("toc");
		u_select_dL				= m_select_program->uniformLocation("dL");
		u_select_color			= m_select_program->uniformLocation("color");
		u_select_round			= m_select_program->uniformLocation("round");
		m_select_program->release();

		//Программа для рисования из текстуры
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

		// Prepare texture
		QOpenGLTexture *gl_texture = new QOpenGLTexture(QImage(":/Resources/images/delete.png"));
		cross_texture	= gl_texture->textureId();

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate texture
		glBindTexture(GL_TEXTURE_2D, cross_texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	textLabel->initializeGL();

	if(dataVBO)
	{
		glDeleteBuffers(1, &dataVBO);
		glDeleteBuffers(1, &markerIBO);
	}

	glGenBuffers(1, &dataVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dataVBO);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(vec2), m_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &markerIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, markerIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10*sizeof(GLuint), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Обновляем VAO оси
	setAxeLength(m_Axe_Length);

	//Обновляем индексы
	oldTimeStep	= 0;
}

void	GAxe::clearGL()
{
	if(dataVBO)	{ glDeleteBuffers(1, &dataVBO); dataVBO = 0; }
	if(axeVBO)	{ glDeleteBuffers(1, &axeVBO); axeVBO = 0; }

	textLabel->clearGL();
}

void	GAxe::setAxeLength(int len, int highlighted)
{
	//Установка длины оси
	m_Axe_Length	= len;
	m_FrameBR	= m_BottomRight;
	if(!m_bOpenGL_inited)	return;

	textLabel->clearGL();
	textLabel->initializeGL();
	textLabel->setFont(3.5f);

	//Заливка данных в видеопамять
	vector<vec2>	data;

	//Прямоугольник трафарета
	data.push_back(vec2(1.f, 0.f));
	data.push_back(vec2(0.f, 0.f));
	data.push_back(vec2(1.f, 1.f));
	data.push_back(vec2(0.f, 1.f));

	//Штрихи оси
	for(int i = 0; i < m_Axe_Length; i++)
	{
		data.push_back(vec2(-1.0f, 0 + oldGrid.y*i));	data.push_back(vec2(0.f, 0 + oldGrid.y*i));
		data.push_back(vec2(-0.5f, 1 + oldGrid.y*i));	data.push_back(vec2(0.f, 1 + oldGrid.y*i));
		data.push_back(vec2(-0.5f, 2 + oldGrid.y*i));	data.push_back(vec2(0.f, 2 + oldGrid.y*i));
		data.push_back(vec2(-0.5f, 3 + oldGrid.y*i));	data.push_back(vec2(0.f, 3 + oldGrid.y*i));
		data.push_back(vec2(-0.5f, 4 + oldGrid.y*i));	data.push_back(vec2(0.f, 4 + oldGrid.y*i));
	}

	//Верхний штрих
	data.push_back(vec2(-1.0f, 5.f*m_Axe_Length));	data.push_back(vec2(0.f, 5.f*m_Axe_Length));

	//Вертикальная линия
	data.push_back(vec2(0.f, 0.f));	data.push_back(vec2(0.f, 5.f*m_Axe_Length));
	m_Axe_nCount	= data.size();

	//Данные для креста
	{
		//Координаты и текстурные координаты
		data.push_back(vec2(1.f, -1.f));	data.push_back(vec2(1.f, 1.f));
		data.push_back(vec2(-1.f, -1.f));	data.push_back(vec2(0.f, 1.f));
		data.push_back(vec2(1.f, 1.f));		data.push_back(vec2(1.f, 0.f));
		data.push_back(vec2(-1.f, 1.f));	data.push_back(vec2(0.f, 0.f));
	}

	//Черточки для обрамления
	{
		if(m_Data_Type == DataType::Bool)
		{
			//Координаты плюс длина по кругу
			vec2	textSize	= textLabel->textSize(m_Name);
			float	b	= textLabel->bottomLine();
			float	t	= textLabel->topLine();
			float	m	= textLabel->midLine();

			float	len	= 0;
			vec2	line	= vec2(-0.5f*textSize.x-1.f, b-m);
			data.push_back(line);	data.push_back(vec2(len));

			line	= vec2(-0.5f*textSize.x-1.f, t-m);
			len		= len + (t-b);
			data.push_back(line);	data.push_back(vec2(len));

			line	= vec2(0.5f*textSize.x+1.f, t-m);
			len		= len + (textSize.x + 2.f);
			data.push_back(line);	data.push_back(vec2(len));

			line	= vec2(0.5f*textSize.x+1.f, b-m);
			len		= len + (t-b);
			data.push_back(line);	data.push_back(vec2(len));

			line	= vec2(-0.5f*textSize.x-1.f, b-m);
			len		= len + (textSize.x + 2.f);
			data.push_back(line);	data.push_back(vec2(len));
		}
		else
		{
			float	dx	= 0.25*oldGrid.x;
			float	dy	= 0.15*oldGrid.y;

			//Координаты плюс длина по кругу
			float	len	= 0;
			vec2	line	= vec2(-1.5f*dx, -dy);
			data.push_back(line);	data.push_back(vec2(len));

			len		= len + 2.f*dy + oldGrid.y*m_Axe_Length;
			line	= vec2(-1.5f*dx, dy + oldGrid.y*m_Axe_Length);
			data.push_back(line);	data.push_back(vec2(len));

			len		= len + (2.f*dx);
			line	= vec2(0.5*dx, dy + oldGrid.y*m_Axe_Length);
			data.push_back(line);	data.push_back(vec2(len));

			len		= len + 2.f*dy + oldGrid.y*m_Axe_Length;
			line	= vec2(0.5*dx, -dy);
			data.push_back(line);	data.push_back(vec2(len));

			len		= len + (2.f*dx);
			line	= vec2(-1.5f*dx, -dy);
			data.push_back(line);	data.push_back(vec2(len));
		}
	}

	//Буфер для оси
	if(axeVBO)
	{ 
		glDeleteBuffers(1, &axeVBO);
	}
	glGenBuffers(1, &axeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(vec2), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Текстовые метки
	vec2 grid	= oldGrid;

	if(m_Data_Type == DataType::Bool)
	{
		textLabel->addString(m_Name, -textLabel->textSize(m_Name).x*0.5f, -textLabel->midLine());
	}
	else
	{
		textLabel->addString(m_Name, -textLabel->textSize(m_Name).x, m_Axe_Length*grid.y + 2.0f);
		for(int i = 0; i <= m_Axe_Length; i++)
		{
			if(i == highlighted) 	textLabel->setFont(5.5f);
			else 					textLabel->setFont(3.5f);

			double	val		= m_AxeMin + i*m_AxeScale;
			if(abs(val) < 1e-10)	val	= 0.0;
			QString	txt		= QString("%1").arg(val);
			vec2	size	= textLabel->textSize(txt);
			textLabel->addString(txt, -size.x - 2., i*grid.y - textLabel->midLine());
		}
	}

	textLabel->prepare();
}

void	GAxe::save(QXmlStreamWriter& xml)
{
	xml.writeStartElement("График");
	{
		xml.writeAttribute("Название", m_Name);
		xml.writeAttribute("Путь", m_Path);
		xml.writeAttribute("Цвет", QString("(%1, %2, %3)").arg(m_Color.r*255).arg(m_Color.g*255).arg(m_Color.b*255));
		xml.writeAttribute("Маркер", QString::number(m_nMarker));
		xml.writeAttribute("Минимум", QString::number(m_AxeMin));
		xml.writeAttribute("Шаг", QString::number(m_AxeScale));
		xml.writeAttribute("Длина", QString::number(m_Axe_Length));
		xml.writeAttribute("X_мм", QString::number(m_BottomRight.x));
		xml.writeAttribute("Y_мм", QString::number(m_BottomRight.y));
		xml.writeAttribute("Тип", QString::number(static_cast<int>(m_Data_Type)));
		xml.writeAttribute("СРК", m_bSRK ? "true" : "false");
		xml.writeAttribute("Бит_СРК", QString::number(m_nBitSRK));
		xml.writeAttribute("Формат", m_TextFormat);
		xml.writeAttribute("Интерполяция", m_bInterpol ? "true" : "false");
		if(!m_DeltaPath.isEmpty())
			xml.writeAttribute("Путь_дельты", m_DeltaPath);
	}
	xml.writeEndElement();	//График
}
void	GAxe::load(QDomElement* node, double ver)
{
	//Получаем набор полей
	if(node->hasAttribute("Название"))		m_Name			= node->attribute("Название");
	if(node->hasAttribute("Путь"))			m_Path			= node->attribute("Путь");
	//Убираем \ в конце
	if(m_Path.at(m_Path.length()-1) == '\\')
		m_Path	= m_Path.left(m_Path.length()-1);
	if(node->hasAttribute("Путь_дельты"))	m_DeltaPath		= node->attribute("Путь_дельты");

	if(ver < 2.1)
	{
		if(node->hasAttribute("Накопление"))
			m_Path		= QString("Данные №%1\\").arg(node->attribute("Накопление").toInt() + 1) + m_Path;
	}

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
	if(node->hasAttribute("Минимум"))		m_AxeMin		= node->attribute("Минимум").toDouble();
	if(node->hasAttribute("Шаг"))			m_AxeScale		= node->attribute("Шаг").toDouble();
	if(node->hasAttribute("Длина"))			setAxeLength(node->attribute("Длина").toInt());
	if(node->hasAttribute("X_мм"))			m_BottomRight.x	= node->attribute("X_мм").toDouble();
	if(node->hasAttribute("Y_мм"))			m_BottomRight.y	= node->attribute("Y_мм").toDouble();
	if(ver < 2.0)
	{
		m_BottomRight.x -= 65;
		m_BottomRight.y	+= 297 - 17;
	}
	y_zad	= m_BottomRight.y;
	ph_dX	= y_zad;
	if(node->hasAttribute("Тип"))			m_Data_Type		= (DataType)node->attribute("Тип").toInt();
	if(node->hasAttribute("СРК"))			m_bSRK			= node->attribute("СРК").toInt();
	if(node->hasAttribute("Бит_СРК"))		m_nBitSRK		= node->attribute("Бит_СРК").toInt();
	if(node->hasAttribute("Формат"))		m_TextFormat	= node->attribute("Формат");
	if(node->hasAttribute("Апериодика"))	m_bAperiodic	= true;
	if(node->hasAttribute("T_Aperiodic"))	m_Aperiodic_T	= node->attribute("T_Aperiodic").toDouble();
	if(node->hasAttribute("Колебательное"))	m_bOscill		= true;
	if(node->hasAttribute("T_Oscill"))		m_Oscill_T		= node->attribute("T_Oscill").toDouble();
	if(node->hasAttribute("Ksi_Oscill"))	m_Oscill_Ksi	= node->attribute("Ksi_Oscill").toDouble();
	if(node->hasAttribute("Интерполяция"))	m_bInterpol		= node->attribute("Интерполяция") == "true";

	if(m_bAperiodic || m_bOscill)	UpdateFiltering();
}

void	GAxe::SetPosition(double x, double y)
{
	m_BottomRight	= vec2(x,y);
	m_FrameBR		= vec2(x,y);
	ph_Y			= vec2(0.,y);
	y_zad			= y;
}

void	GAxe::SetPosition(vec2 pt)
{
	m_BottomRight	= pt;
	m_FrameBR		= pt;
	ph_Y			= vec2(0.,pt.y);
	y_zad			= pt.y;
}

void	GAxe::updateIndices(const double t0, const double TimeScale, const vec2& /*grid*/, const vec2& /*areaSize*/)
{
	if(!m_data.size())	return;

	//При необходимости пересчитываем индексы
	if(t0 != oldTime0 || TimeScale != oldTimeStep)
	{
		//Определяем диапазон индексов
		int	nMin	= 0;
		int	nMax	= m_data.size()-1;
		while(nMax - nMin > 1)
		{
			int n	= (nMin+nMax)/2;
			if(m_data.at(n).x <= t0)	nMin	= n;
			else						nMax	= n;
		}

		GLuint	nStartIndex	= max(0, nMin-1);
/*
		nMin	= 0;
		nMax	= m_data.size()-1;
		while(nMax - nMin > 1)
		{
			int n	= (nMin+nMax)/2;
			if(m_data.at(n).x <= t0 + TimeScale*(areaSize.x/grid.y))	nMin	= n;
			else														nMax	= n;
		}
		GLuint	nStopIndex	= min(int(m_data.size()-1), nMax+1);
*/
		oldTime0	= t0;
		oldTimeStep	= TimeScale;

		//Создаем буфер индексов для маркеров
		vector<GLuint>	markers;
		float	freq	= (m_data.back().x - m_data.front().x)/m_data.size();	//Средняя частота записи
		int dN	= 25.0*TimeScale/freq;	//Шаг между маркерами
		int	first	= 0;
		if(dN)	first	= (nStartIndex/dN)*dN + ((m_BottomRight.x-60.)/60.)*dN;
		if(first < 0)	first	= 0;
		m_markersCount	= 5;
		for(GLuint i = 0; i < m_markersCount; i++)
		{
            size_t	index	= first + (i+1)*dN;
			if(index > m_data.size())
			{
				m_markersCount	= i+1;
				break;
			}
			markers.push_back(index);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, markerIBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, markers.size()*sizeof(GLuint), markers.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void	GAxe::Draw(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha)
{
	//Контроль деления на ноль
	if(!TimeScale)	return;
	if(!grid.y)		return;
	if(oldGrid != grid)
	{
		oldGrid	= grid;
		setAxeLength(m_Axe_Length);
		updateIndices(t0, TimeScale, grid, areaSize);
	}
	if(oldScale != m_scale)
	{
		oldScale	= m_scale;
		setAxeLength(m_Axe_Length);
		updateIndices(t0, TimeScale, grid, areaSize);
	}
	oldAreaSize	= areaSize;
	oldAreaBL	= areaBL;

//	m_BottomRight.y	= Strip(ph_dX, 0.4, 0.5)[y_zad];
//	m_BottomRight.y	= Oscill(ph_Y, 0.05, 0.45)[y_zad];
	m_BottomRight.y	= y_zad;

	//Смешиваем цвет с белым
	vec3 color	= m_Color*alpha + vec3(1.0f)*(1.0f-alpha);

	//Заливаем матрицы в шейдер
	m_axe_program->bind();

	glUniform3fv(u_color, 1, &color.r);
	glUniform1f(u_alpha, 1.0f);//alpha);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
	glUniform1i(u_round, 0);
	
	//Рисуем шкалу
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	mat4 dataModel	= mat4(1.0f);
	dataModel		= translate(dataModel, vec3(areaBL + vec2(m_BottomRight.x, y_zad), 0.f));
	dataModel		= scale(dataModel, vec3(1.5f, grid.y/5.0f, 0.f));
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);

	if(m_Data_Type != DataType::Bool)
		glDrawArrays(GL_LINES, 4, m_Axe_nCount-4);

	//Рисуем обрамление шкалы
	if(m_IsSelected)
	{
		m_select_program->bind();
		mat4 dataModel	= translate(mat4(1.0f), vec3(areaBL + m_FrameBR, 0.f));
		glUniformMatrix4fv(u_select_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
		glUniformMatrix4fv(u_select_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_select_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		vec3 color2(0.3f);
		glUniform3fv(u_select_color, 1, &color2.r);
		glUniform1f(u_select_tick, 1.0f);
		glUniform1f(u_select_toc, 1.0f);
		glUniform1i(u_select_round, 1);

		glUniform1f(u_select_dL, Integral(ph_dL, 2.)[-1.]);

		//Меняем описание данных на два последовательных vec2
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_LINE_STRIP, (m_Axe_nCount+8)/2, 5);

		//Возвращаем настройки буфера
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

		m_select_program->release();
	}

	//Надписи у оси
	dataModel	= translate(mat4(1.f), vec3(areaBL + vec2(m_BottomRight.x, y_zad), 0.f));
	textLabel->setMatrix(dataModel);
	textLabel->renderText(color, alpha);
	glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Крест на пустой оси
	if(m_data.empty())
	{
		m_cross_program->bind();
		glUniformMatrix4fv(u_cross_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_cross_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		mat4	cross(1.0f);
		if(m_Data_Type == DataType::Bool)
			cross	= translate(cross, vec3(m_BottomRight.x, y_zad - 0.5*textLabel->midLine(), 0.f));
		else
			cross	= translate(cross, vec3(m_BottomRight.x, y_zad + 0.5f*m_Axe_Length*grid.y, 0.f));
		cross	= translate(cross, vec3(areaBL, 0.f));
		cross	= scale(cross, vec3(0.6f*grid.x, 0.6f*grid.x, 1.0f));
		glUniformMatrix4fv(u_cross_modelToWorld, 1, GL_FALSE, &cross[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cross_texture);

		//Меняем описание данных на два последовательных vec2
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_STRIP, m_Axe_nCount/2, 4);

		//Возвращаем настройки буфера
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_cross_program->release();
	}

	//Маркер возле оси
	if(m_Data_Type != DataType::Bool)
	{
		m_marker_program->bind();

		//Матрица проекции
		mat4	data(1.0f);
		data	= translate(data, vec3(m_BottomRight.x+0.5*grid.x, y_zad + m_Axe_Length*grid.y + 0.5*grid.y, 0.f));
		data	= translate(data, vec3(areaBL, 0.f));
		mat4	mpv	= m_proj*m_view*data;
		glUniformMatrix4fv(u_marker_ortho, 1, GL_FALSE, &mpv[0][0]);

		glUniform1f(u_marker_size, 0.5*grid.x*m_scale);
		glUniform1f(u_marker_orientation, 0.f);
		glUniform1f(u_marker_linewidth, 1.0f+0*0.25f*m_scale);
		glUniform1f(u_marker_antialias, 0.5f);
		vec4	fg_color	= vec4(0.8f*vec3(1.), 1.0f);//vec4(vec3(1.f)-color, 1.0f);
		glUniform4fv(u_marker_fg_color, 1, &fg_color.r);
		vec4	bg_color	= vec4(color, 1.0f);
		glUniform4fv(u_marker_bg_color, 1, &bg_color.r);
		glUniform1i(u_marker_type, m_nMarker);

		//Рисуем нулевую точку из буфера оси
		glDrawArrays(GL_POINTS, 1, 1);

		m_marker_program->release();
	}

	//Рисуем график сначала в текстуру
	GLuint	curFBO	= (GL_FRAMEBUFFER_BINDING);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0,0,fboWidth,fboHeight);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//Область графиков для трафарета
	{
		m_axe_program->bind();
		glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &m_proj[0][0]);

		mat4	areaMat(1.0f);
		areaMat	= translate(areaMat, vec3(areaBL, 0));
		areaMat	= scale(areaMat, vec3(areaSize, 1.0f));
		glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &areaMat[0][0]);

		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilMask(0x00);
	}

	//График с нулевым масштабом не рисуем
	if(!m_AxeScale || !m_data.size())
	{
		//Переключаемся обратно на экранный буфер
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		return;
	}
	updateIndices(t0, TimeScale, grid, areaSize);

	//Определяем диапазон индексов
	int	nMin	= 0;
	int	nMax	= m_data.size()-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(m_data.at(n).x <= t0)	nMin	= n;
		else						nMax	= n;
	}
	GLuint	nStartIndex	= max(0, nMin-1);

	nMin	= 0;
	nMax	= m_data.size()-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(m_data.at(n).x <= t0 + TimeScale*(areaSize.x/grid.y))	nMin	= n;
		else														nMax	= n;
	}
	GLuint	nStopIndex	= min(int(m_data.size()-1), nMax+1);

	//Формируем модельную матрицу
	dataModel	= mat4(1.0f);
	dataModel	= translate(dataModel, vec3(areaBL.x, areaBL.y + m_BottomRight.y, 0.f));
	dataModel	= scale(dataModel, vec3(grid.x/TimeScale, grid.y/m_AxeScale, 0.f));
	dataModel	= translate(dataModel, vec3(-t0, -m_AxeMin, 0.f));

	//Подключаем буфер графика
	glBindBuffer(GL_ARRAY_BUFFER, dataVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	if(m_Data_Type == DataType::Bool)
	{
		m_bool_program->bind();
		glUniform3fv(u_bool_color, 1, &color.r);
		glUniformMatrix4fv(u_bool_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_bool_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
		glUniformMatrix4fv(u_bool_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
		glUniform1i(u_bool_lineType, 3);
		
		glDrawArrays(GL_LINE_STRIP, nStartIndex, nStopIndex - nStartIndex + 1);		
		m_bool_program->release();
	}
	else
	{
		m_data_program->bind();
		glUniform3fv(u_data_color, 1, &color.r);
		glUniformMatrix4fv(u_data_worldToCamera, 1, GL_FALSE, &m_view[0][0]);
		glUniformMatrix4fv(u_data_cameraToView, 1, GL_FALSE, &m_proj[0][0]);
		glUniformMatrix4fv(u_data_modelToWorld, 1, GL_FALSE, &dataModel[0][0]);
	
		//Выставляем тип линии
		switch(m_Data_Type)
		{
			case DataType::Int:
			{
				glUniform1i(u_data_lineType, 1);
			}break;

			default:
			{
				if(m_bInterpol)
					glUniform1i(u_data_lineType, 0);
				else
					glUniform1i(u_data_lineType, 1);
			}break;
		}

		//Выставляем толщину линии
		if(m_IsSelected)	
		{
			glUniform1f(u_data_linewidth, m_selWidth/m_scale);
			glUniform1f(u_data_antialias, m_selAlias/m_scale);
			if(!m_bInterpol)
			{
				glUniform1f(u_data_linewidth, m_selWidth/m_scale);
				glUniform1f(u_data_antialias, m_interpAlias/m_scale);
			}
		}
		else				
		{
			glUniform1f(u_data_linewidth, m_width/m_scale);
			glUniform1f(u_data_antialias, m_alias/m_scale);
			if(!m_bInterpol)
			{
				glUniform1f(u_data_linewidth, m_interpWidth/m_scale);
				glUniform1f(u_data_antialias, m_interpAlias/m_scale);
			}
		}
		glDrawArrays(GL_LINE_STRIP, nStartIndex, nStopIndex - nStartIndex + 1);
		m_data_program->release();

		//Рисуем набор маркеров
		{
			m_marker_program->bind();

			//Настройка uniform
			mat4	mpv	= m_proj*m_view*dataModel;
			glUniformMatrix4fv(u_marker_ortho, 1, GL_FALSE, &mpv[0][0]);

			glUniform1f(u_marker_size, (1.5f + 1.5f*m_IsSelected)*m_scale);
			static float angle = 0;
			angle += 2.f/60.f/50.f;
			glUniform1f(u_marker_orientation, angle);
			glUniform1f(u_marker_linewidth, 1.f);
			glUniform1f(u_marker_antialias, 0.5f);
			vec4	fg_color	= vec4(0.8f*vec3(1.), 1.0f);//vec4(vec3(1.f)-color, 1.0f);//vec4(0.999f*vec3(1.), 1.0f);
			glUniform4fv(u_marker_fg_color, 1, &fg_color.r);
			vec4	bg_color	= vec4(color, 1.0f);
			glUniform4fv(u_marker_bg_color, 1, &bg_color.r);
			glUniform1i(u_marker_type, m_nMarker);

			//Отрисовка набора маркеров
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, markerIBO);
			glDrawElements(GL_POINTS, m_markersCount, GL_UNSIGNED_INT, nullptr);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			m_marker_program->release();
		}
	}

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Переключаемся обратно на экранный буфер
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//Рисуем полученную текстуру графика
	m_fbo_program->bind();
	glBindBuffer(GL_ARRAY_BUFFER, fboVBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glDisable(GL_STENCIL_TEST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_STENCIL_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_fbo_program->release();
}

bool	GAxe::hitTest(const vec2& pt_)
{
	vec2 pt	= pt_ - oldAreaBL;
	if(m_Data_Type == DataType::Bool)
	{
		//Для Bool рамка должна быть размером надписи
		vec2	textSize	= textLabel->textSize(m_Name);
		float	top	= textLabel->topLine();
		float	btm	= textLabel->bottomLine();
		float	mid	= textLabel->midLine();
		if(pt.x < m_BottomRight.x + 0.5f*textSize.x + 1.f &&
		   pt.x > m_BottomRight.x - 0.5f*textSize.x - 1.f &&
		   pt.y < m_BottomRight.y + top-mid &&
		   pt.y > m_BottomRight.y + btm-mid)
			return true;
		else
			return false;
	}
	else
	{
		//Определяем попадание в ось
		if(pt.x < m_FrameBR.x+1 &&
		   pt.x > m_FrameBR.x-3 &&
		   pt.y < m_FrameBR.y+oldGrid.y*m_Axe_Length+1 &&
		   pt.y > m_FrameBR.y-1)
			return true;
		else
			return false;
	}
}

bool	GAxe::getCursor(const vec2& /*pt*/, Qt::CursorShape& shape)
{
	shape	= Qt::CursorShape::SizeAllCursor;
/*
	if(m_DataType == Bool)	
	{
		shape	= Qt::CursorShape::SizeAllCursor; 
		m_Direction = ALL;
		return true;
	}

	vec2	mouse	= pt - m_FrameBR;
	if(mouse.y > oldGrid.y*(float(m_AxeLength)-0.5)-1.)	{shape	= Qt::CursorShape::SizeVerCursor; m_Direction = TOP;}
	else if(mouse.y < 0.5*oldGrid.y)					{shape	= Qt::CursorShape::SizeVerCursor; m_Direction = BOTTOM;}
	else												{shape	= Qt::CursorShape::SizeAllCursor; m_Direction = ALL;}
*/
	return	true;
}

void	GAxe::OnStopMoving()
{
	m_FrameBR	= m_BottomRight;
}

void	GAxe::MoveOffset(const vec2& delta, const Qt::MouseButtons& /*buttons*/, const Qt::KeyboardModifiers& mdf)
{
	//Переместим рамку
	m_FrameBR		+= delta;
	m_BottomRight.x	= m_FrameBR.x;

	//Положение оси по высоте округлим до сетки
	float	step	= oldGrid.y;
	if(m_Data_Type == DataType::Bool)	step	= 0.5f*step;
	if(mdf & Qt::AltModifier)			y_zad	= m_FrameBR.y;
	else								y_zad	= int(m_FrameBR.y/step + 0.5f)*step;

	return;
/*
	//Дальше в зависимости от типа перетаскивания
	switch(m_Direction)
	{
		case Graph::GAxe::TOP:
		{
			//Растягиваем верх
			if(m_FrameBR.y - m_BottomRight.y > oldGrid.y)
			{
				m_AxeLength++;
				setAxeLength(m_AxeLength);
			}
			else if(m_FrameBR.y - m_BottomRight.y < -oldGrid.y)
			{
				m_AxeLength--;
				if(m_AxeLength < 1)	m_AxeLength	= 1;
				setAxeLength(m_AxeLength);
			}
		}break;

		case Graph::GAxe::BOTTOM:
			//Растягиваем низ
			if(m_FrameBR.y - m_BottomRight.y > oldGrid.y)
			{
				m_AxeLength--;
				if(m_AxeLength < 1)	m_AxeLength	= 1;
				else
				{
					m_AxeMin += m_AxeScale;
					m_BottomRight.y	+= oldGrid.y;
				}
				setAxeLength(m_AxeLength);
			}
			else if(m_FrameBR.y - m_BottomRight.y < -oldGrid.y)
			{
				m_AxeLength++;
				m_AxeMin -= m_AxeScale;
				m_BottomRight.y	-= oldGrid.y;
				setAxeLength(m_AxeLength);
			}
			break;

		case Graph::GAxe::ALL:
		{
			//Положение оси по высоте округлим до сетки
			float	step	= oldGrid.y;
			if(m_DataType == Bool)		step	= 0.5f*step;
			if(mdf & Qt::AltModifier)	m_BottomRight.y	= m_FrameBR.y;
			else						m_BottomRight.y	= int((m_FrameBR.y - oldAreaBL.y)/step + 0.5f)*step + oldAreaBL.y;
		}break;

		default:
			break;
	}*/
}

void	GAxe::onWheel(const vec2& pt, const Qt::KeyboardModifiers& mdf, vec2 numdegrees)
{
	//Обработка колеса
	if(mdf & Qt::ShiftModifier)
	{
		//Меняем начало
		if(m_Data_Type != DataType::Bool)
		{
			m_AxeMin -= int(numdegrees.y/120.)*m_AxeScale;
			setAxeLength(m_Axe_Length);
		}
	}
	else if(mdf & Qt::ControlModifier)
	{
		//Проверяем, что по высоте мышь находится на оси
		if(pt.y < m_FrameBR.y + oldAreaBL.y +oldGrid.y*m_Axe_Length+1 && pt.y > m_FrameBR.y-1 && m_Data_Type != DataType::Bool)
		{
			double Power	= floor(log10(m_AxeScale));
			double Mantiss	= m_AxeScale / pow(10., Power);

			//Изменяем масштаб в нужную сторону
			double	Scale;
			if(numdegrees.y > 0)
			{
				if(Mantiss == 1)		Scale	= 0.5*pow(10., Power);
				else if(Mantiss <= 2)	Scale	= 1*pow(10., Power);
				else if(Mantiss <= 5)	Scale	= 2*pow(10., Power);
				else					Scale	= 5*pow(10., Power);
			}
			else
			{
				if(Mantiss == 1)		Scale	= 2*pow(10., Power);
				else if(Mantiss <= 2)	Scale	= 5*pow(10., Power);
				else if(Mantiss <= 5)	Scale	= 10*pow(10., Power);
				else					Scale	= 20*pow(10., Power);
			}

			//Сохраняем значение в зависимости от того, где стоит курсор
			int	index	= int((pt.y - oldAreaBL.y - m_BottomRight.y)/oldGrid.y + 0.5);
			m_AxeMin	= m_AxeMin + index*(m_AxeScale - Scale);
			m_AxeScale	= Scale;
			setAxeLength(m_Axe_Length, index);
		}
	}
}

void	GAxe::GetLimits(double* pMin, double* pMax)
{
	if(m_data.empty())
	{
		*pMax = 0;
		*pMin = 0;
		return;
	}

	*pMin	= *pMax	= m_data.front().y;
	for(size_t i = 0; i < m_data.size(); i++)
	{
		double	f	= m_data.at(i).y;
		if(f < *pMin)	*pMin	= f;
		if(f > *pMax)	*pMax	= f;
	}
}

void	GAxe::GetLimits(double /*t0*/, double /*t1*/, double* /*pMin*/, double* /*pMax*/)
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

void	GAxe::fitToScale(double t0 /* = 0 */, double t1 /* = 0 */)
{
	//Найдем минимум и максимум
	double	Min;
	double	Max;

	if(t0 == 0 && t1 == 0)
		GetLimits(&Min, &Max);
	else
		GetLimits(t0, t1, &Min, &Max);

	//Зная минимум и максимум, определим диапазон
	double Step = (Max - Min)/m_Axe_Length;
	
	//Округлим его до нормализованного значения
	if(Step)
	{
		double Power	= floor(log10(Step));
		double Mantiss	= Step / pow(10., Power);
		
		if(Mantiss == 1)		m_AxeScale	= 1*pow(10.,Power);
		else if(Mantiss <= 2)	m_AxeScale	= 2*pow(10.,Power);
		else if(Mantiss <= 5)	m_AxeScale	= 5*pow(10.,Power);
		else					m_AxeScale	= 10*pow(10.,Power);
		
		//Итак, получили масштаб. Считаем минимальное значение
		m_AxeMin = floor(Min/m_AxeScale)*m_AxeScale;		
	}
	else
	{
		m_AxeScale	= 1;
		m_AxeMin	= Min - m_AxeScale;
		m_Axe_Length	= 2;
	}

	if(m_Data_Type == DataType::Bool)
	{
		m_AxeMin	= 0;
		m_AxeScale	= 1;
		m_Axe_Length= 1;
	}
	setAxeLength(m_Axe_Length);
}

void	GAxe::uploadData(size_t size, const double* pTime, const char* pData, const DataType nType)
{
	if(!size)	
	{
		m_data.clear();
		return;
	}

	m_Data_Length	= size;
	m_Data_Type		= nType;

	//Принудительно для СРК выставляем признак
	if(m_bSRK)	
	{
		m_Data_Type		= DataType::Bool;
		m_MaskSRK		= 0x1 << (m_nBitSRK-1);				
	}

	if(m_Data_Type == DataType::Bool)
	{
		setAxeLength(1);
		m_AxeMin	= 0;
		m_AxeScale	= 1;
	}

	//Копируем данные
	m_data.clear();
    for(size_t i = 0; i < m_Data_Length; i++)
	{
		switch(m_Data_Type)
		{
		case DataType::Bool:	m_data.push_back(vec2(pTime[i], (float)(*((bool*)pData + i))));break;
		case DataType::Int:		m_data.push_back(vec2(pTime[i], (float)(*((int*)pData + i))));break;
		case DataType::Double:	m_data.push_back(vec2(pTime[i], (float)(*((double*)pData + i))));break;
		case DataType::Float:	m_data.push_back(vec2(pTime[i], (float)(*((float*)pData + i))));break;
		default:
			break;
		}
	}

	UpdateFiltering();
	return;
}

void	GAxe::clearData()
{
	m_data.clear();
}

double	GAxe::GetValueAtTime(const double Time) const
{
	//Получаем значение на заданный момент времени
	if(m_data.size() == 0)			return 0;
	if(Time <= m_data.front().x)	return m_data.front().y;
	if(Time >= m_data.back().x)		return m_data.back().y;

	//Ищем индекс в данных
	size_t	nMin	= 0;
	size_t	nMax	= m_data.size()-1;
	while(nMax - nMin > 1)
	{
		size_t n	= (nMin+nMax)/2;
		if(m_data.at(n).x <= Time)	nMin	= n;
		else						nMax	= n;
	}

	int	nStartIndex	= max(0, int(nMin));
	int	nStopIndex	= min(int(m_data.size()-1), int(nMax));

	//При необходимости интерполируем
	double	f1	= m_data.at(nStartIndex).y;	double	t1	= m_data.at(nStartIndex).x;
	double	f2	= m_data.at(nStopIndex).y;	double	t2	= m_data.at(nStopIndex).x;

	if(m_bInterpol && (m_Data_Type == DataType::Double || m_Data_Type == DataType::Float))
		return	f1+(f2-f1)/(t2-t1)*(Time-t1);
	else			
		return	f1;
}

double GAxe::GetTopPosition() const
{
	return m_BottomRight.y + m_Axe_Length*oldGrid.y;
}

bool	GAxe::IsBoolean() const
{
	return m_Data_Type	== DataType::Bool;
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

void	GAxe::getTime(double& t0, double& t1)
{
	if(!m_data.size())
	{
		t0	= 0;
		t1	= 0;
		return;
	}
	t0	= m_data.front().x;
	t1	= m_data.back().x;
}

void	GAxe::onResize(int width, int height)
{
	//Сохраняем новые размеры
	fboWidth	= width;
	fboHeight	= height;

	//Создаем framebuffer
	if(fbo)
	{
		//Очищаем имеюшийся буфер
		glDeleteTextures(1, &fboTexture);
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);
	}

	GLuint	curFBO	= (GL_FRAMEBUFFER_BINDING);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	{
		//Текстурное прикрепление
		glGenTextures(1, &fboTexture);
		glBindTexture(GL_TEXTURE_2D, fboTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			qDebug() << "Framebuffer ERROR!";
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//GLenum	err	= glCheckFramebufferStatus(GL_FRAMEBUFFER);
		//if(err != GL_FRAMEBUFFER_COMPLETE)
		//	qDebug() << "Framebuffer ERROR!";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, curFBO);
}

}
