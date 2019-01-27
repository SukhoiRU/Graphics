#ifndef STDAFX_H
#define STDAFX_H

#include <QtGui>
#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>

#define BYTE char
#define DWORD uint32_t
#define LPCSTR char*

#ifdef __gl_h_
#undef __gl_h_
#undef GLAPI
#undef GL_INVALID_INDEX
#undef GL_TIMEOUT_IGNORED

#undef GL_VERSION_1_1
#undef GL_VERSION_1_2
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0
#undef GL_VERSION_2_1

#undef GL_VERSION_3_0
#undef GL_VERSION_3_1
#undef GL_VERSION_3_2
#undef GL_VERSION_3_3

#undef GL_VERSION_4_0
#undef GL_VERSION_4_1
#undef GL_VERSION_4_2
#undef GL_VERSION_4_3
#endif
#include <glad/glad.h>

#define ORGANIZATION_NAME "RAA ST"
#define ORGANIZATION_DOMAIN "www.raa-st.com"
#define APPLICATION_NAME "Graphics"

#include <iostream>
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source,
							GLenum type,
							GLuint id,
							GLenum severity,
							GLsizei length,
							const GLchar *message,
							const void *userParam);

#endif //STDAFX_H
