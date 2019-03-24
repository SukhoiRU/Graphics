#ifndef SYSTEMDYNAMIC_H
#define SYSTEMDYNAMIC_H
#include "Sapr.h"

using namespace Graph;

namespace SAPR{
namespace Dynamic{
//////////////////////////////////////////////////////////////////////
// Integral - интеграл с коэффициентом усиления
//
//	 1
//	---
//	 p
//	
//////////////////////////////////////////////////////////////////////
class	Integral
{
	double& x;
	double	K;
public:
	Integral(double& x, double K = 1): x(x), K(K){};
	double operator [] (const double& u)
	{
		if(GraphObject::modelTime == 0) x = 0;

		x += K*u*GraphObject::timeStep;
		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};

class	LimIntegral
{
	double& x;
	double	Min;
	double	Max;
	double	K;
public:
	LimIntegral(double& x, double Min, double Max, double K = 1):x(x), K(K), Min(Min), Max(Max){};
	double operator [] (const double& u)
	{
		if(GraphObject::modelTime == 0) x = 0;

		x += K*u*GraphObject::timeStep;
		x = Clip(Min,Max)[x];
		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};

//////////////////////////////////////////////////////////////////////
// Aperiodic -	апериодика, заданная постоянной времени
//
//	 1
//	----
//	Tp+1
//	
//////////////////////////////////////////////////////////////////////
class	Aperiodic
{
	double& x;
	double	T;
public:
	Aperiodic(double& x, double T):x(x), T(T){};
	double operator [] (const double& u)
	{
		if(GraphObject::modelTime == 0) x = u;

		if(T)	x += (u-x)/T*GraphObject::timeStep;
		else	x	= u;
		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};
/*
class	LimAperiodic
{
	LPCSTR	Name;
	double	T;
	double	Min;
	double	Max;
public:
	LimAperiodic(LPCSTR Name, double T, double Min, double Max):Name(Name), T(T), Min(Min), Max(Max){};
	double operator [] (const double& u)
	{
		Phase<>	x(Name);
		if(ActiveContext->IsStaticMode()) x = u;

		if(T)	
		{
			x.SetDerivative((u-x)/T);
			if(x < Min)	x	= Min;
			if(x > Max)	x	= Max;
		}
		else	x	= u;
		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};*/
//////////////////////////////////////////////////////////////////////
// Oscill - колебательное звено, заданное постоянной времени и 
//			относительным демпфированием
//
//	         1
//	-----------------
//	T^2p^2+2T*ksi*p+1
//	
//////////////////////////////////////////////////////////////////////
class	Oscill
{
	vec2&	x;	
	double	T;
	double	ksi;
public:
	Oscill(vec2& x, double T, double ksi):x(x), T(T), ksi(ksi){};
	double operator [] (const double& u)
	{
		if(GraphObject::modelTime == 0) x = vec2(0, u);

		x.x += (-2.*ksi/T*x.x - 1/T/T*x.y + u/T/T)*GraphObject::timeStep;
		x.y	+= (-1./T/T*x.x)*GraphObject::timeStep;

		return	x.y;
	}
	double operator () (const double& u){return operator[](u);}
};
/*
class	LimOscill
{
	LPCSTR	Name;
	double	T;
	double	ksi;
	double	Min;
	double	Max;
public:
	LimOscill(LPCSTR Name, double T, double ksi, double Min, double Max):Name(Name), T(T), ksi(ksi), Min(Min), Max(Max){};
	double operator [] (const double& u)
	{
		Phase<vector2D>	x(Name);
		matrix2D		A(	-2*ksi/T,	-1/T/T, 
			1,			0	);
		vector2D		B(	1/T/T,		0		);

		if(ActiveContext->IsStaticMode()) x->Set(0, u);

		x.SetMatrixDerivative(A, B*u);
		if(x->y < Min)	x->y	= Min;
		if(x->y > Max)	x->y	= Max;

		return	x->y;
	}
	double operator () (const double& u){return operator[](u);}
};

//////////////////////////////////////////////////////////////////////
// Probka - фильтр-пробка, заданный коэффициентами полиномов 
//			числителя и знаменателя	
//	
//	A*p**2+B*p+C
//	------------
//	D*p**2+E*p+F
//	
//////////////////////////////////////////////////////////////////////
class	Probka
{
	LPCSTR	Name;
	double	A;
	double	B;
	double	C;
	double	D;
	double	E;
	double	F;
public:
	Probka(LPCSTR Name, double A, double B,double C,double D,double E,double F):
	  Name(Name), A(A), B(B), C(C), D(D), E(E), F(F){};
	//////////////////////////////////////////////////////////////////////
	// версия фильтра-пробки, с T и ksi
	//
	//	     (T*p)**2+1
	//	--------------------
	//	(T*p)**2+2*T*ksi*p+1
	//	
	//////////////////////////////////////////////////////////////////////
	Probka (LPCSTR Name, double T, double ksi):
	  Name(Name), A(T*T), B(0), C(1), D(T*T), E(2*T*ksi), F(1){};

	double operator [] (const double& u)
	{
		Phase<vector2D>	x(Name);
		matrix2D		mA(	-E/D,	-F/D,
							1,		0	);
		vector2D		vC(	B/D-A*E/D/D,	C/D-A*F/D/D);
		
		if(ActiveContext->IsStaticMode()) x->Set(0, u*D/F);
		
		x.SetMatrixDerivative(mA, vector2D(u,0));
		
		return vC*x + A/D*u;
	}
	double operator () (const double& u){return operator[](u);}
};
*/
//////////////////////////////////////////////////////////////////////
// Strip -	полосовой фильтр, заданный постоянными времени
//  		числителя и знаменателя	
//
//	T1p+1
//	-----
//	T2p+1
//	
//////////////////////////////////////////////////////////////////////
class	Strip
{
	double& x;
	double	T1;
	double	T2;
public:
	Strip(double& x, double T1, double T2):x(x), T1(T1), T2(T2){};
	double operator [] (const double& u)
	{
		if(GraphObject::modelTime == 0) x = (T1-T2)/T2*u;	 
		x += (( (T1-T2)/T2*u - x )/T2)*GraphObject::timeStep;

		return T1/T2*u - x;
	}
	double operator () (const double& u){return operator[](u);}
};
/*
//////////////////////////////////////////////////////////////////////
// Izodrom - изодром, определенный через постоянные времени
//
//	 T1p
//	-----
//	T2p+1
//	
//////////////////////////////////////////////////////////////////////
class	Izodrom
{
	LPCSTR	Name;
	double	T1;
	double	T2;
public:
	Izodrom(LPCSTR Name, double T1, double T2):Name(Name), T1(T1), T2(T2){};
	double operator [] (const double& u)
	{
		Phase<>	x(Name);

		if(ActiveContext->IsStaticMode()) x = u;	 
		x.SetDerivative((u - x)/T2);

		return T1/T2*(u - x);
	}
	double operator () (const double& u){return operator[](u);}
};

//////////////////////////////////////////////////////////////////////
// MP1 - привод первого порядка
//////////////////////////////////////////////////////////////////////
class	MP1
{
	LPCSTR	Name;
	double	Zone;
	double	Speed;
	double	Quality;
    double	Min_path;
	double	Max_path;
public:
	MP1(LPCSTR Name, double Zone, double Speed,double Quality,double Min_path, double Max_path):
	  Name(Name), Zone(Zone), Speed(Speed), Quality(Quality), 
	  Min_path(Min_path), Max_path(Max_path){};
	double operator [] (const double& u)
	{
		Phase<>	fi(Name);

		if(ActiveContext->IsStaticMode()) fi = u;

		double dfi = Clip(Speed)[Quality*Deadzone(Zone)(u - fi)];		
		
		fi.SetDerivative(dfi);	
		
		fi = Clip(Min_path, Max_path)[fi];

		return fi;
	}
	double operator () (const double& u){return operator[](u);}
};

//////////////////////////////////////////////////////////////////////////
//	Цифровой ограничитель скорости сигнала
//////////////////////////////////////////////////////////////////////////
class	RateLimit
{
	LPCSTR	Name;
	double	Vmax;
public:
	RateLimit(LPCSTR Name, double Vmax):Name(Name), Vmax(Vmax){};
	double operator [] (const double& u)
	{
		Phase<>	x(Name);
		if(ActiveContext->IsStaticMode()) x = u;

		const double& TimeStep	= ActiveContext->m_pSys->GetTimeStep();
		if(fabs(u-x) > Vmax*TimeStep)
		{
			x += Sign(u-x)*Vmax*TimeStep;
		}
		else
		{
			x = u;
		}

		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};

//Признак прохождения логического сигнала
inline bool	_Raised(LPCSTR name, bool b)
{
	State<bool>	old(name);
	bool			res	= false;
	if(b && !old)
		res	= true;
	old	= b;

	return	res;
}

//Временная задержка прохождения сигнала
inline bool	_Delay(LPCSTR name, bool b, double t)
{
	Phase<>	dT(name);
	if(b)
	{
		dT.SetDerivative(1);
		dT	= Clip(0,t)[dT];
	}
	else
	{
		dT	= 0;
	}

	return	b && (dT == t);
}

// Цифровое дифференцирование сигнала
class	Diff
{
	LPCSTR	Name;
public:
	Diff(LPCSTR Name):Name(Name){};
	double operator [] (const double& u)
	{
		Phase<>	x(Name);
		if(ActiveContext->IsStaticMode()) x = u;

		const double& TimeStep	= ActiveContext->m_pSys->GetTimeStep();
		double d;
		if(TimeStep)	d = (u - x)/TimeStep;
		else			d = 0;
		x = u;
		return	d;
	}
	double operator () (const double& u){return operator[](u);}
};

// Интегратор КСУ
class	Integral_CCS
{
	LPCSTR	Name;
	double	Min;
	double	Max;
	double	D;
public:
	Integral_CCS(LPCSTR Name, double Min, double Max, double D = 0):Name(Name),D(D),Min(Min), Max(Max){};
	double operator [] (const double& u)
	{
		Phase<>	x(Name);
		if(D)	x.SetDerivative(-D*x);
		else	x.SetDerivative(u);
		if(ActiveContext->IsStaticMode()) x = 0;
		x = Clip(Min,Max)[x];
		return	x;
	}
	double operator () (const double& u){return operator[](u);}
};

//Макрос для удобства работы
#define Event(b) _Raised(#b,b)
#define Delay(t,b) _Delay(#b, b, t)
*/
} using namespace Dynamic;
}using namespace SAPR;

#endif	//SYSTEMDYNAMIC_H
