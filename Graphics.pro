# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = Graphics

unix{
    CONFIG(release, debug|release) {
        DESTDIR = $$PWD/../bin/qtcreator
        OBJECTS_DIR = $$PWD/../garbage
        LIBS += -ldl
    }

    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../bin/qtcreator/debug
        OBJECTS_DIR = $$PWD/../garbage/debug
        LIBS += -ldl
    }
}

win32{
    CONFIG(release, debug|release) {
        DESTDIR = $$PWD/../../bin/qtcreator
        OBJECTS_DIR = $$PWD/../../garbage
        LIBS += -lopengl32 -lglu32
    }

    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../bin/qtcreator/debug
        OBJECTS_DIR = $$PWD/../../garbage/debug
        LIBS += -lopengl32 -lglu32
    }
}

QT += core xml opengl gui widgets printsupport svg
CONFIG += debug

#DEFINES += WIN64 QT_OPENGL_LIB QT_PRINTSUPPORT_LIB QT_WIDGETS_LIB QT_XML_LIB
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/$(ConfigurationName)\
    ../../include/glad/include\
    ../../include/glm

#LIBS += -lopengl32
#    -lglu32
#DEPENDPATH += .
#MOC_DIR += ./GeneratedFiles/$(ConfigurationName)
#OBJECTS_DIR += debug
#UI_DIR += ./GeneratedFiles
#RCC_DIR += ./GeneratedFiles
include(Graphics.pri)
