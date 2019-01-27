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

class GraphObject// : public QOpenGLFunctions_4_3_Core
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


//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GraphObject();
	virtual ~GraphObject();

	virtual void	initializeGL(){};
	virtual void	clearGL(){};
	virtual void	UpdateRecord(bool bLoad = true){};					//Обновление данных о массиве

	//Рисование
	virtual void	Draw(const double t0, const double TimeScale, const QSizeF& grid, const QRectF& area){};					//Полное рисование
	virtual void	DrawFrame(){};				//Отрисовка только рамки

	//Мышиные дела
	virtual bool	HitTest(const vec2& pt){return false;};		//Проверка на попадание курсора
	virtual bool	MoveOffset(const vec2& pt, int nFlags){return false;};	//Перемещение на заданное расстояние
	virtual void	OnDoubleClick(){};				//Реакция на щелчок мышью
	virtual void	OnStartMoving(){};				//Реакция на начало перетаскивания
	virtual void	OnStopMoving(){};				//Реакция на конец перетаскивания
};
}

#endif	//GRAPH_OBJECT_H