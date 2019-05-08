#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

//Базовый класс накопления
class Accumulation
{
public:
	//Структура элементов описателя накопления
	class SignalInfo
	{
	public:
		string		path;		//Путь к элементу с разделителем "\"
		string		comment;	//Комментарий для второго столбца
		vector<int>	icons;		//Набор иконок-описателей уровней

		SignalInfo(){}
		virtual ~SignalInfo(){}
	};

	//Типы накопления
	enum	AccType{Acc_SAPR, Acc_TRF, Acc_CCS, Acc_Excell, Acc_KARP, Acc_MIG, Acc_MIG_4, Acc_Orion};

protected:
	vector<SignalInfo*>	m_Header;		//Описатель данных в массиве	
	AccType				m_Type;			//Тип накопления
	string				m_Name;			//Имя накопления
	ifstream*			m_pFile;		//Указатель на открытый файл

public:
	Accumulation();
	virtual ~Accumulation();

	//Константный доступ
	const vector<SignalInfo*>&	header()	const	{return m_Header;}
	const AccType				type()		const	{return m_Type;}
	const string&				name()		const	{return m_Name;}
	void						setName(string& name)	{m_Name = name;}

	//Переопределяемые методы
	virtual void	load(const string& filename) = 0;
	virtual void	preloadData(QStringList* pAxes) = 0;
	virtual size_t	getData(const string& path, const double** ppTime, const char** ppData, int* nType) const = 0;
	virtual void	savePart(const string& filename, double Time0, double Time1) const = 0;
	virtual void	print(const string& filename, bool bHead = true){};
};

#endif // ACCUMULATION_H
