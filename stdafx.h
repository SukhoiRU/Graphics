#include <QtGui>
#include <QtWidgets>

#ifdef __gl_h_
#undef __gl_h_
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
#endif

#include <glad/glad.h>

#define ORGANIZATION_NAME "RAA ST"
#define ORGANIZATION_DOMAIN "www.raa-st.com"
#define APPLICATION_NAME "Graphics"
