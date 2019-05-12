#ifndef SAPR_ACCUMULATION_H
#define SAPR_ACCUMULATION_H

#include "Accumulation.h"

class Sapr_Accumulation : public Accumulation
{
private:
	//Структура элементов описателя накопления
	class SaprSignal : public Accumulation::SignalInfo
	{
	public:
		int		Offset;			//Смещение переменной в векторе

		SaprSignal()
		{
			Offset	= 0;
		}
		virtual ~SaprSignal(){}
	};

	struct SaprData
	{
		QString		path;	//Путь к сигналу
		char*		ptr;	//Указатель на считанные данные
		DataType	type;	//Тип данных
		int			offset;	//Смещение сигнала в записи
	};

	int				m_nRecordSize;	//Длина вектора
	int				m_nRecCount;	//Количество записей
	qint64			m_DataPos;		//Положение начала данных в файле

	vector<SaprData*>	m_Data;		//Считанные данные
	double*				m_pTime;	//Общее время

	void	clearData();
public:
	Sapr_Accumulation();
	virtual ~Sapr_Accumulation();

	//Переопределяемые методы
	virtual void	load(const QString& filename);
	virtual void	preloadData(QStringList* pAxes);
	virtual bool	getData(const QString& path, size_t* len, const double** ppTime, const char** ppData, DataType* nType) const;
	virtual void	savePart(const QString& filename, double Time0, double Time1) const{};
	virtual void	print(const QString& filename, bool bHead = true){};
};

#endif // SAPR_ACCUMULATION_H
