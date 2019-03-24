#include "stdafx.h"
#include "GraphObject.h"

namespace Graph{

mat4	GraphObject::m_proj;	//Общая матрица проекции
mat4	GraphObject::m_view;	//Общая матрица камеры	
float	GraphObject::m_scale;	//Общий масштаб
double	GraphObject::modelTime	= 0;		//Общее время, с
double	GraphObject::timeStep	= 0;		//Шаг счета, с

GraphObject::GraphObject()
{
	m_bOpenGL_inited	= false;
	m_IsSelected	= false;
	m_IsMoving		= false;
	m_Type			= FIELD;
}

GraphObject::~GraphObject()
{
}

}