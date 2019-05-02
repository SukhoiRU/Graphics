#include "stdafx.h"
#include "Orion_Accumulation.h"
#include <bitset>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Orion_Accumulation::Orion_Accumulation()
{
	m_Type			= Acc_Orion;
	m_pOrionFile	= 0;
	m_nOrionVersion	= 0;
}

Orion_Accumulation::~Orion_Accumulation()
{
	if(m_pOrionFile)
	{
		m_pOrionFile->close();
		delete m_pOrionFile;
		m_pOrionFile	= 0;
	}
	FreeOrionData();
}

void	Orion_Accumulation::load(const QString& FileName)
{
	//Выставляем тип
	m_Type	= Acc_Orion;

	//Для начала все стираем
	m_Header.clear();

	if(m_pOrionFile)
	{
		m_pOrionFile->close();
		delete m_pOrionFile;
		m_pOrionFile	= 0;
	}
	FreeOrionData();
	m_OrionPacketList.clear();

	//Открываем файл
	m_pOrionFile	= new QFile(FileName);
	if(!m_pOrionFile)	return;

	if(!m_pOrionFile->open(QIODevice::ReadOnly))
	{
		QString	msg = "Не удалось открыть файл\n";
		msg	+= FileName;
		QMessageBox::critical(0, "Чтение Орион", msg);
		return;
	}

	//Читаем количество пакетов в файле
	int	nPackets	= 0;
	m_pOrionFile->read((char*)&nPackets, sizeof(nPackets));

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
		m_pOrionFile->read((char*)&nLen, sizeof(nLen));
		char buf[1024];
		m_pOrionFile->read((char*)buf, nLen);
		buf[nLen]	= 0;

		h.name	= codec->toUnicode(buf);
		m_pOrionFile->read((char*)&h.pos, sizeof(h.pos));

		m_OrionPacketList.push_back(h);
	}

	//Загружаем заголовки пакетов
	for(size_t pos = 0; pos < m_OrionPacketList.size(); pos++)
	{
		const OrionHead&	h	= m_OrionPacketList[pos];
		m_pOrionFile->seek(h.pos);
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
	QTextCodec* codec	= QTextCodec::codecForName("CP1251");

	//Читаем количество сигналов
	int	nCount	= 0;
	m_pOrionFile->read((char*)&nCount, sizeof(nCount));

	//Запоминаем начало списка
	size_t	pos_begin;

	for(int i = 0; i < nCount; i++)
	{
		try{

		OrionSignal	s;
		int	LenPath;
		int	LenIcons;
		char buf[1024];
		QString	Path;
		QString	Icons;
		
		qint64	posBegin	= m_pOrionFile->pos();

		switch(m_nOrionVersion)
		{
		case 0:
		{
			m_pOrionFile->read((char*)&LenPath, sizeof(LenPath));
			m_pOrionFile->read((char*)&LenIcons, sizeof(LenIcons));
			m_pOrionFile->read((char*)&h.Offset, sizeof(h.Offset));
			m_pOrionFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			Path	= codec->toUnicode(buf);
			m_pOrionFile->read((char*)buf, LenIcons);
			buf[LenIcons]	= 0;
			Icons	= buf;
		}break;

		case 2:
		{
			m_pOrionFile->read((char*)&LenPath, sizeof(LenPath));
			m_pOrionFile->read((char*)buf, LenPath);
			buf[LenPath]	= 0;
			Path	= codec->toUnicode(buf);

			m_pOrionFile->read((char*)&LenIcons, sizeof(LenIcons));
			m_pOrionFile->read((char*)buf, LenIcons);
			buf[LenIcons]	= 0;
			Icons	= buf;
		}break;

		default:
			break;
		}
		
		//Разбиваем строки на части
		QStringList	pathList	= Path.split("\\");
		QStringList	iconsList	= Icons.split(",");
		for(int i = 0; i < pathList.size(); i++)
		{
			QString	SubPath	= pathList.at(i);
			QString	SubIcon	= iconsList.at(i);
			Level	L;
			L.nIcon	= SubIcon.toInt();
			if(L.nIcon == 12) L.nIcon = 13;
			L.Name	= SubPath;
			h.Desc.push_back(L);
		}

	
		int	LenComm;
		m_pOrionFile->read((char*)&LenComm, sizeof(LenComm));
		m_pOrionFile->read((char*)buf, LenComm);
		buf[LenComm]	= 0;

		Level& L	= h.Desc.back();
		L.Comment	= codec->toUnicode(buf);

		if(m_nOrionVersion == 2)
		{
			m_pOrionFile->read((char*)&h.Length, sizeof(h.Length));
		}

		m_pOrionFile->read((char*)&h.OrionFilePos, sizeof(h.OrionFilePos));
		m_pOrionFile->read((char*)&h.OrionFileTime, sizeof(h.OrionFileTime));

		if(m_nOrionVersion == 2)
		{
			//Вычитываем оригинальные путь и имя pwf
			m_pOrionFile->read((char*)&LenPath, sizeof(LenPath));
			m_pOrionFile->read((char*)buf, LenPath);
			m_pOrionFile->read((char*)&LenPath, sizeof(LenPath));
			m_pOrionFile->read((char*)buf, LenPath);

			//Дочитываем до килобайта
			m_pOrionFile->seek(posBegin+1024);
		}

		m_Header.push_back(h);
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
		m_pOrionFile->read((char*)&Length, sizeof(Length));

		//Устанавливаем длину всем из этого пакета
		for(size_t pos = pos_begin; pos < m_Header.size(); pos++)
		{
			HeaderElement&	h	= m_Header[pos];
			h.Length			= Length;
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

BYTE*	Orion_Accumulation::GetOrionData(const HeaderElement& h) const
{
	//Прежде всего ищем этот элемент в уже загруженных
	for(size_t pos = 0; pos < m_OrionData.size(); pos++)
	{
		const OrionData&	data	= m_OrionData[pos];
		if(h.OrionFilePos == data.pos)
			return data.ptr;
	}

	//Раз не нашли, то загружаем
	Level	L	= h.Desc.back();
    qint64	size;
	switch(L.nIcon)
	{
	case 2:	size	= h.Length*sizeof(double);	break;
	case 1:	size	= h.Length*sizeof(int);		break;
	case 0:	size	= h.Length*sizeof(bool);	break;
	}

	//Выделяем память
	BYTE*	ptr	= new BYTE[size];
	if(!ptr)	return 0;

	//Читаем из большого файла
	if(!m_pOrionFile->seek(h.OrionFilePos))	return 0;
	if(m_pOrionFile->read((char*)ptr, size) != size)	return 0;

	//Запоминаем указатель в списке
	OrionData	d;
	d.pos	= h.OrionFilePos;
	d.ptr	= ptr;

	m_OrionData.push_back(d);

	return ptr;
}

const double*	Orion_Accumulation::GetOrionTime(const HeaderElement& h) const
{
	//Прежде всего ищем этот элемент в уже загруженных
	for(size_t pos = 0; pos < m_OrionData.size(); pos++)
	{
		const OrionData&	data	= m_OrionData[pos];
		if(h.OrionFileTime == data.pos)
			return (double*)data.ptr;
	}

	//Раз не нашли, то загружаем
    qint64	size	= h.Length*sizeof(double);

	//Выделяем память
	BYTE*	ptr	= new BYTE[size];
	if(!ptr)	return 0;

	//Читаем из большого файла
	if(!m_pOrionFile->seek(h.OrionFileTime))	return 0;
	if(m_pOrionFile->read((char*)ptr, size) != size)	return 0;

	//Запоминаем указатель в списке
	OrionData	d;
	d.pos	= h.OrionFileTime;
	d.ptr	= ptr;

	m_OrionData.push_back(d);

	return (double*)ptr;
}

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

	//Файл готов!
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
	m_pOrionFile->seek(h.OrionFileTime);
	m_pOrionFile->read((char*)pTime, h.Length*sizeof(double));
	
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
				m_pOrionFile->seek(h.OrionFilePos + nTime0*sizeof(double));
				m_pOrionFile->read((char*)pVal, nRecCount*sizeof(double));

				//И тут же пишем его
				pFile->write((char*)pVal, nRecCount*sizeof(double));
			} break;
		case 1:	
			{
				//Читаем нужный кусок
				m_pOrionFile->seek(h.OrionFilePos + nTime0*sizeof(int));
				m_pOrionFile->read((char*)pInt, nRecCount*sizeof(int));

				//И тут же пишем его
				pFile->write((char*)pInt, nRecCount*sizeof(int));
			} break;
		case 0:	
			{
				//Читаем нужный кусок
				m_pOrionFile->seek(h.OrionFilePos + nTime0*sizeof(bool));
				m_pOrionFile->read((char*)pBool, nRecCount*sizeof(bool));

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
