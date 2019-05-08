#include "stdafx.h"
#include "TRF_Accumulation.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TRF_Accumulation::TRF_Accumulation()
{
	m_Type			= Acc_TRF;
	m_nRecordSize	= 0;
	m_nRecCount		= 0;
	m_DataPos		= 0;
	m_pTime			= nullptr;
	m_pTRF_Head		= nullptr;
}

TRF_Accumulation::~TRF_Accumulation()
{
	clearData();
}

void	TRF_Accumulation::clearData()
{
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		TrfData*	data	= m_Data.at(i);
		delete	data->ptr;
		delete	data;
	}
	m_Data.clear();

	if(m_pTime)
	{
		delete[] m_pTime;
		m_pTime	= nullptr;
	}

	if(m_pTRF_Head)	
	{
		delete[] m_pTRF_Head;	
		m_pTRF_Head	= nullptr;
	}

}

//Чтение из файла TRF
void	TRF_Accumulation::load(const char* filename)
{
	QTextCodec* codec	= QTextCodec::codecForName("CP866");

	//Для начала все стираем
	m_Header.clear();
	clearData();

	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
		m_pFile	= nullptr;
	}

	//Открываем файл
	m_pFile	= new ifstream();
	m_pFile->open(filename, ifstream::in | ifstream::binary);
	if(m_pFile->fail())
	{
		QString	msg = "Не удалось открыть файл\n";
		msg	+= QString::fromStdString(filename);
		QMessageBox::critical(0, "Чтение Орион", msg);
		return;
	}

	//Читаем заголовок файла
	TrfHead		header;
	TrfParm		param;

	if(m_pFile->readsome((char*)&header, 512) != 512)	return;

	m_nRecCount		= header.rec_numb;
	m_nRecordSize	= header.rec_leng;
    
	//Теперь читаем описания элементов, пропуская время
	QStringList	names;
	for(int i = 0; i < header.parm_numb; i++)
	{
		m_pFile->seekg(512*(i+1));
		m_pFile->read((char*)&param, sizeof(param));

		QString	Name	= codec->toUnicode(param.buf);	
		Name	= Name.replace(param.zero, "");
		Name	= Name.trimmed();
		
		QString	Comm	= codec->toUnicode(param.bufl);	
		Comm	= Comm.replace(param.zero, ""); 
		Comm	= Comm.trimmed();	

		//Ищем такое же имя в списке параметров
		while(names.filter(Name).size())
		{
			Name	+= "_1";
		}
		names.push_back(Name);

		//Описатель
		TrfSignal*		signal	= new TrfSignal;
		signal->path	= Name.toStdString();
		signal->comment	= Comm.toStdString();
		signal->icons.push_back(12);
		signal->Offset	= param.offset;
		
		//Добавляем в список
		m_Header.push_back(signal);
	}

	//Запоминаем заголовок файла для последующего сохранения
	m_nTRF_Hsize	= 512*(header.parm_numb + 1);
	if(m_pTRF_Head)	{ delete[] m_pTRF_Head;	m_pTRF_Head	= 0; }
	m_pTRF_Head		= new char[m_nTRF_Hsize];
	m_pFile->seekg(0);
	m_pFile->read(m_pTRF_Head, m_nTRF_Hsize);

	//Сохраняем положение в файле
	m_DataPos	= m_pFile->tellg();
}

void	TRF_Accumulation::preloadData(QStringList* pAxes)
{
	clearData();
	
	//Проверяем время
	if(!m_pTime)
	{
		m_pTime	= new double[m_nRecCount];
	}

	//Формируем перечень загрузки из файла
	for(size_t i = 0; i < pAxes->size(); i++)
	{
		QString	path	= pAxes->at(i);

		//Ищем путь
		TrfSignal*	signal	= nullptr;
		for(size_t i = 0; i < m_Header.size(); i++)
		{
			SignalInfo*	pInfo	= m_Header.at(i);
			if(m_Name + '\\' + pInfo->path == path.toStdString())
			{
				signal	= static_cast<TrfSignal*>(pInfo);

				TrfData*	data	= new TrfData;
				data->path		= pInfo->path;
				data->offset	= signal->Offset;

				//Уточняем тип данных
				data->type	= Float;	
				data->ptr	= (char*)new float[m_nRecCount];
				m_Data.push_back(data);
				break;
			}
		}
	}

	//Загружаем данные в память
	m_pFile->seekg(m_DataPos);

	const int	BlockSize	= 10000;
	char*	Block	= new char[m_nRecordSize*BlockSize];
	qint64	recLoaded	= 0;
	int		curRec		= 0;
	for(int i = 0; i < m_nRecCount; i++)
	{
		if(++curRec >= recLoaded)
		{
			//Считываем блок
			recLoaded	= m_pFile->readsome(Block, m_nRecordSize*BlockSize)/m_nRecordSize;
			curRec		= 0;
		}
		
		//Берем одну запись
		char*	buf	= (Block + curRec*m_nRecordSize);

		//Разбираем запись
		m_pTime[i]	= *(float*)(buf + 1);
		for(size_t j = 0; j < m_Data.size(); j++)
		{
			TrfData*	data	= m_Data.at(j);
			*((float*)data->ptr + i)	= *(float*)(buf + data->offset);
		}
	}
	delete[] Block;
}

size_t	TRF_Accumulation::getData(const string& path, const double** ppTime, const char** ppData, int* nType) const
{
	//Ищем в загруженных
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		TrfData* data	= m_Data.at(i);
		if(data->path == path)
		{
			*ppTime	= m_pTime;
			*ppData	= data->ptr;
			*nType	= data->type;
			
			return	m_nRecCount;
		}
	}

	//Данные не найдены в списке предварительно загруженных.
	return 0;
}
