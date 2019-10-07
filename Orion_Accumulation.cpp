#include "stdafx.h"
#include "Orion_Accumulation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Orion_Accumulation::Orion_Accumulation()
{
    m_Type			= AccType::Acc_Orion;
	m_nOrionVersion	= 0;
}

Orion_Accumulation::~Orion_Accumulation()
{
	clearData();
}

void	Orion_Accumulation::load(const QString& filename)
{
	m_Filename	= filename;
	//Для начала все стираем
	m_Header.clear();

	if(m_pFile)
	{
		if(m_pFile->isOpen()) m_pFile->close();
		delete m_pFile;
		m_pFile	= nullptr;
	}
	clearData();
	m_OrionPacketList.clear();

	//Открываем файл
	m_pFile	= new QFile(filename);
	if(!m_pFile)	return;

	if(!m_pFile->open(QIODevice::ReadOnly))
	{
		QString	msg = "Не удалось открыть файл\n";
		msg	+= filename;
		QMessageBox::critical(0, "Чтение Орион", msg);
		return;
	}

	//Читаем количество пакетов в файле
	int	nPackets	= 0;
	m_pFile->read((char*)&nPackets, sizeof(nPackets));

	//Определяем номер версии файла
	m_nOrionVersion	= (nPackets & 0xFFFF0000) >> 16;
	nPackets		= nPackets & 0x0000FFFF;

	QTextCodec* codec	= QTextCodec::codecForName("CP1251");

	//Читаем их список
	for(int i = 0; i < nPackets; i++)
	{
		//Читаем очередной пакет
		OrionHead	h;
		int	nLen	= 0;
		m_pFile->read((char*)&nLen, sizeof(nLen));
		char buf[1024];
		m_pFile->read((char*)buf, nLen);
		buf[nLen]	= 0;

		h.name	= codec->toUnicode(buf);
		m_pFile->read((char*)&h.pos, sizeof(h.pos));

		m_OrionPacketList.push_back(h);
	}

	//Загружаем заголовки пакетов
	for(size_t pos = 0; pos < m_OrionPacketList.size(); pos++)
	{
		const OrionHead&	h	= m_OrionPacketList[pos];
		m_pFile->seek(h.pos);
		LoadOrionPacket();
	}

	//Сохраняем время изменения файла
	QFileInfo	info(*m_pFile);
	m_lastModified	= info.lastModified();
	m_pFile->close();
}

/*
void	Orion_Accumulation::savePart(QFile& file, double Time0, double Time1, TempFileHolder& holder) const
{
		SaveOrionPart(file, Time0, Time1);

		//Сохраняем информацию о файле
		TempFileHolder::Info	info;
		info.name	= file.fileName();
		info.size	= file.size();
		info.type	= TempFileHolder::Info::Orion_File;
		info.comment= m_Name;
		holder.data.push_back(info);
}
*/

void	Orion_Accumulation::LoadOrionPacket()
{
	QTextCodec* codec	= QTextCodec::codecForName("CP1251");

	//Читаем количество сигналов
	int	nCount	= 0;
	m_pFile->read((char*)&nCount, sizeof(nCount));

	//Запоминаем начало списка для старой версии
	size_t	pos_begin;

	for(int i = 0; i < nCount; i++)
	{
		try{
		int	LenPath;
		int	LenIcons;
		char buf[1024];
		QString	Path;
		QString	Icons;
		
		qint64	posBegin	= m_pFile->pos();

		switch(m_nOrionVersion)
		{
		case 0:
		{
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)&LenIcons, sizeof(LenIcons));
			qint64	Offset;
			m_pFile->read((char*)&Offset, sizeof(Offset));
			m_pFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			Path	= codec->toUnicode(buf);
			m_pFile->read((char*)buf, LenIcons);
			buf[LenIcons-1]	= 0;
			Icons	= buf;
		}break;

		case 2:
		{
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			Path	= codec->toUnicode(buf);

			m_pFile->read((char*)&LenIcons, sizeof(LenIcons));
			m_pFile->read((char*)buf, LenIcons);
			buf[LenIcons-1]	= 0;
			Icons	= buf;
		}break;

		default:
			break;
		}
		
		//Формируем описатель сигнала
		OrionSignal*	signal	= new OrionSignal;

		//Путь
		signal->path	= Path;

		//Иконки
		QStringList	iconsList	= Icons.split(",");
		for(auto i = 0; i < iconsList.size(); i++)
		{
			int nIcon	= iconsList.at(i).toInt();
			signal->icons.push_back(nIcon);
		}

		//Комментарий
		int	LenComm;
		m_pFile->read((char*)&LenComm, sizeof(LenComm));
		m_pFile->read((char*)buf, LenComm);
		buf[LenComm]	= 0;
		signal->comment	= codec->toUnicode(buf);

		//Количество точек в сигнале
		if(m_nOrionVersion == 2)
		{
			m_pFile->read((char*)&signal->Length, sizeof(signal->Length));
		}

		m_pFile->read((char*)&signal->OrionFilePos, sizeof(signal->OrionFilePos));
		m_pFile->read((char*)&signal->OrionFileTime, sizeof(signal->OrionFileTime));

		if(m_nOrionVersion == 2)
		{
			//Вычитываем оригинальные путь и имя pwf
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)buf, LenPath);
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)buf, LenPath);

			//Дочитываем до килобайта
			m_pFile->seek(posBegin+1024);
		}

		m_Header.push_back(signal);
		if(i == 0)
		{
			//Запоминаем положение в списке для обновления времени
			pos_begin	= m_Header.size()-1;
		}
		}
		catch(...)
		{
			int a = 0;
		}
	}

	if(m_nOrionVersion == 0)
	{
		//Читаем количество записей в пакете
		int	Length;
		m_pFile->read((char*)&Length, sizeof(Length));

		//Устанавливаем длину всем из этого пакета
		for(size_t pos = pos_begin; pos < m_Header.size(); pos++)
		{
			OrionSignal*	s	= (OrionSignal*)m_Header.at(pos);
			s->Length			= Length;
		}
	}
}

void	Orion_Accumulation::clearData()
{
	//Очистка всех выделенных областей под Орион
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		OrionData*	data	= m_Data[i];
		delete[]	data->ptr;
		delete[]	data->pTime;
		delete		data;
	}

	m_Data.clear();
}

void	Orion_Accumulation::preloadData(const QStringList* pAxes)
{
	clearData();

	//Проверяем файл
	QFileInfo	info(*m_pFile);
	if(info.lastModified() != m_lastModified)
	{
		//Файл был изменен!!!
		load(m_pFile->fileName());
	}

	if(!m_pFile->open(QIODevice::ReadOnly))
	{
		QString	msg = "Не удалось открыть файл\n";
		msg	+= m_pFile->fileName();
		QMessageBox::critical(0, "Чтение Орион", msg);
		return;
	}

	//Формируем перечень загрузки из файла
	for(auto i = 0; i < pAxes->size(); i++)
	{
		const QString&	path	= pAxes->at(i);

		//Ищем путь
		OrionSignal*	signal	= nullptr;
		for(size_t i = 0; i < m_Header.size(); i++)
		{
			SignalInfo*	pInfo	= m_Header.at(i);
			if(m_Name + '\\' + pInfo->path == path)
			{
				signal	= dynamic_cast<OrionSignal*>(pInfo);

				OrionData*	data	= new OrionData;
				data->path	= pInfo->path;
				data->len	= signal->Length;

				//Грузим сигнал
				qint64	size;
				switch(signal->icons.back())
				{
					case 0:		{size	= signal->Length*sizeof(bool);		data->type	= DataType::Bool;}		break;
					case 1:		{size	= signal->Length*sizeof(int);		data->type	= DataType::Int;}		break;
					case 2:		{size	= signal->Length*sizeof(double);	data->type	= DataType::Double;}	break;
					case 12:	{size	= signal->Length*sizeof(float);		data->type	= DataType::Float;}		break;
					case 13:	{size	= signal->Length*sizeof(int);		data->type	= DataType::Int;}		break;
					default:
						continue;
				}

				//Выделяем память
				char*	ptr		= new char[size];
				double*	pTime	= new double[signal->Length];
				if(!ptr)	return;
				if(!pTime)	return;

				//Читаем из файла сигнал и время
				if(!m_pFile->seek(signal->OrionFilePos))		return;
				if(m_pFile->read((char*)ptr, size) != size)		return;
				size	= signal->Length*sizeof(double);
				if(!m_pFile->seek(signal->OrionFileTime))		return;
				if(m_pFile->read((char*)pTime, size) != size)	return;

				//Запоминаем указатель в списке
				data->ptr	= ptr;
				data->pTime	= pTime;

				m_Data.push_back(data);
			}
		}
	}

	m_pFile->close();
}

bool	Orion_Accumulation::getData(const QString& path, size_t* len, const double** ppTime, const char** ppData, DataType* nType) const
{
	//Ищем в загруженных
	for(size_t i = 0; i < m_Data.size(); i++)
	{
		const OrionData* data	= m_Data.at(i);
		if(data->path == path)
		{
			*ppTime	= data->pTime;
			*ppData	= data->ptr;
			*len	= data->len;
			*nType	= data->type;

			return	true;
		}
	}

	//Данные не найдены в списке предварительно загруженных.
	return false;
}

/*
void	Orion_Accumulation::SaveOrionPart(QFile& file, double Time0, double Time1) const
{
	//Копируем список пакетов
	vector<OrionHead>	PacketList;

	for(size_t pos = 0; pos < m_OrionPacketList.size(); pos++)
	{
		const OrionHead&	h	= m_OrionPacketList[pos];
		PacketList.push_back(h);
	}

	//Пишем в файл заголовки пакетов
	size_t	nPackets	= PacketList.size();
	file.write((char*)&nPackets, sizeof(nPackets));

	//Имена пакетов и нулевые позиции
	for(size_t pos = 0; pos < PacketList.size(); pos++)
	{
		OrionHead&	h	= PacketList[pos];
		h.pos	= 0;

		int	n	= h.name.size();
		file.write((char*)&n, sizeof(n));
		file.write((char*)h.name.toStdString().c_str(), n);
		file.write((char*)&h.pos, sizeof(h.pos));
	}

	//Пишем сами пакеты
	for(size_t pos = 0; pos < PacketList.size(); pos++)
	{
		OrionHead&	h	= PacketList[pos];
		
		//Сохраняем положение пакета в файле
		h.pos	= file.pos();

		SaveOrionPart_Packet(&file, h.name, Time0, Time1);
	}

	//Переписываем заголовок большого файла для сохранения позиций пакетов
	file.seek(sizeof(int));
	for(size_t pos = 0; pos < PacketList.size(); pos++)
	{
		OrionHead&	h	= PacketList[pos];

		int	n	= h.name.size();
		file.write((char*)&n, sizeof(n));
		file.write((char*)h.name.toStdString().c_str(), n);
		file.write((char*)&h.pos, sizeof(h.pos));
	}
}

void	Orion_Accumulation::SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const
{
	//Ищем в заголовке элементы с именем пакета и создаем новый список
	HeaderList	packet_list;
	packet_list.clear();

	bool	bFound	= false;
	for(size_t pos = 0; pos < m_Header.size(); pos++)
	{
		const HeaderElement&	h	= m_Header[pos];
		const Level&			L	= h.Desc.front();

		if(L.Name == packet_name)
		{
			bFound	= true;
			packet_list.push_back(h);
		}
		else if(bFound)
		{
			//Учитывая, что элементы в пакете идут последовательно, нет смысла перебирать каждый раз весь список
			break;
		}
	}

	//Запоминаем положение заголовка в файле
	qint64	head_pos	= pFile->pos();

	//Сразу же пишем заголовок файла
	size_t	nCount	= packet_list.size();
	pFile->write((char*)&nCount, sizeof(nCount));

	for(size_t pos = 0; pos < packet_list.size(); pos++)
	{
		HeaderElement&	H		= packet_list[pos];
		const LevelsList&		List	= H.Desc;

		//Получим полный путь
		QString	Path;
		QString	Icons;
		QString	Comment;
		for(size_t pos2 = 0; pos2 < List.size(); pos2++)
		{
			const Accumulation::Level& L	= List[pos2];
			Path	+= L.Name + "\\";
			Icons	+= QString("%1").arg(L.nIcon);
			Comment	= L.Comment;
		}

		//Описание элемента
		int	LenPath		= Path.length();
		int	LenIcons	= Icons.length();
		pFile->write((char*)&LenPath, sizeof(int));
		pFile->write((char*)&LenIcons, sizeof(int));

		pFile->write((char*)&H.Offset, sizeof(H.Offset));
		pFile->write((char*)Path.toStdString().c_str(), LenPath);
		pFile->write((char*)Icons.toStdString().c_str(), LenIcons);

		int	LenComm		= Comment.length();
		pFile->write((char*)&LenComm, sizeof(int));
		pFile->write((char*)Comment.toStdString().c_str(), LenComm);

		//Положение в большом файле. Пока ноль
		pFile->write((char*)&H.OrionFilePos, sizeof(H.OrionFilePos));
		pFile->write((char*)&H.OrionFileTime, sizeof(H.OrionFileTime));
	}

	//Получаем время этого пакета
	const HeaderElement&	h	= packet_list.front();
	double*	pTime	= new double[h.Length];
	m_pFile->seek(h.OrionFileTime);
	m_pFile->read((char*)pTime, h.Length*sizeof(double));
	
	//Ищем индексы для Time0, Time1 методом деления отрезка пополам
	int	nTime0;
	int nTime1;

	int	nMin	= 0;
	int	nMax	= h.Length-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(pTime[n] <= Time0)	nMin	= n;
		else					nMax	= n;
	}
	nTime0	= nMin;
	nMax	= h.Length-1;
	while(nMax - nMin > 1)
	{
		int n	= (nMin+nMax)/2;
		if(pTime[n] <= Time1)	nMin	= n;
		else					nMax	= n;
	}
	nTime1	= nMax;


	//Пишем количество записей в этом пакете
	int	nRecCount	= nTime1-nTime0+1;
	pFile->write((char*)&nRecCount, sizeof(nRecCount));

	//Запоминаем положение времени для этого пакета
	qint64	pos_time	= pFile->pos();

	//Запись времени в большой файл
	pFile->write((char*)pTime+nTime0, nRecCount*sizeof(double));

	//Выделям блок памяти для чтения данных
	double*	pVal	= new double[nRecCount];
	int*	pInt	= new int[nRecCount];
	bool*	pBool	= new bool[nRecCount];

	if(!pTime || !pVal || !pInt || !pBool)
	{
		QString	msg = "Невозможно выделить память даже под один сигнал!!!\n";
		QMessageBox::critical(0, "Запись части накопления", msg);
		return;
	}

	//Запись данных
	for(size_t pos = 0; pos < packet_list.size(); pos++)
	{
		HeaderElement&	h	= packet_list[pos];
		Level&			L	= h.Desc.back();
		
		//Запоминаем метку для нового файла
		qint64	pos_data	= pFile->pos();

		switch(L.nIcon)
		{
		case 2:	
			{
				//Читаем нужный кусок
				m_pFile->seek(h.OrionFilePos + nTime0*sizeof(double));
				m_pFile->read((char*)pVal, nRecCount*sizeof(double));

				//И тут же пишем его
				pFile->write((char*)pVal, nRecCount*sizeof(double));
			} break;
		case 1:	
			{
				//Читаем нужный кусок
				m_pFile->seek(h.OrionFilePos + nTime0*sizeof(int));
				m_pFile->read((char*)pInt, nRecCount*sizeof(int));

				//И тут же пишем его
				pFile->write((char*)pInt, nRecCount*sizeof(int));
			} break;
		case 0:	
			{
				//Читаем нужный кусок
				m_pFile->seek(h.OrionFilePos + nTime0*sizeof(bool));
				m_pFile->read((char*)pBool, nRecCount*sizeof(bool));

				//И тут же пишем его
				pFile->write((char*)pBool, nRecCount*sizeof(bool));
			} break;
		}

		//Перебиваем метки
		h.OrionFileTime	= pos_time;
		h.OrionFilePos	= pos_data;
	}

	//Освобождаем буферы памяти
	delete[]	pTime;
	delete[]	pVal;
	delete[]	pInt;
	delete[]	pBool;

	//Запоминаем, где закончились данные
	qint64	pos_end	= pFile->pos();

	//Повторно пишем заголовок для сохранения позиций элементов
    if(!pFile->seek(head_pos))
    {
        int a = 0;
    }
    {
		size_t	nCount	= packet_list.size();
		pFile->write((char*)&nCount, sizeof(nCount));
		for(size_t pos = 0; pos < packet_list.size(); pos++)
		{
			const HeaderElement&	H		= packet_list[pos];
			const LevelsList&		List	= H.Desc;

			//Получим полный путь
			QString	Path	= "";
			QString	Icons	= "";
			QString	Comment	= "";
			for(size_t pos2 = 0; pos2 < List.size(); pos2++)
			{
				const Accumulation::Level& L	= List[pos2];
				Path	+= L.Name + "\\";
				Icons	+= QString("%1").arg(L.nIcon);
				Comment	= L.Comment;
			}

			//Описание элемента
			int	LenPath		= Path.length();
			int	LenIcons	= Icons.length();
			pFile->write((char*)&LenPath, sizeof(int));
			pFile->write((char*)&LenIcons, sizeof(int));

			pFile->write((char*)&H.Offset, sizeof(H.Offset));
			pFile->write((char*)Path.toStdString().c_str(), LenPath);
			pFile->write((char*)Icons.toStdString().c_str(), LenIcons);

			int	LenComm		= Comment.length();
			pFile->write((char*)&LenComm, sizeof(int));
			pFile->write((char*)Comment.toStdString().c_str(), LenComm);

			//Положение в большом файле, ради чего все и затевалось
			pFile->write((char*)&H.OrionFilePos, sizeof(H.OrionFilePos));
			pFile->write((char*)&H.OrionFileTime, sizeof(H.OrionFileTime));
		}
	}

	pFile->write((char*)&nRecCount, sizeof(nRecCount));

	qint64	pos_cur	= pFile->pos();
    if(pos_cur != pos_time)
    {
        int a = 0;
    }

	//Восстанавливаем положение в файле
    if(!pFile->seek(pos_end))
    {
        int a = 0;
    }
}
*/
