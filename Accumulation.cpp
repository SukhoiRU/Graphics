#include "stdafx.h"
#include "Accumulation.h"

Accumulation::Accumulation()
{
    m_Type	= AccType::Acc_SAPR;
	m_Name	= "";
	m_pFile	= nullptr;
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

	//Закрываем файл
	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
		m_pFile	= nullptr;
	}
}
