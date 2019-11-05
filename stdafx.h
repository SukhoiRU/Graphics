#ifndef STDAFX_H
#define STDAFX_H

#include <QtGui>
#include <QtWidgets>
#include <QOpenGLWidget>

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

constexpr char	ORGANIZATION_NAME[]		= "RAA ST";
constexpr char	ORGANIZATION_DOMAIN[]	= "www.raa-st.com";
constexpr char	APPLICATION_NAME[]		= "Graphics";

#endif //STDAFX_H
