#include "stdafx.h"
#include "GraphObject.h"

namespace Graph{

mat4	GraphObject::m_proj;	//����� ������� ��������
mat4	GraphObject::m_view;	//����� ������� ������	
float	GraphObject::m_scale;	//����� �������

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