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

GTextLabel::GTextLabel()
{
	fontIndex	= 0;
	textVBO		= 0;
	m_model		= mat4(1.0f);
	font		= 0;
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

		loadFontInfo();
	}

	// Configure VAO/VBO for texture quads
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_data.clear();
}

void	GTextLabel::loadFontInfo()
{
	bFontLoaded	= true;

	//Читаем описатель шрифта
	QFile file(":/Resources/fonts/arialN.xml");
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("Cannot read file %1:\n%2.").arg(":/Resources/fonts/arialN.xml").arg(file.errorString()));
		return;
	}

	QDomDocument xml;

	QString errorStr;
	int errorLine;
	int errorColumn;
	if(!xml.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("%1\nParse error at line %2, column %3:\n%4").arg(":/Resources/fonts/arialN.xml").arg(errorLine).arg(errorColumn).arg(errorStr));
		return;
	}

	//Проверяем тип файла
	QDomElement root = xml.documentElement();
	if(root.tagName() != "fontInfo")
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", "В файле должен быть список шрифтов!");
		return;
	}

	QDomElement	n	= root.firstChildElement("Font");

	//Очищаем шрифты
	for(size_t f = 0; f < fonts.size(); f++)
		delete fonts.at(f);
	fonts.clear();

	//Читаем шрифт
	FontInfo*	font	= new FontInfo;
	font->name		= n.attribute("name");
	font->pxrange	= n.attribute("pxrange").toInt();
	font->texSize	= n.attribute("texSize").toInt();
	font->ascender	= n.attribute("ascender").toInt()/2048.;
	font->descender	= n.attribute("descender").toInt()/2048.;
	font->midline	= n.attribute("midline").toInt()/2048.;

	int	count	= n.childNodes().count();
	int	w	= font->texSize;
	int	h	= font->texSize;

	//Загружаем текстурный массив
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	//Выделяем под текстуру память
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, w, h, count, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	int layer	= 0;
	for(QDomElement c = n.firstChildElement("char"); !c.isNull(); c = c.nextSiblingElement("char"))
	{
		//Создаем описание символа
		CharInfo	info;
		info.unicode	= c.attribute("unicode").toInt();
		info.layer		= layer;
		info.size.x		= c.attribute("width").toInt()/2048.;
		info.size.y		= c.attribute("height").toInt()/2048.;
		info.offset.x	= c.attribute("horiBearingX").toInt()/2048.;
		info.offset.y	= c.attribute("horiBearingY").toInt()/2048.;
		info.advance	= c.attribute("horiAdvance").toInt()/2048.;
		font->charMap.insert(std::pair<int, CharInfo>(info.unicode, info));

		//Загружаем символ с контролем формата
		QString		imgBase64	= c.attribute("texture");
		QByteArray	imgArray;
		QBuffer		imgBuffer(&imgArray);
		imgBuffer.open(QIODevice::WriteOnly);
		QDataStream	in(&imgBuffer);
		in << imgBase64;
		imgBuffer.close();

		QImage	ch;
		ch.loadFromData(QByteArray::fromBase64(imgArray), "PNG");
		if(ch.format() != QImage::Format_RGB32)
			ch	= ch.convertToFormat(QImage::Format_RGB32);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, ch.bits());

		layer++;

	}
	fonts.push_back(font);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
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
		CharInfo	info;
		map<int, CharInfo>::iterator	it	= font->charMap.find(str.at(i).unicode());
		if(it == font->charMap.end())	info	= font->charMap.at(126);
		else							info	= it->second;

		float		texSize	= (std::max(info.size.x, info.size.y)*fontSize)*((float)(font->texSize + 3*font->pxrange))/(float)font->texSize;
		vec2		center	= vec2(x, y) + fontSize*vec2(info.offset.x + 0.5f*info.size.x, info.offset.y - 0.5*info.size.y);

		//Создаем два треугольника. Координаты в миллиметрах документа!
		Data	data;
		data.text.z	= info.layer;
		data.corr.x	= 1.f;
		data.corr.y	= -0.1f;

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

void	GTextLabel::renderText(vec3 color, float /*alpha*/)
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
		map<int, CharInfo>::iterator	it	= font->charMap.find(code);
		if(it == font->charMap.end())
			code	= 126;

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

}
