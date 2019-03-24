//Знак числа (+1, 0, -1)	
inline	int		Sign(const double& x)					{if(!x) return 0; else return x < 0 ? -1 : 1;}

//Минимум
inline	double	Min (const double& x1, const double& x2){return x1 < x2 ? x1 : x2;}	
inline	double	Min (const double& x1, const double& x2, const double& x3){return Min(x1, Min(x2,x3));}
inline	double	Min (const double& x1, const double& x2, const double& x3, const double& x4){return Min(x1, Min(x2,x3,x4));}	
inline	double	Min (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5){return Min(x1, Min(x2,x3,x4,x5));}

//Максимум
inline	double	Max (const double& x1, const double& x2){return x1 > x2 ? x1 : x2;}	
inline	double	Max (const double& x1, const double& x2, const double& x3){return Max(x1, Max(x2,x3));}
inline	double	Max (const double& x1, const double& x2, const double& x3, const double& x4){return Max(x1, Max(x2,x3,x4));}	
inline	double	Max (const double& x1, const double& x2, const double& x3, const double& x4, const double& x5){return Max(x1, Max(x2,x3,x4,x5));}


inline double Clip::operator [] (const double& u)
{
	if(u <= MIN) return MIN;
	if(u >= MAX) return MAX;
	return u;
}

inline double Clip::operator () (const double& u)
{
	return operator[](u);
}

inline double Clip_Max::operator [] (const double& u)
{
	if(u >= MAX) 
		return MAX;
	else
		return u;
}

inline double Clip_Max::operator () (const double& u)
{
	return operator[](u);
}

inline double Clip_Min::operator [] (const double& u)
{
	if(u <= MIN) 
		return MIN;
	else
		return u;
}

inline double Clip_Min::operator () (const double& u)
{
	return operator[](u);
}

inline double Deadzone::operator [] (const double& u)
{
	if(u <= Left)	return u - Left;
	if(u >= Right)	return u - Right;
	return 0;
}

inline double Deadzone::operator () (const double& u)
{
	return operator[](u);
}

inline double Dead_Left::operator [] (const double& u)
{
	if(u >= Left) return u - Left;
	return 0;
}

inline double Dead_Left::operator () (const double& u)
{
	return operator[](u);
}

inline double Dead_Right::operator [] (const double& u)
{
	if(u <= Right) return u - Right;
	return 0;
}

inline double Dead_Right::operator () (const double& u)
{
	return operator[](u);
}

inline If::operator	const double&() const
{
	return z_bKey ? z_True : z_False;
}

inline double DiscretValue::operator [] (const double& u)
{
	int		Nmax	= (1 << nBits) - 1;
	int		Code	= int((u - MIN)/(MAX-MIN)*(double)Nmax);
	if(Code > Nmax)	Code	= Nmax;
	if(Code < 0)	Code	= 0;
	double	Value	= (double)Code/(double)Nmax*(MAX-MIN) + MIN;
	
	return Value;
}

inline double DiscretValue::operator () (const double& u)
{
	return operator[](u);
}
