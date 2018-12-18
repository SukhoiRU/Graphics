#include "stdafx.h"
#include "GraphObject.h"

namespace Graph{

mat4	GraphObject::m_proj;	//Общая матрица проекции
mat4	GraphObject::m_view;	//Общая матрица камеры	

GraphObject::GraphObject()
{
	m_IsSelected	= false;
	m_IsMoving		= false;
	m_Type			= FIELD;
}

GraphObject::~GraphObject()
{
}

}