#pragma once

namespace SAPR{
namespace Static{
/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Звенья, не являющиеся динамическими
//
/////////////////////////////////////////////////////////////////////////////////////////////

//Знак числа (+1, 0, -1)	
int		Sign(const double& x);

//Минимум
double	Min (const double& x1, const double& x2);	
double	Min (const double& x1, const double& x2, const double& x3);	
double	Min (const double& x1, const double& x2, const double& x3, const double& x4);	
double	Min (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5);	

//Максимум
double	Max (const double& x1, const double& x2);	
double	Max (const double& x1, const double& x2, const double& x3);	
double	Max (const double& x1, const double& x2, const double& x3, const double& x4);	
double	Max (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5);	

class Corrector //Одномерная интерполяция
{
	double	*x;			//Указатель на массив аргумента
	double	*y;			//Указатель на массив функции
	double*	a;
	double*	b;
	int		n;			//Размерность массивов
	bool	IsOwn;		//Признак собственной памяти под массивы
public:	
	//Метод линейной интерполяции
	double operator () (const double& x0)
	{
		//Проверка на диапазон
		if(x0 <= x[0])		return y[0];
		if(x0 >= x[n-1])	return y[n-1];

		//Ищем в массиве аргумента
		int	nMin	= 0;
		int	nMax	= n-1;
		int i;
		while(nMax - nMin > 1)
		{
			i	= (nMin+nMax)/2;
			if(x0 >= x[i])	nMin	= i;
			else			nMax	= i;
		}

		return a[nMin]*x0 + b[nMin];
	}

	double operator [] (const double& x0){return operator()(x0);}

	Corrector (double* X, double* Y, int N, bool Copy = false);
	Corrector (const char* s);
	virtual ~Corrector ();
};

class Clip		//Ограничитель с верхней и нижней полками
{	
private:
	double MIN;	//Минимальное значение
	double MAX;	//Максимальное значение	

public:
	Clip(double MIN, double MAX): MIN(MIN),MAX(MAX){};
	Clip(double M): MIN(-M),MAX(M){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала 	
};


class Clip_Max	//Ограничитель с верхней полкой
{
private:
	double MAX;	//Максимальное значение	

public:
	Clip_Max(double MAX):MAX(MAX){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала	
};



class Clip_Min	//Ограничитель с нижней полкой
{	
private:
	double MIN;	//Минимальное значение

public:
	Clip_Min(double MIN):MIN(MIN){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала
};


class Deadzone		//Зона нечувствительности
{
private:
	double Left;	//Ограничение слева
	double Right;	//Ограничение справа	

public:
	Deadzone(double Left, double Right): Left(Left), Right(Right){};
	Deadzone(double M): Left(-M), Right(M){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала
};

class Dead_Left		//Зона нечувствительности слева
{
private:
	double Left;	//Ограничение слева

public:
	Dead_Left(double Left): Left(Left){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала
};

class Dead_Right	//Зона нечувствительности справа
{
private:
	double Right;	//Ограничение справа	

public:
	Dead_Right(double Right): Right(Right){};

	double operator [] (const double& u);//Метод ограничения сигнала
	double operator () (const double& u);//Метод ограничения сигнала
};

class If
{
	bool	z_bKey;
	double	z_True;
	double	z_False;
public:
	If(bool B): z_bKey(B), z_True(0), z_False(0) {}

	If&	Then (const double& True)	{z_True	 = True; return *this;}
	If&	Else (const double& False)	{z_False = False; return *this;}
	
	operator	const double&() const;
};

class DiscretValue		//Имитация ограниченной разрядности числа
{	
private:
	int		nBits;	//Разрядность
	double	MIN;	//Минимальное значение
	double	MAX;	//Максимальное значение	

public:
	DiscretValue(int nBits, double MIN, double MAX): nBits(nBits), MIN(MIN), MAX(MAX){};

	double operator [] (const double& u);//Метод расчета
	double operator () (const double& u);//Метод расчета 	
};

double	quorum(double u1, double u2);
double	quorum(double u1, double u2, double u3);
double	quorum(double u1, double u2, double u3, double u4);
double	quorum(double u1, double u2, double u3, double u4,double u5);
double	quorum(double u1, double u2, double u3, double u4,double u5,double u6);
double	quorum(double u1, double u2, double u3, double u4,double u5,double u6,double u7);
double	quorum(double u1, double u2, double u3, double u4,double u5,double u6,double u7,double u8);
double	quorum(double u1, double u2, double u3, double u4,double u5,double u6,double u7,double u8,double u9);
double	quorum(double u1, double u2, double u3, double u4,double u5,double u6,double u7,double u8,double u9,double u10);

double	quorum_min(double u1, double u2);
double	quorum_min(double u1, double u2, double u3, double u4);
double	quorum_min(double u1, double u2, double u3, double u4,double u5,double u6);
double	quorum_min(double u1, double u2, double u3, double u4,double u5,double u6,double u7,double u8);
double	quorum_min(double u1, double u2, double u3, double u4,double u5,double u6,double u7,double u8,double u9,double u10);

/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Sapr.inl"

} using namespace Static;

} using namespace SAPR;
