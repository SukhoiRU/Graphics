#include "stdafx.h"
#include "GTextLabel.h"
#include "GraphObject.h"
#include <QDomDocument>

#include <ft2build.h>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

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

GTextLabel::GTextLabel()
{
	fontIndex	= 0;
	textVBO		= 0;
	m_model		= mat4(1.0f);
	font		= 0;

	//if(!bFontLoaded)
	//	loadFontInfo();
}

void	GTextLabel::loadFontInfo()
{
	bFontLoaded	= true;
/*
	//Читаем описатель шрифта
	QFile file(":/Resources/fonts/arial_new.xml");
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("Cannot read file %1:\n%2.").arg(":/Resources/fonts/arial.xml").arg(file.errorString()));
		return;
	}

	QDomDocument xml;

	QString errorStr;
	int errorLine;
	int errorColumn;
	if(!xml.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("%1\nParse error at line %2, column %3:\n%4").arg(":/Resources/fonts/courier.xml").arg(errorLine).arg(errorColumn).arg(errorStr));
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
			
//			CharInfo	info	= info2;
			info.tex	= ivec2(0);
			info.size	= vec2(6.5);
			info.offset	= vec2(0, 2.);
			info.origSize	= info.size;

			font->charMap.insert(std::pair<int, CharInfo>(info.id, info));
		}
		fonts.push_back(font);
	}
*/
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
		m_model    = model;
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
		textShader->release();

		//Подключаем FreeType
		FT_Library library;
		FT_Error error = FT_Init_FreeType(&library);
		FT_Face face;
		error = FT_New_Face(library, "Resources\\fonts\\arialN\\arialN.ttf", 0, &face);
		fonts.clear();
		FontInfo*	font	= new FontInfo;
		font->name		= face->family_name;

		//Загружаем текстурный массив
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

		//Определяем размер текстур по символу 'X'
		QImage	ch1	= QImage(QString(":/Resources/fonts/arialN/88.png"));
		error	= FT_Load_Char(face, 88, FT_LOAD_NO_SCALE);
		int	w	= ch1.width();
		int	h	= ch1.height();
		font->pxrange	= 12;
		font->texSize	= w;
		font->ascender	= face->ascender/2048.;
		font->descender	= face->descender/2048.;
		font->midline	= 0.5f*face->glyph->metrics.height/2048.;
		font->bbox_min	= vec2(face->bbox.xMin/2048.f, face->bbox.yMin/2048.f);
		font->bbox_max	= vec2(face->bbox.xMax/2048.f, face->bbox.yMax/2048.f);

		//Выделяем под текстуру память
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, w, h, 171, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
		int layer	= 0;
		int code	= 0;
		while(getNextCode(code))
		{
			//Загружаем символ с контролем формата
			QImage	ch	= QImage(QString(":/Resources/fonts/arialN/%1.png").arg(code));
			if(ch.format() != QImage::Format_RGB32)
				ch	= ch.convertToFormat(QImage::Format_RGB32);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, ch.bits());

			//Создаем описание символа
			CharInfo	info;
			FT_Error error = FT_Load_Char(face, code, FT_LOAD_NO_SCALE);
			info.unicode	= code;
			info.layer		= layer;
			info.size.x		= face->glyph->metrics.width/2048.;
			info.size.y		= face->glyph->metrics.height/2048.;
			info.offset.x	= face->glyph->metrics.horiBearingX/2048.;
			info.offset.y	= face->glyph->metrics.horiBearingY/2048.;
			info.advance	= face->glyph->metrics.horiAdvance/2048.;
			font->charMap.insert(std::pair<int, CharInfo>(code, info));

			layer++;
		}
		fonts.push_back(font);

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Configure VAO/VBO for texture quads
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_data.clear();
}

bool	GTextLabel::getNextCode(int& code)
{
	if(code < 32)			{ code = 32; return true; }
	else if(code < 127)		{ code++; return true; }
	else if(code == 127)	{ code = 169; return true; }
	else if(code == 169)	{ code = 171; return true; }
	else if(code == 171)	{ code = 174; return true; }
	else if(code == 174)	{ code = 187; return true; }
	else if(code == 187)	{ code = 1025; return true; }
	else if(code == 1025)	{ code = 1040; return true; }
	else if(code < 1105)	{ code++; return true;}
	else if(code == 1105)	{ code = 8211; return true;}
	else if(code == 8211)	{ code = 8212; return true;}
	else if(code == 8212)	{ code = 8220; return true;}
	else if(code == 8220)	{ code = 8221; return true;}
	else
		return false;
}

void	GTextLabel::clearGL()
{
	if(textVBO)	{ glDeleteBuffers(1, &textVBO); textVBO = 0; }
}

void	GTextLabel::addString(QString str, GLfloat x, GLfloat y)
{
	str	= str.trimmed();
	for(int i = 0; i < str.length(); i++)
	{
		//Получаем информацию о символе
		int code	= str.at(i).unicode();
		auto	it	= font->charMap.find(code);
		if(it == font->charMap.end())
			code	= 127;

		CharInfo	info	= font->charMap.at(code);
		float		texSize	= (std::max(info.size.x, info.size.y)*fontSize)*((float)(font->texSize + 3*font->pxrange))/(float)font->texSize;
		vec2		center	= vec2(x, y) + fontSize*vec2(info.offset.x + 0.5f*info.size.x, info.offset.y - 0.5*info.size.y);

		//Создаем два треугольника. Координаты в миллиметрах документа!
		Data	data;
		data.text.z	= info.layer;
		data.corr.x	= 1.;
		data.corr.y	= 0;

		//Левый верхний
		data.point.x	= center.x - 0.5*texSize;
		data.point.y	= center.y + 0.5*texSize;
		data.text.x		= 0;
		data.text.y		= 0;
		m_data.push_back(data);

		//Левый нижний
		data.point.x	= center.x - 0.5*texSize;
		data.point.y	= center.y - 0.5*texSize;
		data.text.x		= 0;
		data.text.y		= 1;
		m_data.push_back(data);

		//Правый верхний
		data.point.x	= center.x + 0.5*texSize;
		data.point.y	= center.y + 0.5*texSize;
		data.text.x		= 1;
		data.text.y		= 0;
		m_data.push_back(data);
		m_data.push_back(data);

		//Правый нижний
		data.point.x	= center.x + 0.5*texSize;
		data.point.y	= center.y - 0.5*texSize;
		data.text.x		= 1;
		data.text.y		= 1;
		m_data.push_back(data);

		//Левый нижний
		data.point.x	= center.x - 0.5*texSize;
		data.point.y	= center.y - 0.5*texSize;
		data.text.x		= 0;
		data.text.y		= 1;
		m_data.push_back(data);

		//Продвигаемся на символ дальше
		x += info.advance*fontSize;
	}
}

void	GTextLabel::prepare()
{
	//Заливаем данные в буфер
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(Data), m_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	GTextLabel::renderText(vec3 color, float alpha)
{
	textShader->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &m_model[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &GraphObject::m_view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &GraphObject::m_proj[0][0]);

	glUniform3f(u_color, color.r, color.g, color.b);
	glUniform1f(textShader->uniformLocation("pxRange"), 12.0f);	//Тут надо бы из шрифта брать...
	
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));

	// Render glyph texture over quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	textShader->release();
}

void	GTextLabel::setFont(GLfloat size)
{
	fontIndex	= 0;
	FontInfo*	f	= fonts.at(fontIndex);
	fontSize	= size;
	font		= f;
}

vec2	GTextLabel::textSize(const QString& str1)
{
	if(str1.isEmpty())	return vec2(0.0f);
	if(fonts.empty())	return vec2(0.0f);
	if(!font)			return vec2(0.0f);

	QString	str	= str1.trimmed();
	GLfloat	x	= 0;
	for(int i = 0; i < str.length(); i++)
	{
		//Получаем информацию о символе
		int code	= str.at(i).unicode();
		auto	it	= font->charMap.find(code);
		if(it == font->charMap.end())
			code	= 127;

		const CharInfo&	info	= font->charMap.at(code);
		x += info.advance;
	}
	return vec2(x*fontSize, (font->ascender)*fontSize);
}

GLfloat	GTextLabel::midLine()
{
	return font->midline*fontSize;
}

GLfloat	GTextLabel::topLine()
{
	return font->ascender*fontSize;
}

GLfloat	GTextLabel::bottomLine()
{
	return font->descender*fontSize;
}

void	GTextLabel::bBox(vec2& min, vec2& max)
{
	min	= font->bbox_min*fontSize;
	max	= font->bbox_max*fontSize;
}

}