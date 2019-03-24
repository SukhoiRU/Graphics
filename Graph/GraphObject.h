#ifndef GRAPH_OBJECT_H
#define GRAPH_OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::scale;
using glm::translate;
using glm::rotate;
#include <vector>
using std::vector;

namespace Graph{

enum	GraphType {FIELD = 0, MARKER = 1, AXEARG = 2, AXE = 3, TEXT = 4};

class GraphObject
{
//////////////////////////////////////////////////////////////////////////////////
//		Данные
//////////////////////////////////////////////////////////////////////////////////
protected:
	bool			m_bOpenGL_inited;

public:
	bool			m_IsSelected;		//Признак выделения объекта
	bool			m_IsMoving;			//Признак перетаскивания объекта
	GraphType		m_Type;				//Тип объекта (он же признак переднего плана)

	static mat4		m_proj;				//Общая матрица проекции				
	static mat4		m_view;				//Общая матрица камеры
	static float	m_scale;			//Общий масштаб, [пикселей/мм]
	static double	modelTime;			//Общее время, с
	static double	timeStep;			//Шаг счета, с

//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GraphObject();
	virtual ~GraphObject();

    virtual void	initializeGL(){}
    virtual void	clearGL(){}

	//Рисование
    virtual void	Draw(const double /*t0*/, const double /*TimeScale*/, const vec2& /*grid*/, const vec2& /*areaBL*/, const vec2& /*areaSize*/, const float /*alpha*/){}					//Полное рисование
    virtual void	DrawFrame(){}				//Отрисовка только рамки

	//Мышиные дела
    virtual bool	HitTest(const vec2& /*pt*/){return false;}		//Проверка на попадание курсора
	virtual bool	getCursor(const vec2& /*pt*/, Qt::CursorShape& /*shape*/)	{return false;}
    virtual void	MoveOffset(const vec2& /*delta*/, const Qt::MouseButtons& /*buttons*/, const Qt::KeyboardModifiers& /*mdf*/){}	//Перемещение на заданное расстояние
	virtual void	onWheel(const vec2& pt, const Qt::KeyboardModifiers& mdf, vec2 numdegrees){}	//Обработка колеса
    virtual void	OnDoubleClick(){}				//Реакция на щелчок мышью
    virtual void	OnStartMoving(){}				//Реакция на начало перетаскивания
    virtual void	OnStopMoving(){}				//Реакция на конец перетаскивания
};
}

#endif	//GRAPH_OBJECT_H
