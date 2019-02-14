#include "stdafx.h"
#include "GTextLabel.h"
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

	if(!bFontLoaded)
		loadFontInfo();
}

void	GTextLabel::loadFontInfo()
{
	bFontLoaded	= true;

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

void	GTextLabel::setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
{
	textShader->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &proj[0][0]);
	textShader->release();
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

		// Prepare texture
		glGenTextures(1, &texture);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 96, 96, 127);
		for(int code = 0x0020; code < 0x007F; code++)
		{
			QImage	ch	= QImage(QString(":/Resources/fonts/arialN/%1.png").arg(code));
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, code-0x0020, 96, 96, 1, GL_RGBA, GL_UNSIGNED_BYTE, ch.bits());
		}

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	//Выбираем шрифт
	FontInfo*	font	= fonts.at(fontIndex);
	for(int i = 0; i < str.length(); i++)
	{
		//Получаем информацию о символе
		const QChar		c		= str.at(i);
		const CharInfo&	info2	= font->charMap.at(c.unicode());
		CharInfo	info	= info2;
		info.tex	= ivec2(0);
		info.size	= vec2(2.f*scale);
		info.offset	= ivec2(0);
		info.origSize	= info.size;
		
		//Создаем два треугольника. Координаты в миллиметрах документа!
		Data	data;
		data.text.z	= c.unicode()-0x0020;
		if(data.text.z > 126)	data.text.z	= 126;
		
		//Левый верхний
		data.point.x	= x + info.offset.x;
		data.point.y	= y + (info.origSize.y - info.offset.y);
		data.text.x		= 0;
		data.text.y		= 0;
		m_data.push_back(data);

		//Левый нижний
		data.point.x	= x + info.offset.x;
		data.point.y	= y + (info.origSize.y - info.offset.y - info.size.y);
		data.text.x		= 0;
		data.text.y		= 1;
		m_data.push_back(data);

		//Правый верхний
		data.point.x	= x + (info.offset.x + info.size.x);
		data.point.y	= y + (info.origSize.y - info.offset.y);
		data.text.x		= 1;
		data.text.y		= 0;
		m_data.push_back(data);
		m_data.push_back(data);

		//Правый нижний
		data.point.x	= x + (info.offset.x + info.size.x);
		data.point.y	= y + (info.origSize.y - info.offset.y - info.size.y);
		data.text.x		= 1;
		data.text.y		= 1;
		m_data.push_back(data);

		//Левый нижний
		data.point.x	= x + info.offset.x;
		data.point.y	= y + (info.origSize.y - info.offset.y - info.size.y);
		data.text.x		= 0;
		data.text.y		= 1;
		m_data.push_back(data);

		//Продвигаемся на символ дальше
		x += (info.origSize.x)*0.4;
	}
}

void	GTextLabel::prepare()
{
	//Заливаем данные в буфер
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(Data), m_data.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	GTextLabel::renderText(vec3 color, float alpha)
{
	textShader->bind();
	glUniform3f(u_color, color.r, color.g, color.b);
	
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));

	// Render glyph texture over quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	textShader->release();
}

void	GTextLabel::setFont(int size, GLfloat scale)
{
	//Устанавливаем цвет
	this->scale	= scale;

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
		size.x += info.origSize.x+0;

		if(info.origSize.y > size.y)	size.y	= info.origSize.y;
	}

	return	size/scale;
}

GLfloat	GTextLabel::baseLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return (info.origSize.y - info.offset.y - info.size.y)/scale;
}

GLfloat	GTextLabel::midLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return ((float)info.origSize.y - (float)info.offset.y - (float)info.size.y*0.5)/scale;
}

GLfloat	GTextLabel::topLine()
{
	//Выбираем шрифт
	FontInfo*		font	= fonts.at(fontIndex);
	const CharInfo&	info	= font->charMap.at('0');

	return (info.origSize.y - info.offset.y)/scale;
}

}