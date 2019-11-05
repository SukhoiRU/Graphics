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
    for(SignalInfo* pInfo: m_Header)  delete  pInfo;
	m_Header.clear();

	//Закрываем файл
	if(m_pFile)
	{
		if(m_pFile->isOpen())	m_pFile->close();
		delete m_pFile;
		m_pFile	= nullptr;
	}
}
