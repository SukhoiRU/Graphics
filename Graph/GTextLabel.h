#ifndef GTEXTLABEL_H
#define GTEXTLABEL_H

#include <glm/glm.hpp>
using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
#include <vector>
#include <map>
using std::vector;
using std::map;

class QOpenGLShaderProgram;

namespace Graph{

class GTextLabel
{
private:
	// Holds all state information relevant to a character as loaded using FreeType
	struct CharInfo{
		int		unicode;	//��� ������� � UNICODE
		int		layer;		//���� � ��������
		vec2	size;		//������� �������
		vec2	offset;		//�������� ������� ������������ baseline
		float	advance;	//�������� � ���������� �������
	};

	struct FontInfo
	{
		QString		name;		//�������� ������
		GLfloat		size;		//������
		GLuint		pxrange;	//����� ������ �������
		GLuint		texSize;	//������ ��������
		map<int, CharInfo>	charMap;	//��������� ������
	};

	//����� ������
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

	//������ ��� ������� �������
	struct Data 
	{
		vec2	point;
		vec3	text;
		vec2	corr;
	};
	vector<Data>	m_data;
	mat4			m_model;

	GLuint	textVBO;
	int		fontIndex;

	void	loadFontInfo();

public:
	GTextLabel();
	~GTextLabel();
	void	initializeGL();
	void	clearGL();
	static void	finalDelete();

	void	setMatrix(glm::mat4 model);
	void	addString(QString str, GLfloat x, GLfloat y);
	void	prepare();
	void	setFont(GLfloat size);
	void	renderText(vec3 color, float alpha);
	vec2	textSize(const QString& str);
	GLfloat	midLine();
	GLfloat	topLine();
};

}
#endif // GTEXTLABEL_H
