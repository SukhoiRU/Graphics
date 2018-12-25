#include "stdafx.h"
#include "GText.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

GText::GText()
{
	color	= glm::vec3(0.0f);
}


GText::~GText()
{
	delete textShader;
}

void	GText::setMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
{
	textShader->bind();
	glUniformMatrix4fv(u_modelToWorld, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(u_worldToCamera, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(u_cameraToView, 1, GL_FALSE, &proj[0][0]);
	textShader->release();

	scale	= 1./(view*glm::vec4(1,0,0,0)).x;
}

void	GText::initializeGL()
{
	textShader = new QOpenGLShaderProgram();
	textShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/text.vert");
	textShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/text.frag");
	textShader->link();

	textShader->bind();
	u_modelToWorld	= textShader->uniformLocation("modelToWorld");
	u_worldToCamera	= textShader->uniformLocation("worldToCamera");
	u_cameraToView	= textShader->uniformLocation("cameraToView");
	u_color			= textShader->uniformLocation("textColor");
	textShader->release();

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if(FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if(FT_New_Face(ft, "Resources/fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 14);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set � ��� ������� ����� � �����������
	for(GLuint c = 0; c < (128+95+79); c++)
	{
		// Load character glyph 
		GLuint code	= c;
		if(code > 127+95)
			code += 0x2100-(128+95);
		else if(code > 127)
			code += 0x0400-128;
		if(FT_Load_Char(face, code, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// Now store character for later use
		Character character ={
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLuint, Character>(code, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void	GText::RenderText(const QString& str, GLfloat x, GLfloat y)
{
	// Activate corresponding render state	
	textShader->bind();
	glUniform3f(u_color, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// Iterate through all characters
	std::wstring	text	= str.toStdWString();
	std::wstring::const_iterator c;
	for(c = text.begin(); c != text.end(); c++)
	{
		GLuint	cc	= *c;
		Character ch = Characters[cc];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] ={
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	textShader->release();
}

glm::vec2	GText::TextSize(const QString& str)
{
	GLfloat	x	= 0;
	GLfloat	y	= 0;
	// Iterate through all characters
	std::wstring	text	= str.toStdWString();
	std::wstring::const_iterator c;
	for(c = text.begin(); c != text.end(); c++)
	{
		GLuint	cc	= *c;
		Character ch = Characters[cc];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = 0 - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		if(h > y)	y = h;
		// Update VBO for each character
		GLfloat vertices[6][4] ={
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	return	glm::vec2(x, y);
}