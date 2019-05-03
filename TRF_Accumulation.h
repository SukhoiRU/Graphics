#ifndef TRF_ACCUMULATION_H
#define TRF_ACCUMULATION_H

#include "Accumulation.h"

class TRF_Accumulation : public Accumulation
{
private:
	//Структура элементов описателя накопления
	class TrfSignal : public Accumulation::SignalInfo
	{
	public:
		int		Offset;			//Смещение переменной в векторе

		TrfSignal()
		{
			Offset	= 0;
		}
		virtual ~TrfSignal(){}
	};

	enum DataType	{Bool, Int, Double, Float = 12, Short = 14};

	struct TrfData
	{
		QString		path;	//Путь к сигналу
		char*		ptr;	//Указатель на считанные данные
		DataType	type;	//Тип данных
		int			offset;	//Смещение сигнала в записи
	};

#pragma pack(push, 1)
	struct TrfHead
	{
		int    		rec_numb;
		short  		rec_leng;
		short  		parm_numb;
		short       fic;
		short       a1;
		short       a2;
		short       a3;
		char        buf[496];
	};

	struct TrfParm
	{
		BYTE		parm_type;
		BYTE		parm_type2;
		BYTE		gpanel, gcol, tpanel, tcol;
		float   	dv;
		short   	offset;
		float   	min, max;
		float   	ak, vk;
		char        buf[9];
		char        zero[9];
		char        bufl[484];
		float		GetValue(const BYTE* buf)
		{
			short a;
			float w;

			switch(parm_type)
			{
				case 1: { w	= *(float*)(buf + offset); }break;
				case 2: { a	= *(short*)(buf	+ offset);	w	= a*vk	+ ak; }break;
				case 4: { w	= *(float*)(buf	+ offset); }break;
			}

			return w;
		}
	};
#pragma pack(pop)

	int				m_nRecordSize;	//Длина вектора
	int				m_nRecCount;	//Количество записей
	qint64			m_DataPos;		//Положение начала данных в файле
	char*			m_pTRF_Head;	//Заголовок TRF-файла для сохранения куска
	int				m_nTRF_Hsize;	//Размер заголовка TRF-файла

	vector<TrfData*>	m_Data;		//Считанные данные
	double*				m_pTime;	//Общее время

	void	clearData();
public:
	TRF_Accumulation();
	virtual ~TRF_Accumulation();

	//Переопределяемые методы
	virtual void	load(const QString& filename);
	virtual void	preloadData(QStringList* pAxes);
	virtual size_t	getData(const QString& path, const double** ppTime, const char** ppData, int* nType) const;
	virtual void	savePart(const QString& filename, double Time0, double Time1) const{};
	virtual void	print(const QString& filename, bool bHead = true){};
};

#endif // TRF_ACCUMULATION_H
