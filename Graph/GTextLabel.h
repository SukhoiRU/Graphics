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

namespace Graph{

class GTextLabel
{
private:
public:
	// Holds all state information relevant to a character as loaded using FreeType
	struct CharInfo{
		int		id;			//��� ������� � UNICODE
		ivec2	tex;		//���������� � ��������
		ivec2	size;		//������� �������
		ivec2	offset;		//�������� ������� ������������ ���������
		ivec2	origSize;	//�������� ������ �������
	};

	struct FontInfo
	{
		QString		name;	//�������� ������
		int			size;	//������ � ��������
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
	static	int					u_alpha;

	//������ ��� ������� �������
	vector<vec4>	m_data;

	GLuint	textVBO;
	int		fontIndex;
	GLfloat	scale;

	void	loadFontInfo();

public:
	GTextLabel();
	~GTextLabel();
	void	initializeGL();
	void	clearGL();
	static void	finalDelete();

	void	setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj);
	void	addString(QString str, GLfloat x, GLfloat y);
	void	prepare();
	void	setFont(int size, GLfloat scale);
	void	renderText(vec3 color, float alpha);
	vec2	textSize(const QString& str);
	GLfloat	baseLine();
	GLfloat	midLine();
	GLfloat	topLine();
};

}
#endif // GTEXTLABEL_H
