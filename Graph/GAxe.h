#ifndef GAXE_H
#define GAXE_H

#include "GraphObject.h"

class QCursor;
class QDomElement;
class QOpenGLShaderProgram;
using glm::ivec2;

namespace Graph{
	class GTextLabel;

class GAxe : public GraphObject  //Класс оси графика
{
public:
	enum class DataType	{Bool, Int, Double, Float, Short};

//////////////////////////////////////////////////////////////////////////////////
//		Данные
//////////////////////////////////////////////////////////////////////////////////
private:
	vec2			m_BottomRight;	//Положение нижнего правого угла, мм
	vec2			m_FrameBR;		//Положение нижнего правого угла рамки, мм
	DataType		m_Data_Type;	//Тип отображаемых данных
	size_t			m_Data_Length;	//Длина для Орион
	int				m_Axe_Length;	//Длина оси в клетках	
	int				m_Axe_nCount;	//Количество точек в буфере отрисовки шкалы
	int				m_MaskSRK;		//Маска СРК

	//Данные для OpenGL
	vector<vec2>	m_data;
	vector<GLuint>	m_indices;
	GLuint			m_markersCount;
	GLuint			dataVBO;
	GLuint			markerIBO;
	GLuint			axeVBO;
	GTextLabel*		textLabel;
	vec2			oldGrid;
	vec2			oldAreaBL;
	vec2			oldAreaSize;
	float			oldScale;
	double			oldTime0;
	double			oldTimeStep;

	//Shader Information
	static QOpenGLShaderProgram*	m_axe_program;
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
	static int		u_data_lineType;
	static int		u_data_linewidth;
	static int		u_data_antialias;

	static QOpenGLShaderProgram*	m_bool_program;
	static int		u_bool_modelToWorld;
	static int		u_bool_worldToCamera;
	static int		u_bool_cameraToView;
	static int		u_bool_color;
	static int		u_bool_lineType;

	static QOpenGLShaderProgram*	m_marker_program;
	static int		u_marker_ortho;
	static int		u_marker_size;
	static int		u_marker_orientation;
	static int		u_marker_linewidth;
	static int		u_marker_antialias;
	static int		u_marker_fg_color;
	static int		u_marker_bg_color;
	static int		u_marker_type;

	static QOpenGLShaderProgram*	m_cross_program;
	static	int		u_cross_modelToWorld;
	static	int		u_cross_worldToCamera;
	static	int		u_cross_cameraToView;
	static	GLuint	cross_texture;

	static QOpenGLShaderProgram*	m_select_program;
	static	int		u_select_modelToWorld;
	static	int		u_select_worldToCamera;
	static	int		u_select_cameraToView;
	static	int		u_select_tick;
	static	int		u_select_toc;
	static	int		u_select_dL;
	static	int		u_select_color;
	static	int		u_select_round;

public:
	static	GLfloat	m_width, m_selWidth, m_interpWidth;
	static	GLfloat	m_alias, m_selAlias, m_interpAlias;

public:
	QString			m_Name;			//Название оси
	QString			m_Path;			//Путь к элементу в накоплении
	QString			m_DeltaPath;	//Путь к уменьшаемому для дельты
	int				m_nMarker;		//Тип маркера

	double			m_AxeMin;		//Минимальное значение на оси
	double			m_AxeScale;		//Цена деления, физическая величина клетки

	vec3	  		m_Color;		//Цвет графика
	bool			m_bShowNum;		//Признак отрисовки номера накопления
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
	GAxe(GAxe& axe);
	virtual ~GAxe();
	bool	isEmpty() const {return m_data.empty();}

	//Запись-чтение
	virtual void	save(QXmlStreamWriter& xml);		//Сохранение XML
	virtual void	load(QDomElement* node, double ver);		//Чтение XML
	void			uploadData(size_t size, const double* pTime, const char* pData, const DataType nType);
	void			clearData();
	
	//Рисование
	virtual void	initializeGL() override;
	virtual void	clearGL() override;
	static  void	finalDelete();
	virtual void	draw(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha) override;		//Полное рисование
	virtual void	drawFrame(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha) override;	//Только ось

	//Мышиные дела
	virtual bool	hitTest(const vec2& pt) override;//Проверка на попадание курсора
	virtual bool	getCursor(const vec2& pt, Qt::CursorShape& shape) override;
    virtual void	moveOffset(const vec2& delta, const Qt::MouseButtons& buttons, const Qt::KeyboardModifiers& mdf) override;	//Перемещение на заданное расстояние
    virtual void	onWheel(const vec2& pt, const Qt::KeyboardModifiers& mdf, vec2 numdegrees) override;	//Обработка колеса
	virtual void	onStartMoving() override{};				//Реакция на начало перетаскивания
	virtual void	onStopMoving() override;				//Реакция на конец перетаскивания

	void	fitToScale(double t0 = 0, double t1 = 0);//Подбор масштаба и начала
	void	getTime(double& t0, double& t1);	
	void	getLimits(	double* pMin = 0,
						double* pMax = 0);			//минимакс

	void	getLimits(double t0, double t1,
						double* pMin = 0,
						double* pMax = 0);			//минимакс

	//Служебные функции
private:
	void	updateIndices(const double t0, const double TimeScale, const vec2& grid, const vec2& areaSize);						//Обновление массива отрисовываемых индексов

public:
	void		setPosition(double x, double y);		//Начальная установка координат
	void		setPosition(vec2 pt);					//Начальная установка координат
	vec2		getPosition()const{return m_BottomRight + oldAreaBL;} 	//Получение позиции
	double		getValueAtTime(const double Time) const;	//Получение данных из конкретной записи
	double		getTopPosition() const;						//Координата верхнего угла по высоте
	bool		isBoolean() const;						//Признак логической оси
	void		getStatistic(const double& t0, const double& t1, bool SKO_from_Mid, double& Min, double& Max, double& MO, double& D, double& Sigma, int& nPoints) const;					//Выдача статистической информации
	void		errorsFilter() const;					//Фильтрация сбоев
	void		updateFiltering();						//Фильтрация сигнала
	int			getAxeLength() const {return m_Axe_Length;}
	void		setAxeLength(int len, int highlighted = -1);
};
}

#endif // !GAXE_H
