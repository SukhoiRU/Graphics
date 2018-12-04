#ifndef GRAPH_OBJECT_H
#define GRAPH_OBJECT_H
#include <glm/glm.hpp>
using glm::vec2;

namespace Graph{

enum	GraphType {FIELD = 0, MARKER = 1, AXEARG = 2, AXE = 3, TEXT = 4};

class GraphObject  
{
//////////////////////////////////////////////////////////////////////////////////
//		Данные
//////////////////////////////////////////////////////////////////////////////////
public:
	bool			m_IsSelected;						//Признак выделения объекта
	bool			m_IsMoving;							//Признак перетаскивания объекта
	GraphType		m_Type;								//Тип объекта (он же признак переднего плана)

//////////////////////////////////////////////////////////////////////////////////
//		Методы
//////////////////////////////////////////////////////////////////////////////////
public:
	//Конструктор-деструктор
	GraphObject();
	virtual ~GraphObject();

	virtual void	UpdateRecord(bool bLoad = true){};					//Обновление данных о массиве

	//Рисование
	virtual void	Draw(){};					//Полное рисование
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