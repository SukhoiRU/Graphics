#include "stdafx.h"
#include "Orion_Accumulation.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Orion_Accumulation::Orion_Accumulation()
{
	m_Type			= Acc_Orion;
	m_nOrionVersion	= 0;
}

Orion_Accumulation::~Orion_Accumulation()
{
	FreeOrionData();
}

void	Orion_Accumulation::load(const char* filename)
{
	//Для начала все стираем
	m_Header.clear();

	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
		m_pFile	= nullptr;
	}
	FreeOrionData();
	m_OrionPacketList.clear();

	//Открываем файл
	m_pFile	= new ifstream();
	m_pFile->open(filename, ifstream::in | ifstream::binary);
 	if(m_pFile->fail())
 	{
 		QString	msg = "Не удалось открыть файл\n";
 		msg	+= QString::fromLocal8Bit(filename);
 		QMessageBox::critical(0, "Чтение Орион", msg);
 		return;
 	}

	//Читаем количество пакетов в файле
	int	nPackets	= 0;
	m_pFile->read((char*)&nPackets, sizeof(nPackets));

	//Определяем номер версии файла
	m_nOrionVersion	= (nPackets & 0xFFFF0000) >> 16;
	nPackets		= nPackets & 0x0000FFFF;

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

		h.name	= buf;
		m_pFile->read((char*)&h.pos, sizeof(h.pos));

		m_OrionPacketList.push_back(h);
	}

	//Загружаем заголовки пакетов
	for(size_t pos = 0; pos < m_OrionPacketList.size(); pos++)
	{
		const OrionHead&	h	= m_OrionPacketList[pos];
		m_pFile->seekg(h.pos);
		LoadOrionPacket();
	}
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
		
		streampos	posBegin	= m_pFile->tellg();

		//Формируем описатель сигнала
		OrionSignal*	signal	= new OrionSignal;

		switch(m_nOrionVersion)
		{
		case 0:
		{
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)&LenIcons, sizeof(LenIcons));
			int64_t	Offset;
			m_pFile->read((char*)&Offset, sizeof(Offset));
			m_pFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			signal->path	= buf;

			m_pFile->read((char*)buf, LenIcons);
			buf[LenIcons-1]	= 0;
		}break;

		case 2:
		{
			m_pFile->read((char*)&LenPath, sizeof(LenPath));
			m_pFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			signal->path	= buf;

			m_pFile->read((char*)&LenIcons, sizeof(LenIcons));
			m_pFile->read((char*)buf, LenIcons);
			buf[LenIcons-1]	= 0;
		}break;

		default:
			break;
		}

		//Убираем запятую в конце
		string	strIcons(buf);
		while(!isdigit(strIcons.back()) && !strIcons.empty())
			strIcons.pop_back();

		//Иконки
		stringstream	ss(strIcons);
		string	icon;
		while(getline(ss, icon, ','))
		{
			int nIcon	= atoi(icon.c_str());
			if(nIcon == 12) nIcon = 13;
			signal->icons.push_back(nIcon);
		}

		//Комментарий
		int	LenComm;
		m_pFile->read((char*)&LenComm, sizeof(LenComm));
		m_pFile->read((char*)buf, LenComm);
		buf[LenComm]	= 0;
		signal->comment	= buf;//codec->toUnicode(buf).toStdString();

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
			m_pFile->seekg(posBegin+(streampos)1024);
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

void	Orion_Accumulation::FreeOrionData()
{
	//Очистка всех выделенных областей под Орион
	for(size_t pos = 0; pos < m_OrionData.size(); pos++)
	{
		OrionData&	data	= m_OrionData[pos];
		if(data.ptr)
		{
			delete[] data.ptr;
			data.ptr	= 0;
		}
	}

	m_OrionData.clear();
}

size_t	Orion_Accumulation::getData(const char* path, const double** ppTime, const char** ppData, int* nType) const
{
	//Ищем путь
	OrionSignal*	signal	= nullptr;
	for(size_t i = 0; i < m_Header.size(); i++)
	{
		SignalInfo*	pInfo	= m_Header.at(i);
		if(pInfo->path == path)
		{
			signal	= static_cast<OrionSignal*>(pInfo);
			break;
		}
	}

	//Проверка на существование
	if(!signal)	return 0;

	//Прежде всего, ищем этот элемент в уже загруженных
	*ppTime	= nullptr;
	*ppData	= nullptr;
	for(size_t pos = 0; pos < m_OrionData.size(); pos++)
	{
		//Ищем время
		const OrionData&	data	= m_OrionData[pos];
		if(signal->OrionFileTime == data.pos)
		{
			*ppTime	= (double*)data.ptr;
			break;
		}
	}

	for(size_t pos = 0; pos < m_OrionData.size(); pos++)
	{
		//Ищем данные
		const OrionData&	data	= m_OrionData[pos];
		if(signal->OrionFilePos == data.pos)
		{
			*ppData	= data.ptr;
			break;
		}
	}

	if(*ppTime == nullptr)
	{
		//Раз не нашли, то загружаем
		qint64	size	= signal->Length*sizeof(double);

		//Выделяем память
		char*	ptr	= new char[size];
		if(!ptr)	return 0;

		//Читаем из большого файла
		if(!m_pFile->seekg(signal->OrionFileTime))	return 0;
		if(!m_pFile->read((char*)ptr, size))		return 0;

		//Запоминаем указатель в списке
		OrionData	d;
		d.pos	= signal->OrionFileTime;
		d.ptr	= ptr;

		m_OrionData.push_back(d);
		*ppTime	= (double*)ptr;
	}

	if(*ppData == nullptr)
	{
		//Раз не нашли, то загружаем
		qint64	size;
		switch(signal->icons.back())
		{
		case 2:	size	= signal->Length*sizeof(double);	break;
		case 1:	size	= signal->Length*sizeof(int);		break;
		case 0:	size	= signal->Length*sizeof(bool);		break;
		}

		//Выделяем память
		char*	ptr	= new char[size];
		if(!ptr)	return 0;

		//Читаем из большого файла
		if(!m_pFile->seekg(signal->OrionFilePos))	return 0;
		if(!m_pFile->read((char*)ptr, size))		return 0;

		//Запоминаем указатель в списке
		OrionData	d;
		d.pos	= signal->OrionFilePos;
		d.ptr	= ptr;

		m_OrionData.push_back(d);
		*ppData	= ptr;
	}

	//Все загружено
	*nType	= signal->icons.back();
	return signal->Length;
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