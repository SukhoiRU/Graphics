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
	QCoreApplication::setApplicationVersion("Версия 1.1 по ветке accumulation");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption grfOption("grf", "Загружает файл экрана <file.grf>", "*.grf");
	QCommandLineOption orionOption("orion", "Открывает файл <file.orion>.", "*.orion");
	QCommandLineOption bufOption("buf", "Открывает файл <file.buf>.", "*.buf");
	QCommandLineOption trfOption("trf", "Открывает файл <file.trf>.", "*.trf");
	parser.addOption(grfOption);
	parser.addOption(orionOption);
	parser.addOption(bufOption);
	parser.addOption(trfOption);
	parser.process(a);

	//QColorDialog::setStandardColor(0, Qt::black);
	//QColorDialog::setStandardColor(1, Qt::red);
	//QColorDialog::setStandardColor(2, Qt::green);
	//QColorDialog::setStandardColor(3, Qt::blue);
	//QColorDialog::setStandardColor(4, Qt::darkBlue);

	GraphicsDoc w;
	w.showMaximized();
//	w.show();
	if(parser.isSet(grfOption))		w.loadScreen(parser.value(grfOption));
    if(parser.isSet(orionOption))	w.loadData(parser.value(orionOption), Accumulation::AccType::Acc_Orion);
    if(parser.isSet(bufOption))		w.loadData(parser.value(bufOption), Accumulation::AccType::Acc_SAPR);
    if(parser.isSet(trfOption))		w.loadData(parser.value(trfOption), Accumulation::AccType::Acc_TRF);
	
    return a.exec();
}
