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
		qint64		OrionFilePos;	//Начало данных в общем файле
		qint64		OrionFileTime;	//Начало времени в общем файле
		uint		Length;			//Количество точек

		OrionSignal():OrionFilePos(0), OrionFileTime(0), Length(0){}
		virtual ~OrionSignal(){}
	};

	//Структура для большого файла Орион
	struct OrionHead
	{
		QString			name;		//Имя пакета
		qint64			pos;		//Начало пакета		
	};

	struct OrionData
	{
		QString		path;	//Путь к сигналу
		double*		pTime;	//Указатель на считанное время
		char*		ptr;	//Указатель на считанные данные
		DataType	type;	//Тип данных
		size_t		len;	//Количество точек
	};

	int					m_nOrionVersion;	//Номер версии файла Орион
	vector<OrionHead>	m_OrionPacketList;	//Перечень пакетов в файле Ориона
	vector<OrionData*>	m_Data;				//Перечень считанных данных

	void	LoadOrionPacket();					//Чтение Орион
	void	SaveOrionPart(QFile& file, double Time0, double Time1) const;	//Запись нужного куска
	void	SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const;	//Запись нужного куска пакета
	void	clearData();	//Очистка всех выделенных областей памяти под Орион

public:
	Orion_Accumulation();
	virtual ~Orion_Accumulation();
	
	//Переопределяемые методы
	virtual void	load(const QString& filename);
    virtual void	preloadData(const QStringList* pAxes);
	virtual bool	getData(const QString& path, size_t* len, const double** ppTime, const char** ppData, DataType* nType) const;
    virtual void	savePart(const QString& /*filename*/, double /*Time0*/, double /*Time1*/) const{};
    virtual void	print(const QString& /*filename*/, bool /*bHead = true*/){};
};

#endif // ORION_ACCUMULATION_H
