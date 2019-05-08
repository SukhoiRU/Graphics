#include "stdafx.h"
#include "Sapr_Accumulation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Sapr_Accumulation::Sapr_Accumulation()
{
	m_Type			= Acc_SAPR;
	m_nRecordSize	= 0;
	m_nRecCount		= 0;
	m_DataPos		= 0;
	m_pTime			= nullptr;
}

Sapr_Accumulation::~Sapr_Accumulation()
{
	clearData();
}

void	Sapr_Accumulation::clearData()
{
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		SaprData*	data	= m_Data.at(i);
		delete	data->ptr;
		delete	data;
	}
	m_Data.clear();

	if(m_pTime)
	{
		delete[] m_pTime;
		m_pTime	= nullptr;
	}
}

//Чтение из файла САПР
void	Sapr_Accumulation::load(const QString& filename)
{
	struct Head
	{
		int		nParams;		//Количество параметров
		int		nRecSize;		//Размер записи
		int		nRecs;			//Количество записей
	} H;

	QTextCodec* codec	= QTextCodec::codecForName("CP1251");

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
	m_pFile	= new QFile(filename);
	if(!m_pFile)	return;

	if(!m_pFile->open(QIODevice::ReadOnly))
	{
		QString	msg = "Не удалось открыть файл\n";
		msg	+= filename;
		QMessageBox::critical(0, "Чтение САПР", msg);
		return;
	}

	//Читаем заголовок файла
	m_pFile->read((char*)&H, sizeof(H));
	m_nRecCount		= H.nRecs;
	m_nRecordSize	= H.nRecSize;
    
	//Теперь читаем описания элементов
	for(int i = 0; i < H.nParams; i++)
	{
		SaprSignal*		signal	= new SaprSignal;
		QString			Path;
		QString			Icons;

		//Сначала длину строк
		int	nPathLength;	m_pFile->read((char*)&nPathLength, sizeof(int));
		int	nIconLength;	m_pFile->read((char*)&nIconLength, sizeof(int));
        m_pFile->read((char*)&signal->Offset, sizeof(int));

		//Теперь собственно описание
		char	buf[1024];
		m_pFile->read(buf, nPathLength);
		buf[nPathLength]	= 0;
		
		//Удаляем лидирующий слеш
		if(buf[0] == '\\')	Path	= codec->toUnicode(buf+1);
		else				Path	= codec->toUnicode(buf);
		
		m_pFile->read(buf, nIconLength);
		buf[nIconLength]	= 0;
		//Убираем запятую в конце
		string	strIcons(buf);
		while(!isdigit(strIcons.back()) && !strIcons.empty())
			strIcons.pop_back();
		Icons	= QString::fromStdString(strIcons);

		//Путь
		signal->path	= Path;

		//Иконки
		QStringList	iconsList	= Icons.split(",");
		for(size_t i = 0; i < iconsList.size(); i++)
		{
			int nIcon	= iconsList.at(i).toInt();
			if(nIcon == 12) nIcon = 13;
			signal->icons.push_back(nIcon);
		}
		
		//Добавляем в список
		m_Header.push_back(signal);
	}

	//Сохраняем положение в файле
	m_DataPos	= m_pFile->pos();

	//Восстанавливаем количество записей при ошибке заголовка
	if(!m_nRecCount && m_nRecordSize)
	{
		qint64	dwPosition	= m_pFile->pos();
		qint64	dwLength	= m_pFile->size();
		m_nRecCount	= (dwLength-dwPosition)/m_nRecordSize;
	}
}

void	Sapr_Accumulation::preloadData(QStringList* pAxes)
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
		SaprSignal*	signal	= nullptr;
		for(size_t i = 0; i < m_Header.size(); i++)
		{
			SignalInfo*	pInfo	= m_Header.at(i);
			if(m_Name + '\\' + pInfo->path == path)
			{
				signal	= static_cast<SaprSignal*>(pInfo);

				SaprData*	data	= new SaprData;
				data->path		= pInfo->path;
				data->offset	= signal->Offset;

				//Уточняем тип данных
				switch(signal->icons.back())
				{
					case 0:		{data->type	= Bool;		data->ptr	= (char*)new bool[m_nRecCount];}	break;
					case 1:		{data->type	= Int;		data->ptr	= (char*)new int[m_nRecCount];}		break;
					case 2:		{data->type	= Double;	data->ptr	= (char*)new double[m_nRecCount];}	break;
					case 12:	{data->type	= Float;	data->ptr	= (char*)new float[m_nRecCount];}	break;
					case 13:	{data->type	= Int;		data->ptr	= (char*)new int[m_nRecCount];}		break;
					case 14:	{data->type	= Short;	data->ptr	= (char*)new short[m_nRecCount];}	break;
					default:	throw;
				};
				m_Data.push_back(data);
				break;
			}
		}
	}

	//Загружаем данные в память
	m_pFile->seek(m_DataPos);

	const int	BlockSize	= 10000;
	char*	Block	= new char[m_nRecordSize*BlockSize];
	qint64	recLoaded	= 0;
	int		curRec		= 0;
	for(int i = 0; i < m_nRecCount; i++)
	{
		if(++curRec >= recLoaded)
		{
			//Считываем блок
			recLoaded	= m_pFile->read(Block, m_nRecordSize*BlockSize)/m_nRecordSize;
			curRec		= 0;
		}
		
		//Берем одну запись
		char*	buf	= (Block + curRec*m_nRecordSize);

		//Разбираем запись
		m_pTime[i]	= *(double*)buf;
		for(size_t j = 0; j < m_Data.size(); j++)
		{
			SaprData*	data	= m_Data.at(j);
			switch(data->type)
			{
				case Bool:		*((bool*)data->ptr + i)		= *(bool*)(buf + data->offset); 	break;
				case Int:		*((int*)data->ptr + i)		= *(int*)(buf + data->offset); 		break;
				case Double:	*((double*)data->ptr + i)	= *(double*)(buf + data->offset); 	break;
				case Float:		*((float*)data->ptr + i)	= *(float*)(buf + data->offset);	break;
				case Short:		*((short*)data->ptr + i)	= *(short*)(buf + data->offset);	break;
				default:
					break;
			}
		}
	}
	delete[] Block;
}

size_t	Sapr_Accumulation::getData(const QString& path, const double** ppTime, const char** ppData, int* nType) const
{
	//Ищем в загруженных
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		SaprData* data	= m_Data.at(i);
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
