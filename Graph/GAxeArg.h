#ifndef GAXEARG_H
#define GAXEARG_H

#include "GraphObject.h"

class QCursor;
class QOpenGLShaderProgram;

namespace Graph{
	class GTextLabel;

class GAxeArg : public GraphObject  //Класс оси графика
{

//////////////////////////////////////////////////////////////////////////////////
//		Данные
//////////////////////////////////////////////////////////////////////////////////
private:
	vec2			m_BottomRight;	//Положение нижнего правого угла, мм

	//Данные для OpenGL
	vector<vec2>			m_data;
	QOpenGLShaderProgram*	m_program;
	int		u_modelToWorld;
	int		u_worldToCamera;
	int		u_cameraToView;
	GLuint	gridVBO;
	GLuint	axeVBO;
	int		nCountGrid;
	int		nCountAxe;

	double	oldTime;
	double	oldTimeScale;
	vec2	oldGrid;
	vec2	oldAreaBL;
	vec2	oldAreaSize;
	float	oldScale;
	GTextLabel*		textLabel;

public:
	QString			m_Name;			//Название оси

//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GAxeArg();
	virtual ~GAxeArg();

	//Рисование
	virtual	void	initializeGL();
	virtual void	Draw(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha);					//Полное рисование

	//Мышиные дела
	virtual bool	HitTest(const vec2& /*pt*/){return false;}//Проверка на попадание курсора
	virtual void	MoveOffset(const vec2& /*delta*/, const Qt::MouseButtons& /*buttons*/, const Qt::KeyboardModifiers& /*mdf*/){};	//Перемещение на заданное расстояние
	virtual void	OnDoubleClick(){};				//Реакция на щелчок мышью
	virtual void	OnStartMoving(){};				//Реакция на начало перетаскивания
	virtual void	OnStopMoving() {};				//Реакция на конец перетаскивания
};
}

#endif // !GAXEARG_H
