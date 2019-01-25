#ifndef GTEXTLABEL_H
#define GTEXTLABEL_H

#include <glm/glm.hpp>
using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
#include <vector>
#include <map>
using std::vector;
using std::map;

class QOpenGLShaderProgram;

class GTextLabel
{
private:
	// Holds all state information relevant to a character as loaded using FreeType
	struct CharInfo{
		int		id;			//Код символа в UNICODE
		ivec2	tex;		//Координаты в текстуре
		ivec2	size;		//Размеры символа
		ivec2	offset;		//Смещение символа относительно оригинала
		ivec2	origSize;	//Исходный размер символа
	};

	struct FontInfo
	{
		QString		name;	//Название шрифта
		int			size;	//Размер в пикселях
		map<int, CharInfo>	charMap;	//Описатель шрифта
	};

	//Общие данные
	static	bool				bFontLoaded;
	static	vector<FontInfo*>	fonts;
	static	bool				bTextureLoaded;
	static	GLuint				texture;
	static	ivec2				texSize;
	static	QOpenGLShaderProgram*	textShader;
	static	int					u_modelToWorld;
	static	int					u_worldToCamera;
	static	int					u_cameraToView;
	static	int					u_color;

	//Данные для каждого объекта
	vector<vec4>	m_data;

	GLuint	textVAO, textVBO;
	glm::vec3	color;
	int		fontIndex;
	GLfloat	scale;

	void	loadFontInfo();

public:
	GTextLabel();
	~GTextLabel();
	void	initializeGL();
	void	clearGL();
	void	setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj);
	void	addString(QString str, GLfloat x, GLfloat y);
	void	prepare();
	void	setFont(int size, vec3 color);
	void	renderText();
	vec2	textSize(const QString& str);
	GLfloat	baseLine();
	GLfloat	midLine();
	GLfloat	topLine();
};

#endif // GTEXTLABEL_H
