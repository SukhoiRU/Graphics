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
		streampos	OrionFilePos;	//Начало данных в общем файле
		streampos	OrionFileTime;	//Начало времени в общем файле
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
		string			name;		//Имя пакета
		streampos		pos;		//Начало пакета
		
		void	operator = (const OrionHead& h)
		{
			name	= h.name;
			pos		= h.pos;
		}
	};

	struct OrionData
	{
		streampos	pos;	//Положение в большом файле
		char*		ptr;	//Указатель на считанные данные
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
	virtual size_t	getData(const string& path, const double** ppTime, const char** ppData, int* nType) const;
	virtual void	savePart(const string& filename, double Time0, double Time1) const{};
	virtual void	print(const string& filename, bool bHead = true){};
};

#endif // ORION_ACCUMULATION_H
