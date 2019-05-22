#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <QString>
#include <vector>
using namespace std;

//Базовый класс накопления
class Accumulation
{
public:
	//Структура элементов описателя накопления
	class SignalInfo
	{
	public:
		QString		path;		//Путь к элементу с разделителем "\"
		QString		comment;	//Комментарий для второго столбца
		vector<int>	icons;		//Набор иконок-описателей уровней

		SignalInfo(){}
		virtual ~SignalInfo(){}
	};

	//Типы накопления
    enum class	AccType{Acc_SAPR, Acc_TRF, Acc_CCS, Acc_Excell, Acc_KARP, Acc_MIG, Acc_MIG_4, Acc_Orion};

	//Типы данных
	enum class	DataType{Bool, Int, Double, Float, Short};

protected:
	vector<SignalInfo*>	m_Header;		//Описатель данных в массиве	
	AccType				m_Type;			//Тип накопления
	QString				m_Name;			//Имя накопления
	QFile*				m_pFile;		//Указатель на открытый файл
	QDateTime			m_lastModified;	//Время последнего изменения файла

public:
	Accumulation();
	virtual ~Accumulation();

	//Константный доступ
	const vector<SignalInfo*>&	header()	const	{return m_Header;}
    AccType                     type()		const	{return m_Type;}
	const QString&				name()		const	{return m_Name;}
    void						setName(const QString& name)	{m_Name = name;}

	//Переопределяемые методы
	virtual void	load(const QString& filename) = 0;
	virtual void	preloadData(const QStringList* pAxes) = 0;
	virtual bool	getData(const QString& path, size_t* len, const double** ppTime, const char** ppData, DataType* nType) const = 0;
	virtual void	savePart(const QString& filename, double Time0, double Time1) const = 0;
    virtual void	print(const QString& /*filename*/, bool /*bHead = true*/){};
};

#endif // ACCUMULATION_H
