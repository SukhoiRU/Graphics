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

	enum DataType	{Bool, Int, Double, Float = 12, Short = 14};

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
	virtual size_t	getData(const QString& path, const double** ppTime, const char** ppData, int* nType) const;
	virtual void	savePart(const QString& filename, double Time0, double Time1) const{};
	virtual void	print(const QString& filename, bool bHead = true){};
};

#endif // SAPR_ACCUMULATION_H
