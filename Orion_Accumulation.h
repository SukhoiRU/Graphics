#ifndef ORION_ACCUMULATION_H
#define ORION_ACCUMULATION_H

#include "Accumulation.h"

class Orion_Accumulation : public Accumulation
{
private:
	//Структура элементов описателя накопления
	class OrionSignal : public Accumulation::SignalInfo
	{
	public:
		int64_t		OrionFilePos;	//Начало данных в общем файле
		int64_t		OrionFileTime;	//Начало времени в общем файле
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
		string		name;		//Имя пакета
		int64_t		pos;		//Начало пакета
		
		void	operator = (const OrionHead& h)
		{
			name	= h.name;
			pos		= h.pos;
		}
	};

	struct OrionData
	{
		int64_t	pos;	//Положение в большом файле
		char*	ptr;	//Указатель на считанные данные
	};

	int							m_nOrionVersion;	//Номер версии файла Орион
	vector<OrionHead>			m_OrionPacketList;	//Перечень пакетов в файле Ориона
	mutable vector<OrionData>	m_OrionData;		//Перечень считанных данных

	void	LoadOrionPacket();					//Чтение Орион
	void	SaveOrionPart(QFile& file, double Time0, double Time1) const;	//Запись нужного куска
	void	SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const;	//Запись нужного куска пакета
	void	FreeOrionData();	//Очистка всех выделенных областей памяти под Орион

public:
	Orion_Accumulation();
	virtual ~Orion_Accumulation();
	
	//Переопределяемые методы
	virtual void	load(const char* filename);
	virtual void	preloadData(QStringList* pAxes){};
	virtual size_t	getData(const char* path, const double** ppTime, const char** ppData, int* nType) const;
	virtual void	savePart(const char* filename, double Time0, double Time1) const{};
	virtual void	print(const char* filename, bool bHead = true){};
};

#endif // ORION_ACCUMULATION_H
