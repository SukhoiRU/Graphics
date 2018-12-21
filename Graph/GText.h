#ifndef GTEXT_H
#define GTEXT_H

#include <glm/glm.hpp>
#include <map>
using std::vector;

class QOpenGLShaderProgram;

class GText
{
private:
	// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;    // Horizontal offset to advance to next glyph
	};
	std::map<GLuint, Character> Characters;

	QOpenGLShaderProgram*	textShader;
	GLuint	textVAO, textVBO;
	int		u_modelToWorld;
	int		u_worldToCamera;
	int		u_cameraToView;
	int		u_color;
	glm::vec3	color;
	GLfloat	scale;

public:
	GText();
	~GText();
	void	initializeGL();
	void	setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj);
	void	setColor(glm::vec3 c){color = c;}
	void	RenderText(const QString& str, GLfloat x, GLfloat y);
};

#endif // GTEXT_H
