#ifndef ORION_ACCUMULATION_H
#define ORION_ACCUMULATION_H

#include "Accumulation.h"
class QDomElement;

class Orion_Accumulation : public Accumulation
{
private:

	//Структура элементов описателя накопления
	class OrionSignal : public Accumulation::SignalInfo
	{
	public:
		qint64		OrionFilePos;	//Начало данных в общем файле
		qint64		OrionFileTime;	//Начало времени в общем файле
		uint		Length;			//Количество точек

		OrionSignal()
		{
			OrionFilePos	= 0;
			OrionFileTime	= 0;
			Length			= 0;
		}
		virtual ~OrionSignal(){}
	};

	//Структура для большого файла Орион
	struct OrionHead
	{
		QString			name;		//Имя пакета
		qint64			pos;		//Начало пакета
        QDomElement*	attr;		//Описатель пакета
		
		void	operator = (const OrionHead& h)
		{
			name	= h.name;
			pos		= h.pos;
			attr	= h.attr;
		}
	};

	struct OrionData
	{
		qint64	pos;	//Положение в большом файле
		BYTE*	ptr;	//Указатель на считанные данные
	};

private:	
	int					m_nOrionVersion;	//Номер версии файла Орион
	QFile*				m_pOrionFile;		//Указатель на единый файл Ориона
	vector<OrionHead>	m_OrionPacketList;	//Перечень пакетов в файле Ориона
	mutable vector<OrionData>	m_OrionData;		//Перечень считанных данных

	void			LoadOrionPacket();					//Чтение Орион
	void			SaveOrionPart(QFile& file, double Time0, double Time1) const;	//Запись нужного куска
	void			SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const;	//Запись нужного куска пакета

	//double*	GetOrionTime(const HeaderElement& h) const;
	//BYTE*	GetOrionData(const HeaderElement& h) const;
	void	FreeOrionData();	//Очистка всех выделенных областей памяти под Орион

public:
	Orion_Accumulation();
	virtual ~Orion_Accumulation();
	
	//Переопределяемые методы
	virtual const double*	getTime(const QString& path) const;
	virtual const char*		getData(const QString& path) const;

	virtual void	load(const QString& filename);
	virtual void	savePart(const QString& filename, double Time0, double Time1) const;
	virtual void	print(const QString& filename, bool bHead = true){};
};

#endif // ORION_ACCUMULATION_H
