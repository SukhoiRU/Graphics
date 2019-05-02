#include "stdafx.h"
#include "Accumulation.h"

Accumulation::Accumulation()
{
	m_Type	= Acc_SAPR;
	m_Name	= "";
}

Accumulation::~Accumulation()
{
	//Очищаем заголовок
	for(size_t i = 0; i < m_Header.size(); i++)
	{
		SignalInfo*	pInfo	= m_Header.at(i);
		delete pInfo;
	}
	m_Header.clear();
}
