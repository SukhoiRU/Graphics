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
	vector<FontInfo*>	fonts;

	struct VertexInfo
	{
		vec2	pt0;
	};
	vector<vec4>	m_data;

	QOpenGLShaderProgram*	textShader;
	GLuint	textVAO, textVBO;
	GLuint	texture;
	ivec2	texSize;
	int		u_modelToWorld;
	int		u_worldToCamera;
	int		u_cameraToView;
	int		u_color;
	glm::vec3	color;
	int		fontIndex;
	GLfloat	scale;

public:
	GTextLabel();
	~GTextLabel();
	void	initializeGL();
	void	setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj);
	void	addString(QString str, GLfloat x, GLfloat y);
	void	prepare();
	void	setFont(int size, vec3 color);
	void	renderText();
	vec2	textSize(const QString& str);
};

#endif // GTEXTLABEL_H
