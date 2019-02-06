#ifndef GAXE_H
#define GAXE_H

#include "GraphObject.h"

class QCursor;
class QDomElement;
class Accumulation;
class QOpenGLShaderProgram;
class GTextLabel;
using glm::ivec2;

namespace Graph{

class GAxe : public GraphObject  //Класс оси графика
{
	enum Direction	{TOP, BOTTOM, ALL};
	enum DataType	{Bool, Int, Double, Float, Short};

//////////////////////////////////////////////////////////////////////////////////
//		Данные
//////////////////////////////////////////////////////////////////////////////////
private:
	vec2			m_BottomRight;	//Положение нижнего правого угла, мм
	vec2			m_FrameBR;		//Положение нижнего правого угла рамки, мм

	vec2			m_OldPoint;		//Предыдущая точка графика
	Direction		m_Direction;	//Направление перетаскивания объекта

	DataType		m_DataType;		//Тип отображаемых данных
	int				m_AxeLength;	//Длина оси в клетках	
	int				m_Axe_nCount;	//Количество точек в буфере отрисовки шкалы

	int				m_Offset;		//Смещение в записи		
	int				m_Data_Len;		//Длина для Орион
	int				m_MaskSRK;		//Маска СРК
	double			m_K_short;		//Масштаб для записей *.mig

	double*			m_pOrionTime;	//Время Ориона из большого файла
    BYTE*			m_pOrionData;	//Данные Ориона

	//Данные для OpenGL
	vector<vec2>	m_data;
	GLuint	dataVAO, dataVBO;
	GLuint	axeVAO, axeVBO;
	GTextLabel*		textLabel;
	QSizeF	oldGrid;
	QRectF	oldArea;
	float	oldScale;

	//Shader Information
	static QOpenGLShaderProgram*	m_program;
	static int		u_modelToWorld;
	static int		u_worldToCamera;
	static int		u_cameraToView;
	static int		u_color;
	static int		u_alpha;
	static int		u_round;
	static int		u_lineType;

	static QOpenGLShaderProgram*	m_data_program;
	static int		u_data_modelToWorld;
	static int		u_data_worldToCamera;
	static int		u_data_cameraToView;
	static int		u_data_color;
	static int		u_data_alpha;
	static int		u_data_round;
	static int		u_data_lineType;
	static int		u_data_baseLine;
	static int		u_data_pixelSize;

	static QOpenGLShaderProgram*	m_data2_program;
	static int		u_data2_modelToWorld;
	static int		u_data2_worldToCamera;
	static int		u_data2_cameraToView;
	static int		u_data2_color;
	static int		u_data2_alpha;
	static int		u_data2_round;
	static int		u_data2_lineType;

	static QOpenGLShaderProgram*	m_cross_program;
	static	int		u_cross_modelToWorld;
	static	int		u_cross_worldToCamera;
	static	int		u_cross_cameraToView;
	static	GLuint	cross_texture;

public:
	QString			m_Name;			//Название оси
	QString			m_Path;			//Путь к элементу в накоплении
	int				m_Record;		//Номер в накоплении
	int				m_nMarker;		//Тип маркера
	int				m_nAcc;			//Номер накопления
	QString			m_AccName;		//Имя накопления

	double			m_Min;			//Минимальное значение на оси
	double			m_AxeScale;		//Цена деления, физическая величина клетки
	int				m_nSubTicks;	//Количество мелких штрихов	

	vec3	  		m_Color;		//Цвет графика
	bool			m_bShowNum;		//Признак отрисовки номера накопления
	static double 	m_FontScale;	//Масштабный коэффициент шрифта
	static double 	m_TickSize;		//Размер штриха, мм
	static double	m_Width;		//Толщина линии
	static double	m_SelectedWidth;//Толщина выделенной линии
	double			m_SpecWidth;	//Особая толщина 
	bool			m_bSRK;			//Признак разовой команды
	int				m_nBitSRK;		//Бит СРК
	QString			m_TextFormat;	//Вывод в таблице и на оси
	bool			m_bInterpol;	//Признак интерполяции точек на графике

	//////////////////////////////////////////////////////////////////////////
	//	Для фильтрации
	bool			m_bAperiodic;
	bool			m_bOscill;
	double			m_Aperiodic_T;
	double			m_Oscill_T;
	double			m_Oscill_Ksi;

//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GAxe();
	virtual ~GAxe();

	//Запись-чтение
	virtual void	Save(QDomElement* node);		//Сохранение XML
	virtual void	Load(QDomElement* node);		//Чтение XML
	void			UpdateRecord(std::vector<Accumulation*>* pBuffer);//Обновление данных о массиве
	
	//Рисование
	virtual void	initializeGL();
	virtual void	clearGL();
	virtual void	Draw(const double t0, const double TimeScale, const QSizeF& grid, const QRectF& area, const float alpha);					//Полное рисование
//	virtual void	DrawFrame(const QSize& grid);			//Отрисовка только рамки

	//Мышиные дела
	virtual bool	HitTest(const vec2& pt);//Проверка на попадание курсора
//	virtual HCURSOR	GetCursorHandle(const vec2& pt, UINT nFlags);		//Получение типа курсора
    virtual bool	MoveOffset(const vec2& delta, const Qt::MouseButtons& buttons, const Qt::KeyboardModifiers& mdf);	//Перемещение на заданное расстояние
	virtual void	OnDoubleClick();				//Реакция на щелчок мышью
	virtual void	OnStartMoving(){};				//Реакция на начало перетаскивания
	virtual void	OnStopMoving();					//Реакция на конец перетаскивания

	void	FitToScale(double t0 = 0, double t1 = 0);//Подбор масштаба и начала
	void	GetLimits(	double* pMin = 0,
						double* pMax = 0);			//минимакс

	void	GetLimits(double t0, double t1,
						double* pMin = 0,
						double* pMax = 0);			//минимакс

	//Служебные функции
private:
	void	DrawMarker(int x, int y);				//Отрисовка маркера в заданных координатах
	void	Draw_DEC_S();							//Отрисовка для вторички

public:
	void		SetPosition(double x, double y);		//Начальная установка координат
	void		SetPosition(vec2 pt);					//Начальная установка координат
	vec2		GetPosition()const{return m_BottomRight;} 	//Получение позиции
	double		GetValueAtTime(const double Time) const;	//Получение данных из конкретной записи
	double		GetTopPosition();						//Координата верхнего угла по высоте
	bool		IsBoolean() const;						//Признак логической оси
	void		GetStatistic() const;					//Выдача статистической информации
	void		ErrorsFilter() const;					//Фильтрация сбоев
	void		ClearOrionData() {m_pOrionData = 0; m_pOrionTime = 0;}
	void		UpdateFiltering();						//Фильтрация сигнала
	void		Zoom(bool bUp);							//Растяжение/сжатие
	int			getAxeLength() const {return m_AxeLength;}
	void		setAxeLength(int len);
};
}

#endif // !GAXE_H
