#pragma once

namespace SAPR{
namespace Static{
/////////////////////////////////////////////////////////////////////////////////////////////
//
//	������, �� ���������� �������������
//
/////////////////////////////////////////////////////////////////////////////////////////////

//���� ����� (+1, 0, -1)	
int		Sign(const double& x);

//�������
double	Min (const double& x1, const double& x2);	
double	Min (const double& x1, const double& x2, const double& x3);	
double	Min (const double& x1, const double& x2, const double& x3, const double& x4);	
double	Min (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5);	

//��������
double	Max (const double& x1, const double& x2);	
double	Max (const double& x1, const double& x2, const double& x3);	
double	Max (const double& x1, const double& x2, const double& x3, const double& x4);	
double	Max (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5);	

class Corrector //���������� ������������
{
	double	*x;			//��������� �� ������ ���������
	double	*y;			//��������� �� ������ �������
	double*	a;
	double*	b;
	int		n;			//����������� ��������
	bool	IsOwn;		//������� ����������� ������ ��� �������
public:	
	//����� �������� ������������
	double operator () (const double& x0)
	{
		//�������� �� ��������
		if(x0 <= x[0])		return y[0];
		if(x0 >= x[n-1])	return y[n-1];

		//���� � ������� ���������
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

class Clip		//������������ � ������� � ������ �������
{	
private:
	double MIN;	//����������� ��������
	double MAX;	//������������ ��������	

public:
	Clip(double MIN, double MAX): MIN(MIN),MAX(MAX){};
	Clip(double M): MIN(-M),MAX(M){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� ������� 	
};


class Clip_Max	//������������ � ������� ������
{
private:
	double MAX;	//������������ ��������	

public:
	Clip_Max(double MAX):MAX(MAX){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� �������	
};



class Clip_Min	//������������ � ������ ������
{	
private:
	double MIN;	//����������� ��������

public:
	Clip_Min(double MIN):MIN(MIN){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� �������
};


class Deadzone		//���� ������������������
{
private:
	double Left;	//����������� �����
	double Right;	//����������� ������	

public:
	Deadzone(double Left, double Right): Left(Left), Right(Right){};
	Deadzone(double M): Left(-M), Right(M){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� �������
};

class Dead_Left		//���� ������������������ �����
{
private:
	double Left;	//����������� �����

public:
	Dead_Left(double Left): Left(Left){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� �������
};

class Dead_Right	//���� ������������������ ������
{
private:
	double Right;	//����������� ������	

public:
	Dead_Right(double Right): Right(Right){};

	double operator [] (const double& u);//����� ����������� �������
	double operator () (const double& u);//����� ����������� �������
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

class DiscretValue		//�������� ������������ ����������� �����
{	
private:
	int		nBits;	//�����������
	double	MIN;	//����������� ��������
	double	MAX;	//������������ ��������	

public:
	DiscretValue(int nBits, double MIN, double MAX): nBits(nBits), MIN(MIN), MAX(MAX){};

	double operator [] (const double& u);//����� �������
	double operator () (const double& u);//����� ������� 	
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
