#include "stdafx.h"
#include "GTextLabel.h"
#include <QDomDocument>

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
	scale		= 3.5f;
	fontIndex	= 0;
	textVAO		= 0;
	textVBO		= 0;

	if(!bFontLoaded)
		loadFontInfo();
}

void	GTextLabel::loadFontInfo()
{
	bFontLoaded	= true;

	//Читаем описатель шрифта
	QFile file(":/Resources/fonts/arial.xml");
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(nullptr, "Загрузка шрифта", QString("Cannot read file %1:\n%2.").arg(":/Resources/fonts/courier.xml").arg(file.errorString()));
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
	for(int f = 0; f < fonts.size(); f++)
		delete fonts.at(f);
	fonts.clear();

	for(QDomElement n = root.firstChildElement("font"); !n.isNull(); n = n.nextSiblingElement("font"))
	{
		//Читаем шрифт
		FontInfo*	font	= new FontInfo;
		font->name	= n.attribute("name");
		font->size	= n.attribute("size").remove("px").toInt();
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
	for(int f = 0; f < fonts.size(); f++)
		delete fonts.at(f);
	fonts.clear();

	delete textShader;
}

void	GTextLabel::setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
{
	textShader->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &proj[0][0]);
	textShader->release();

	//scale	= 1./(view*glm::vec4(1,0,0,0)).x;
}

void	GTextLabel::initializeGL()
{
	if(!bTextureLoaded)
	{
//		initializeOpenGLFunctions();
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
		QOpenGLTexture *gl_texture = new QOpenGLTexture(QImage(":/Resources/fonts/arial.png"));
		texSize.x	= gl_texture->width();
		texSize.y	= gl_texture->height();
		texture	= gl_texture->textureId();

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_data.clear();
}

void	GTextLabel::clearGL()
{
	if(textVAO)	{glDeleteVertexArrays(1, &textVAO); textVAO = 0;}
	if(textVBO)	{glDeleteBuffers(1, &textVBO); textVBO = 0;}
}

void	GTextLabel::addString(QString str, GLfloat x, GLfloat y)
{
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
		point.z	= info.tex.x/(float)texSize.x;
		point.w	= info.tex.y/(float)texSize.y;
		m_data.push_back(point);

		//Левый нижний
		point.x	= x + info.offset.x/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= info.tex.x/(float)texSize.x;
		point.w	= (info.tex.y + info.size.y)/(float)texSize.y;
		m_data.push_back(point);

		//Правый верхний
		point.x	= x + (info.offset.x + info.size.x)/scale;
		point.y	= y + (info.origSize.y - info.offset.y)/scale;
		point.z	= (info.tex.x + info.size.x)/(float)texSize.x;
		point.w	= info.tex.y/(float)texSize.y;
		m_data.push_back(point);
		m_data.push_back(point);

		//Правый нижний
		point.x	= x + (info.offset.x + info.size.x)/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= (info.tex.x + info.size.x)/(float)texSize.x;
		point.w	= (info.tex.y + info.size.y)/(float)texSize.y;
		m_data.push_back(point);

		//Левый нижний
		point.x	= x + info.offset.x/scale;
		point.y	= y + (info.origSize.y - info.offset.y - info.size.y)/scale;
		point.z	= info.tex.x/(float)texSize.x;
		point.w	= (info.tex.y + info.size.y)/(float)texSize.y;
		m_data.push_back(point);

		//Продвигаемся на символ дальше
		x += (info.origSize.x +0)/scale;
	}
}

void	GTextLabel::prepare()
{
	//Заливаем данные в буфер
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, m_data.size()*sizeof(vec4), m_data.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	GTextLabel::renderText()
{
	// Activate corresponding render state	
//	glEnable(GL_MULTISAMPLE);
	textShader->bind();
	glUniform3f(u_color, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// Render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	textShader->release();
//	glDisable(GL_MULTISAMPLE);
}

void	GTextLabel::setFont(int size, vec3 color)
{
	//Устанавливаем цвет
	this->color	= color;

	//Подбираем наиболее подходящий шрифт
	for(int i = 0; i < fonts.size(); i++)
	{
		FontInfo*	f	= fonts.at(i);
		if(f->size >= size)
		{
			fontIndex	= i;
			break;
		}
	}
}

vec2	GTextLabel::textSize(const QString& str)
{
	vec2	size(0.0f);
	if(str.isEmpty())	return vec2(0.0f);
	
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
		size.x += info.origSize.x;

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
