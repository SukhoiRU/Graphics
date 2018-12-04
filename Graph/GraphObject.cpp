#include "stdafx.h"
#include "GraphObject.h"

namespace Graph{


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