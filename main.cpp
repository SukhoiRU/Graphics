#include "stdafx.h"
#include "graphicsdoc.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator*	tr	= new QTranslator;
	tr->load(":/Resources/tranlations/qtbase_ru.qm");
	a.installTranslator(tr);

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

	//QColorDialog::setStandardColor(0, Qt::black);
	//QColorDialog::setStandardColor(1, Qt::red);
	//QColorDialog::setStandardColor(2, Qt::green);
	//QColorDialog::setStandardColor(3, Qt::blue);
	//QColorDialog::setStandardColor(4, Qt::darkBlue);

	GraphicsDoc w;
//    if(parser.isSet(grfOption))		w.LoadScreen(parser.value(grfOption));
//    if(parser.isSet(orionOption))	w.LoadOrion(parser.value(orionOption));
	w.showMaximized();
//	w.show();
	if(parser.isSet(orionOption))	w.LoadOrion(parser.value(orionOption));
	if(parser.isSet(grfOption))		w.LoadScreen(parser.value(grfOption));
	
    return a.exec();
}
