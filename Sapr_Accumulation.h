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
		string		path;	//Путь к сигналу
		char*		ptr;	//Указатель на считанные данные
		DataType	type;	//Тип данных
		int			offset;	//Смещение сигнала в записи
	};

	int				m_nRecordSize;	//Длина вектора
	int				m_nRecCount;	//Количество записей
	streampos		m_DataPos;		//Положение начала данных в файле

	vector<SaprData*>	m_Data;		//Считанные данные
	double*				m_pTime;	//Общее время

	void	clearData();
public:
	Sapr_Accumulation();
	virtual ~Sapr_Accumulation();

	//Переопределяемые методы
	virtual void	load(const char* filename);
	virtual void	preloadData(QStringList* pAxes);
	virtual size_t	getData(const char* path, const double** ppTime, const char** ppData, int* nType) const;
	virtual void	savePart(const char* filename, double Time0, double Time1) const{};
	virtual void	print(const char* filename, bool bHead = true){};
};

#endif // SAPR_ACCUMULATION_H
