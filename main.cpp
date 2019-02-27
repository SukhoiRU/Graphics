#include "stdafx.h"
#include "graphicsdoc.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
//	QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, false);
//	qputenv("QT_ANGLE_PLATFORM", "d3d11");
	QApplication a(argc, argv);
//	a.setAttribute(Qt::AA_UseDesktopOpenGL, true);

	QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
	QCoreApplication::setApplicationName(APPLICATION_NAME);

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption orionOption("orion", "Открывает файл <file.orion>.", "file.orion");
	QCommandLineOption grfOption("grf", QCoreApplication::translate("main", "Copy all source files into <directory>."), "*.grf");
	parser.addOption(orionOption);
	parser.addOption(grfOption);
	parser.process(a);

//	QMessageBox::information(0, "safr", parser.helpText());

	GraphicsDoc w;
//    if(parser.isSet(grfOption))		w.LoadScreen(parser.value(grfOption));
//    if(parser.isSet(orionOption))	w.LoadOrion(parser.value(orionOption));
	w.showMaximized();
//	w.show();
	if(parser.isSet(orionOption))	w.LoadOrion(parser.value(orionOption));
	if(parser.isSet(grfOption))		w.LoadScreen(parser.value(grfOption));
	
    return a.exec();
}
