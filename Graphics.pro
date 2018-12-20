# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = Graphics
win32:CONFIG(release, debug|release): DESTDIR = ../../../bin/qtcreator
else:win32:CONFIG(debug, debug|release): DESTDIR = ../../../bin/qtcreator/debug
QT += core xml opengl gui widgets printsupport svg
CONFIG += debug
#DEFINES += WIN64 QT_OPENGL_LIB QT_PRINTSUPPORT_LIB QT_WIDGETS_LIB QT_XML_LIB
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/$(ConfigurationName)\
    ../../include/glad/include
LIBS += -ldl
#LIBS += -lopengl32
#    -lglu32
PRECOMPILED_HEADER = stdafx.h
#DEPENDPATH += .
#MOC_DIR += ./GeneratedFiles/$(ConfigurationName)
OBJECTS_DIR += debug
#UI_DIR += ./GeneratedFiles
#RCC_DIR += ./GeneratedFiles
include(Graphics.pri)
