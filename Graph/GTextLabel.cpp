#include "stdafx.h"
#include "GTextLabel.h"
#include "GraphObject.h"
#include <QDomDocument>

namespace Graph{

bool	GTextLabel::bFontLoaded	= false;
vector<GTextLabel::FontInfo*>	GTextLabel::fonts;

bool	GTextLabel::bTextureLoaded	= false;
GLuint	GTextLabel::texture;
ivec2	GTextLabel::texSize;

QOpenGLShaderProgram*	GTextLabel::textShader;
int					GTextLabel::u_modelToWorld;
int					GTextLabel::u_worldToCamera;
int					GTextLabel::u_cameraToView;
int					GTextLabel::u_color;
int					GTextLabel::u_alpha;

GTextLabel::GTextLabel()
{
	fontIndex	= 0;
	textVBO		= 0;
	m_model		= mat4(1.f);

	if(!bFontLoaded)
		loadFontInfo();
}

void	GTextLabel::loadFontInfo()
{
	bFontLoaded	= true;

	//Читаем описатель шрифта
	QFile file(":/Resources/fonts/arialn.xml");
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("Cannot read file %1:\n%2.").arg(":/Resources/fonts/arialn.xml").arg(file.errorString()));
		return;
	}

	QDomDocument xml;

	QString errorStr;
	int errorLine;
	int errorColumn;
	if(!xml.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("%1\nParse error at line %2, column %3:\n%4").arg(":/Resources/fonts/arialn.xml").arg(errorLine).arg(errorColumn).arg(errorStr));
		return;
	}

	//Проверяем тип файла
	QDomElement root = xml.documentElement();
	if(root.tagName() != "fontList")
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", "В файле должен быть список шрифтов!");
		return;
	}

	//Сканируем шрифты
    for(size_t f = 0; f < fonts.size(); f++)
		delete fonts.at(f);
	fonts.clear();

	for(QDomElement n = root.firstChildElement("font"); !n.isNull(); n = n.nextSiblingElement("font"))
	{
		//Читаем шрифт
		FontInfo*	font	= new FontInfo;
		font->name	= n.attribute("name");
		font->size	= n.attribute("size").remove("pt").toInt();
		for(QDomElement c = n.firstChildElement("char"); !c.isNull(); c = c.nextSiblingElement("char"))
		{
			//Читаем описание символа
			CharInfo	info;
			info.id			= c.attribute("id").toInt();
			info.tex.x		= c.attribute("x").toInt();
			info.tex.y		= c.attribute("y").toInt();
			info.size.x		= c.attribute("width").toInt();
			info.size.y		= c.attribute("height").toInt();
			info.offset.x	= c.attribute("Xoffset").toInt();
			info.offset.y	= c.attribute("Yoffset").toInt();
			info.origSize.x	= c.attribute("OrigWidth").toInt();
			info.origSize.y	= c.attribute("OrigHeight").toInt();
			
			font->charMap.insert(std::pair<int, CharInfo>(info.id, info));
		}
		fonts.push_back(font);
	}
}

GTextLabel::~GTextLabel()
{
    clearGL();
	if(textVBO)	{glDeleteBuffers(1, &textVBO); textVBO = 0;}
}

void	GTextLabel::finalDelete()
{
	//Удаляем шрифты
	if(bFontLoaded)
	{
		for(size_t f = 0; f < fonts.size(); f++)
			delete fonts.at(f);
		fonts.clear();
	}

	//Чистим текстуру
	if(bTextureLoaded)
		glDeleteTextures(1, &texture);

	//И программу
	if(textShader)	delete textShader;
}

void	GTextLabel::setMatrix(glm::mat4 model)
{
	if(m_model != model)
	{
		//Сохраняем новую матрицу положения надписи
		m_model	= model;

		//Пересчитываем координаты точек для попадания в пиксельную сетку
	}
}

void	GTextLabel::initializeGL()
{
	if(!bTextureLoaded)
	{
		bTextureLoaded	= true;

		textShader = new QOpenGLShaderProgram();
		textShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/textlabel.vert");
		textShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/textlabel.frag");
		textShader->link();

		textShader->bind();
		u_modelToWorld	= textShader->uniformLocation("modelToWorld");
		u_worldToCamera	= textShader->uniformLocation("worldToCamera");
		u_cameraToView	= textShader->uniformLocation("cameraToView");
		u_color			= textShader->uniformLocation("textColor");
		u_alpha			= textShader->uniformLocation("alpha");
		textShader->release();

		// Prepare texture
		QImage	im(":/Resources/fonts/arialn.png");
		texSize.x	= im.width();
		texSize.y	= im.height();

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.width(), im.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im.bits());

		// Disable byte-alignment restriction
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate texture
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Configure VAO/VBO for texture quads
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_data.clear();
}

void	GTextLabel::clearGL()
{
	if(textVBO)	{ glDeleteBuffers(1, &textVBO); textVBO = 0; }
}

void	GTextLabel::addString(QString str, GLfloat x, GLfloat y)
{
	if(str == "Vh_SNP")
	{
		int a = 0;
	}
	float	scale	= GraphObject::m_scale;

	//Выбираем шрифт
	FontInfo*	font	= fonts.at(fontIndex);
	for(int i = 0; i < str.length(); i++)
	{
		//Получаем информацию о символе
		const QChar		c		= str.at(i);
		const CharInfo&	info	= font->charMap.at(c.unicode());
		
		//Создаем два треугольника. Координаты в миллиметрах документа!
		vec4	point;
		
		//Левый верхний
		point.x	= x + info.offset.x/scale;
		point.y	= y + (info.origSize.y - info.offset.y)/scale;
		point.z	= info.tex.x/(float)(texSize.x-0);
		point.w	= info.tex.y/(float)(texSize.y-0);
		m_data.push_back(point);

		//Левый нижний
		point.x	= x + info.offset.x/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= info.tex.x/(float)(texSize.x-0);
		point.w	= (info.tex.y + info.size.y)/(float)(texSize.y-0);
		m_data.push_back(point);

		//Правый верхний
		point.x	= x + (info.offset.x + info.size.x)/scale;
		point.y	= y + (info.origSize.y - info.offset.y)/scale;
		point.z	= (info.tex.x + info.size.x)/(float)(texSize.x-0);
		point.w	= info.tex.y/(float)(texSize.y-0);
		m_data.push_back(point);
		m_data.push_back(point);

		//Правый нижний
		point.x	= x + (info.offset.x + info.size.x)/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= (info.tex.x + info.size.x)/(float)(texSize.x-0);
		point.w	= (info.tex.y + info.size.y)/(float)(texSize.y-0);
		m_data.push_back(point);

		//Левый нижний
		point.x	= x + info.offset.x/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= info.tex.x/(float)(texSize.x-0);
		point.w	= (info.tex.y + info.size.y)/(float)(texSize.y-0);
		m_data.push_back(point);

		//Продвигаемся на символ дальше
		x += (info.origSize.x + 1)/scale;
	}
}

void	GTextLabel::prepare()
{
	//Заливаем данные в буфер
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(vec4), m_data.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	GTextLabel::renderText(vec3 color, float alpha)
{
	// Activate corresponding render state	
//	glEnable(GL_MULTISAMPLE);
	textShader->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &m_model[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &GraphObject::m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &GraphObject::m_proj[0][0]);

	glUniform3f(u_color, color.r, color.g, color.b);
	glUniform1f(u_alpha, 1.0f);//alpha);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// Render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	textShader->release();
//	glDisable(GL_MULTISAMPLE);
}

void	GTextLabel::setFont(int size)
{
	//Подбираем наиболее подходящий шрифт
	for(int i = 0; i < fonts.size(); i++)
	{
		FontInfo*	f	= fonts.at(i);
		if(f->size >= size)
		{
			fontIndex	= i;
			return;
		}
	}

	//Берем самый крупный
	fontIndex	= std::max(0, (int)(fonts.size()-1));
}

vec2	GTextLabel::textSize(const QString& str)
{
	vec2	size(0.0f);
	if(str.isEmpty())	return vec2(0.0f);
	if(fonts.empty())	return vec2(0.0f);
	
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	for(int i = 0; i < str.length(); i++)
	{
		//Получаем информацию о символе
		const QChar		c		= str.at(i);
		const CharInfo&	info	= font->charMap.at(c.unicode());

		//Для первого и последнего символа вычтем смещение
		if(i == 0)	size.x	= -info.offset.x;
		//if(i == str.length()-1)	size.x -= (info.origSize.x - info.offset.x - info.size.x);
		
		//Продвигаемся на символ дальше
		size.x += info.origSize.x+1;

		if(info.origSize.y > size.y)	size.y	= info.origSize.y;
	}

	return	size/GraphObject::m_scale;
}

GLfloat	GTextLabel::baseLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return (info.origSize.y - info.offset.y - info.size.y)/GraphObject::m_scale;
}

GLfloat	GTextLabel::midLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return ((float)info.origSize.y - (float)info.offset.y - (float)info.size.y*0.5)/GraphObject::m_scale;
}

GLfloat	GTextLabel::topLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return (info.origSize.y - info.offset.y)/GraphObject::m_scale;
}

}