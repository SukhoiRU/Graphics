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
	enum	AccType{Acc_SAPR, Acc_TRF, Acc_CCS, Acc_Excell, Acc_KARP, Acc_MIG, Acc_MIG_4, Acc_Orion};

protected:
	vector<SignalInfo*>	m_Header;		//Описатель данных в массиве	
	AccType				m_Type;			//Тип накопления
	QString				m_Name;			//Имя накопления

public:
	Accumulation();
	virtual ~Accumulation();

	//Константный доступ
	const vector<SignalInfo*>&	header()	const	{return m_Header;}
	const AccType				type()		const	{return m_Type;}
	const QString&				name()		const	{return m_Name;}
	void						setName(QString& name)	{m_Name = name;}

	//Переопределяемые методы
	virtual const double*	getTime(const QString& path) const = 0;
	virtual const char*		getData(const QString& path) const = 0;

	virtual void	load(const QString& filename) = 0;
	virtual void	savePart(const QString& filename, double Time0, double Time1) const = 0;
	virtual void	print(const QString& filename, bool bHead = true){};
};

#endif // ACCUMULATION_H
