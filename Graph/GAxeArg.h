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
	QString			m_Name;		//Название оси
	GLfloat			m_y;		//Положение по высоте, мм

//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GAxeArg();
	virtual ~GAxeArg();

	//Рисование
	virtual	void	initializeGL() override;
	virtual void	draw(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha) override;		//Полное рисование
	virtual void	drawFrame(const double t0, const double TimeScale, const vec2& grid, const vec2& areaBL, const vec2& areaSize, const float alpha) override;	//Только шкала

	//Мышиные дела
	virtual bool	hitTest(const vec2& /*pt*/) override;//Проверка на попадание курсора
	virtual bool	getCursor(const vec2& pt, Qt::CursorShape& shape) override;
	virtual void	moveOffset(const vec2& /*delta*/, const Qt::MouseButtons& /*buttons*/, const Qt::KeyboardModifiers& /*mdf*/) override;	//Перемещение на заданное расстояние
};
}

#endif // !GAXEARG_H
