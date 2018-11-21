#include "stdafx.h"
#include "Accumulation.h"
#include <bitset>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>

extern bool	g_bOrion;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Accumulation::Accumulation()
{
	m_nRecordSize	= 0;
	m_nRecCount		= 0;
	m_pData			= 0;
	m_Type			= Acc_SAPR;
	m_pTRF_Head		= 0;
	m_nTRF_Hsize	= 0;
	m_bIsEnabled	= true;
	m_pOrionFile	= 0;
	m_nOrionVersion	= 0;
}

Accumulation::~Accumulation()
{
	if(m_pData)		{delete[] m_pData; m_pData	= 0;}
	if(m_pTRF_Head)	{delete[] m_pTRF_Head; m_pTRF_Head	= 0;}
	if(m_pOrionFile)
	{
		m_pOrionFile->close();
		delete m_pOrionFile;
		m_pOrionFile	= 0;
	}
	FreeOrionData();
}

void Accumulation::HeaderElement::operator = (const Accumulation::HeaderElement& h)
{
	Offset	= h.Offset;
	Length	= h.Length;
	K		= h.K;
	OrionFilePos	= h.OrionFilePos;
	OrionFileTime	= h.OrionFileTime;
	
	Desc.clear();
	for(size_t pos = 0; pos < h.Desc.size(); pos++)
	{
		const Level&	L	= h.Desc[pos];
		Desc.push_back(L);
	}
}

//Чтение из двоичного файла
void	Accumulation::Load(QFile& in)
{
	m_Type	= Acc_SAPR;
	struct Head
	{
		int		nParams;		//Количество параметров
		int		nRecSize;		//Размер записи
		int		nRecs;			//Количество записей
	} H;

	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	//Читаем заголовок файла
	in.read((char*)&H, sizeof(H));
	m_nRecCount		= H.nRecs;
	m_nRecordSize	= H.nRecSize;
    
	//Теперь читаем описания элементов
	for(int i = 0; i < H.nParams; i++)
	{
		HeaderElement	h;
		QString			Path;
		QString			Icons;

		//Сначала длину строк
		int	nPathLength;	in.read((char*)&nPathLength, sizeof(int));
		int	nIconLength;	in.read((char*)&nIconLength, sizeof(int));
        in.read((char*)&h.Offset, sizeof(qint64));

		//Теперь собственно описание
		char	buf[1024];
		in.read(buf, nPathLength);
		Path	= buf;
		in.read(buf, nIconLength);
		Icons	= buf;

		//Разбиваем строки на части
		QStringList	pathList	= Path.split('\\');
		QStringList	iconList	= Icons.split(',');
		for(int i = 0; i < pathList.size(); i++)
		{
			QString	SubPath	= pathList.at(i);
			QString	SubIcon	= iconList.at(i);

			Level	L;
			L.nIcon	= SubIcon.toInt();
			if(L.nIcon == 12) L.nIcon = 13;
			L.Name	= SubPath;
			h.Desc.push_back(L);
		}
		
		//Добавляем в список
		m_Header.push_back(h);
	}

	//Выделяем массив данных
	if(m_pData)	{delete[] m_pData; m_pData	= 0;}

	if(!m_nRecCount && m_nRecordSize)
	{
		qint64	dwPosition	= in.pos();
		qint64	dwLength	= in.size();
		m_nRecCount	= (dwLength-dwPosition)/m_nRecordSize;
	}

	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];

	in.read((char*)m_pData, m_nRecordSize*m_nRecCount);
}

void	Accumulation::LoadTRF(QFile& trf)
{/*
#pragma pack(push, 1)
			struct TrfHead
			{
				int    		rec_numb;
				short  		rec_leng;
				short  		parm_numb;
				short       fic;
				short       a1;
				short       a2;
				short       a3;
				char        buf[496];
			};

			struct TrfParm
			{
				BYTE		parm_type;
				BYTE		parm_type2;
				BYTE		gpanel, gcol, tpanel, tcol;
				float   	dv;
				short   	offset;
				float   	min,max;
				float   	ak,vk;
				char        buf[9];
				char        zero[9];
				char        bufl[484];
				float		GetValue(const BYTE* buf)
				{
					short a;
					float w;

					switch(parm_type)
					{
					case 1:	{	w	= *(float*)(buf + offset);						}break;
					case 2:	{	a	= *(short*)(buf	+ offset);	w	= a*vk	+ ak;	}break;
					case 4:	{	w	= *(float*)(buf	+ offset);						}break;
					}

					return w;
				}
			};
#pragma pack(pop)

	//Переоткрываем файл
	QString	FileName	= trf.GetFilePath();

	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	m_Type	= Acc_TRF;
	
	//Читаем заголовок файла
	TrfHead		header;
	TrfParm		param;

	if(trf.read((char*)&header,512) != 512)	return;
	
	m_nRecCount		= header.rec_numb;
	m_nRecordSize	= header.rec_leng;
    
	QStringList	NameList;

	FILE*	pFileInfo	= 0;//fopen("trf_info.txt", "rt");
	char	line[4096];

	//Запускаем окошечко
	CWaitCursor	wait;
	CMessDlg*	pDlg	= new CMessDlg;
	pDlg->m_Text	= "Обработка заголовка файла";
	pDlg->Create(IDD_MESS_DLG);
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_SHOW);
	pDlg->RedrawWindow();

	//Теперь читаем описания элементов, пропуская время
	for(int i = 0; i < header.parm_numb; i++)
	{
		HeaderElement	h;
		QString			Path;

		trf.Seek(512*(i+1), QFile::begin);
		trf.read((char*)&param, sizeof(param));

		QString	Name	= param.buf;	Name.Replace(param.zero, ""); Name.TrimRight();	Name.OemToAnsi();
		QString	Comm	= param.bufl;	Comm.Replace(param.zero, ""); Comm.TrimRight();	Comm.OemToAnsi();

		QString str;
		QString	name1;
		QString	Whos;
		if(pFileInfo && (i>1))
		{
			//Есть файл описателя TRF
			str.Format("%d.Краткое наименование: ",i-1);
			do{
				fgets(line, 4096, pFileInfo);
				if(feof(pFileInfo))	
				{
					//Это ошибка!
					AfxMessageBox("Ошибка описателя TRF!!!\n\nОн короче ождаемого...", MB_ICONERROR);
					return;
				}
			}while(memcmp(line, str, str.GetLength()));

			//Нашли нужную строку. Берем имя.
			name1	= line;
			name1.Delete(0,str.GetLength());	name1.TrimRight();

			//Проверка на первые буквы имени
			for(int j = 0; j < Name.GetLength(); j++)
			{
				if(name1[j] != Name[j])
				{
					if(name1[j] != '«')
					{
						//Не совпадают первые буквы имени
						AfxMessageBox("Ошибка описателя TRF!!!\n\nНе совпадают первые буквы имени\n\n" + name1+"\n"+Name, MB_ICONINFORMATION);
					}
				}
			}
			Name	= name1;
			
			//Читаем комментарий
			fgets(line, 4096, pFileInfo);
			str		= "Полное наименование: ";
			Comm	= line;
			Comm.Delete(0,str.GetLength());	Comm.TrimRight();
			
			//Дополнительно определяем принадлежность
			fgets(line, 4096, pFileInfo);
			str		= "Принадлежность: ";
			if(memcmp(line, str, str.GetLength()) == 0)
			{
				Whos	= line;
				Whos.Delete(0,str.GetLength());	Whos.TrimRight();

				//Если она есть, добавляем уровень
				if(!Whos.IsEmpty())
				{
					Level	L		= {9, Whos, ""};
					h.Desc.push_back(L);
				}
			}
		}
		else
		{
			//Ищем такое же имя в списке параметров
			while(NameList.Find(Name))
			{
				Name	+= "_1";
			}
			NameList.AddTail(Name);
		}

		h.Offset		= param.offset;
		Level	L		= {12, Name, Comm};
		h.Desc.push_back(L);

		//Добавляем в список
		m_Header.push_back(h);
	}

	//Закрываем файл описателя
	if(pFileInfo)
	{
		fclose(pFileInfo);
		pFileInfo	= 0;
	}

	//Запоминаем заголовок файла для последующего сохранения
	m_nTRF_Hsize	= 512*(header.parm_numb + 1);
	if(m_pTRF_Head)	{delete[] m_pTRF_Head;	m_pTRF_Head	= 0;}
	m_pTRF_Head		= new BYTE[m_nTRF_Hsize];
	trf.SeekToBegin();
	trf.read((char*)m_pTRF_Head, m_nTRF_Hsize);

	//Определяем максимальный размер данных
	pDlg->m_Text	= "Определение свободной памяти";
	pDlg->UpdateData(false);

	DWORD	nSize		= m_nRecordSize*m_nRecCount;
	DWORD	nOffset		= 0;
	int		nDataMax	= nSize;
	try
	{
		//Попытка сразу открыть файл
		if(m_pData)	{delete[] m_pData;	m_pData	= 0;}
		m_pData	= new BYTE[nSize];
	}
	catch (...)
	{
		//Раз не удалось, то подбираем максимально возможный размер
		for(int i = 100; i < 10000; i += 100)
		{
			try
			{
				if(m_pData)	{delete[] m_pData;	m_pData	= 0;}
				m_pData	= new BYTE[i*1024*1024];
			}
			catch (...)
			{
				i -= 100;
				nDataMax	= i*1024*1024;
				break;
			}
		}
	}

	if(nSize > nDataMax)
	{
		//Запрос процента пропуска
		CSetBigFile	dlg;
		dlg.m_nSize		= nSize;
		dlg.m_nDataMax	= nDataMax;
		m_nRecCount	= 0;
		dlg.DoModal();
		m_nRecCount		= header.rec_numb;

		//Пропускаемое количество записей
		int	nMissed	= (dlg.m_Curpos/32768.)*header.rec_numb;
		nOffset		= nMissed*m_nRecordSize;

		//Новое количество записей
		m_nRecCount	= min(nDataMax/m_nRecordSize, header.rec_numb-nMissed);
		nSize		= m_nRecordSize*m_nRecCount;
	}

	pDlg->m_Text	= "Чтение данных";
	pDlg->UpdateData(false);

	//Выделяем массив данных
	if(m_pData)	{delete[] m_pData; m_pData	= 0;}

	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];
	trf.Seek(m_nTRF_Hsize + nOffset, QFile::begin);
	trf.read((char*)m_pData, nSize);
	trf.Close();

	pDlg->m_Text	= "Обработка времени";
	pDlg->UpdateData(false);

	//Выполняем обработку времени
	float	OldTime	= *(float*) (m_pData+1);
	int	Count	= 0;

	for(int i = 1; i < m_nRecCount; i++)
	{
		float&	t	= *(float*) (m_pData + i*m_nRecordSize + 1);

		if(abs(t-OldTime) > 1)
		{
			//Эта запись битая...
			t	= OldTime;
			continue;
		}

		if(t == OldTime)
		{
			Count++;
			t += 1./8.*Count;
		}
		else
		{
			Count	= 0;
			OldTime	= t;
		}
	}

	//Обрезаем нулевые записи в конце
	while(!*(float*)(m_pData + 1 + (m_nRecCount-1)*m_nRecordSize))
		m_nRecCount--;

	delete	pDlg;*/
}

void	Accumulation::DwordToBool(bool* Data, const DWORD& Value)
{
	DWORD	value	= Value;
	for(int i = 0; i < 32; i++)
	{
		Data[i]	= value & 1;
		value	= value >> 1;
	}
}

float	Accumulation::BoolToFloat(const bool* Data) const
{
	DWORD value			= 0;
	for(int i = 31; i >= 0; i--)
	{
		value	= value << 1;
		if(Data[i]) value	= value | 1;
	}
	return	value;
}

//Структура файла
struct	FileInfo
{
	int		FloatLength;
	int		nBoolBlocks;
	float*	pBoolData;
	FileInfo():FloatLength(0), nBoolBlocks(0), pBoolData(0){};
};

void	Accumulation::LoadCCS(QFile& inf, QFile& dat)
{
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	m_Type	= Acc_CCS;
	
	//Читаем файл описателя
	HeaderList		InfoList;
	vector<FileInfo>	FileInfoList;
	int				nDatReserv	= 0;	//Размер резерва в файле
	int				nAccReserv	= 0;	//Размер резерва в нашем массиве
	{
		char*		buf		= new char[inf.size()+1];
		int			n		= inf.read(buf, inf.size());
		buf[n]	= '\0';
		QString		Inf(buf);
		delete[]	buf;
		
		//Запоминаем описатель для сохранения куска
		m_sCCS_Head	= Inf;

		//Делим описатель на строки
		QStringList	Lines	= Inf.split("\n\r\t");

		//Полученный массив строк формируем в структуру
		HeaderElement	hPath;						//Текущий путь
		int				Offset		= 4;			//Смещение в структуре
		int				level		= 0;			//Уровень вложенности
		int				BoolCount	= 0;			//Количество bool-ов в прошлой структуре
		bool			IsBool		= false;		//Признак bool-ов
		bool			BoolBlock_Finished	= false;//Признак завершившего блока РК

		FileInfo	file_info;
		int			ElementIndex	= 1;
		for(int i = 0; i < Lines.size(); i++)
		{
			//Перебираем строки
			QString	Line		= Lines.at(i);
			QString	NextLine;
			if(i+1 < Lines.size()) NextLine	= Lines.at(i+1);

			//Анализируем вложенность блоков
			if(Line	== "{")  
			{
				level++;	
			}
			else if(Line == "}")	
			{			
				level--;

				//Из пути удаляем прошлый блок
				if(hPath.Desc.size()) hPath.Desc.pop_back();
			}
			else 
			{
				if(NextLine == "{")
				{
					//Строка соответствует новой папке
					Level		l;
					l.nIcon		= 9;	//Система
					l.Name		= Line;
					ElementIndex= 1;

					//Добавляем к текущему пути
					hPath.Desc.push_back(l);
				}
				else 
				{
					if(Line == "RK+")
					{
						//Начало блока bool
						BoolCount	= 0;
						IsBool		= true;
						file_info.nBoolBlocks++;
						BoolBlock_Finished	= false;
 						ElementIndex= 1;
						continue;
					}
					if(Line == "RK-") 
					{
						//Пересчитываем сдвиг bool
						Offset	+= 32-BoolCount;
						IsBool	= false;
						nDatReserv	+= sizeof(float);
						nAccReserv	+= 32*sizeof(bool);

						//Признак законченного блока РК
						BoolBlock_Finished	= true;
						continue;
					}

					//Строка - это новая переменная
					HeaderElement	hInf;

					//Добавляем путь
					for(size_t pos = 0; pos < hPath.Desc.size(); pos++)
					{
						hInf.Desc.push_back(hPath.Desc[pos]);
					}

					//Проверяем bool-float
					if(BoolBlock_Finished)
					{
						BoolBlock_Finished	= false;

						//Сохраняем разделение bool-float
						file_info.pBoolData		= new float[file_info.nBoolBlocks];
						FileInfoList.push_back(file_info);
						
						//Готовим следующий блок
						file_info.FloatLength	= 0;
						file_info.nBoolBlocks	= 0;
						file_info.pBoolData		= 0;
					}

					//Определяем тип переменной
					int	CurSize;	//Текущий тип переменной
					int	CurType;	//Номер иконки (0-Bool, 12-Float)
					if(!IsBool)
					{
						//Float
						CurType	= 12;
						CurSize	= sizeof(float);
						nDatReserv	+= sizeof(float);
						nAccReserv	+= sizeof(float);
						file_info.FloatLength += sizeof(float);
					}
					else
					{
						//Bool
						CurType	= 0;
						CurSize	= sizeof(bool);
						BoolCount++;
					}

					//Добавляем переменную
					Level		l;
					l.nIcon		= CurType;
					l.Name		= Line;
					l.Comment	= QString("%1").arg(ElementIndex);
					if(IsBool)	l.Comment	= QString("%1").arg(30-ElementIndex);
					hInf.Desc.push_back(l);

					//Определяем сдвиг
					hInf.Offset	= Offset;
					Offset		+= CurSize;

					//Заносим переменную
					InfoList.push_back(hInf);
					ElementIndex++;

					//Спецпроверка для КСУ-35
					if(hInf.Desc.front().Name	== "СБИ 7" && ElementIndex == 49)
						ElementIndex	= 57;
					if(hInf.Desc.front().Name	== "СБИ 9" && ElementIndex == 21)
						ElementIndex	= 23;
				}
			}
		}

		//Сохраняем разделение bool-float
		file_info.pBoolData		= new float[file_info.nBoolBlocks];
		FileInfoList.push_back(file_info);
	}
	
	//Формируем перечень записей в файле
	{
		QString	Names[5]	= {"Резерв 1", "Резерв 2", "Резерв 3", "Резерв 4", "Модель"};
		QString	Pref[5]		= {"1:", "2:", "3:", "4:", "5:"};
		for(int i = 0; i < 5; i++)
		{
			Level		L;
			L.nIcon		= 8;
			L.Name		= Names[i];

			for(size_t pos = 0; pos < InfoList.size(); pos++)
			{
				HeaderElement	h;	h	= InfoList[pos];
				HeaderElement	H;
				H.Desc.push_back(L);

				Level&	last	= h.Desc.back();
				last.Name		= Pref[i] + last.Name;

				for(size_t j = 0; j < h.Desc.size(); j++)
					H.Desc.push_back(h.Desc[j]);

				H.Offset	= h.Offset + i*nAccReserv;
				m_Header.push_back(H);
			}
		}
	}

	//Определяем длину собственной записи и записи в файле dat
	m_nRecordSize		= sizeof(float) + 5*nAccReserv;
	int	DatRecLenght	= sizeof(float) + 5*nDatReserv;

	//Определяем количество записей
	int	DatSize	= dat.size();
	m_nRecCount	= DatSize/DatRecLenght;

	//Выделяем массив данных
	if(m_pData)	delete[] m_pData;
	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];

	//Читаем файл данных
	{
		//Для обнуления времени...
		bool	IsFirst	= true;
		float	Time0;
		BYTE*	pPos	= m_pData;
		for(int n = 0; n < m_nRecCount; n++)
		{
			//Читаем время
			dat.read((char*)pPos, sizeof(float));	
			
			//Корректируем его
			if(IsFirst)
			{
				Time0	= *(float*)pPos;
				IsFirst	= false;
			}
 			*(float*)pPos	-= Time0;
			pPos	+= sizeof(float);

			//Чтение резерва
			for(int i = 0; i < 5; i++)
			{
				//Перебор по всем кускам
				for(size_t pos = 0; pos < FileInfoList.size(); pos++)
				{
					FileInfo&	info	= FileInfoList[pos];
					
					//Float
					dat.read((char*)pPos, info.FloatLength);	pPos	+= info.FloatLength;

					//Bool
					dat.read((char*)info.pBoolData, info.nBoolBlocks*sizeof(DWORD));

					//Преобразование bool
					for(int j = 0; j < info.nBoolBlocks; j++)
					{
						DwordToBool((bool*)pPos, info.pBoolData[j]);	pPos	+= 32;
					}
				}
			}
		}

		//Удаляем память bool
		for(size_t pos = 0; pos < FileInfoList.size(); pos++)
		{
			FileInfo&	info	= FileInfoList[pos];
			delete[] info.pBoolData;
		}
	}
}
/*
void	Accumulation::LoadExcell(QFile& in)
{
	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.RemoveAll();

	//Определяем имя файла и переоткрываем его в FILE
	QString	FileName	= in.GetFilePath();
	in.Close();

	FILE*	pFile	= fopen(FileName, "rt");
	if(!pFile)	return;

	m_Type	= Acc_Excell;

	//Читаем заголовок
	char	line[4096];
	fgets(line, 4096, pFile);

	QString	HeadLine	= line;
	{
		//Разделитель ;
		QString	Path;
		int	Offset	= 0;
		int	curPos	= 0;
		Path	= HeadLine.Tokenize(";", curPos);

		//Пропускаем время
		Path	= HeadLine.Tokenize(";", curPos);
		while(Path != "")
		{
			//Вырезаем из пути имя и комментарий
			QString	name	= Path;
			QString	comm;
			int n = name.Find(',');
			if(n != -1)
			{
				comm	= "№ " + name.Left(n);
				name.Delete(0, n+1);
			}
			n = name.ReverseFind(',');
			if(n != -1)	
			{
				comm	+= ", опрос ";
				comm	+= name.Right(name.GetLength()-n-1);
				name	= name.Left(n);
			}

			//Заносим все элементы в список
			HeaderElement	h;
			Offset	+= sizeof(double);
			h.Offset	= Offset;
			Level	L;
			L.nIcon		= 2;
			L.Name		= name;
			L.Comment	= comm;
			h.Desc.AddTail(L);

			m_Header.AddTail(h);

			Path	= HeadLine.Tokenize(";", curPos);
			Path.Trim();
		}
	}

	//Определяем размер данных
	int		nLines	= 0;	
	while(!feof(pFile))	{	fgets(line, 4096, pFile);	nLines++;}

	//Возвращаем файл в начало и убираем заголовок
	fseek(pFile, 0, SEEK_SET);	
	fgets(line, 4096, pFile);

	m_nRecCount		= nLines-1;	//По количеству строк
	m_nRecordSize	= (m_Header.GetCount()+1)*sizeof(double);

	//Выделяем массив данных
	if(m_pData)	delete[] m_pData;
	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];

	//Читаем построчно
	for(int i = 0; i < m_nRecCount; i++)
	{
		double*	pRec	= (double*)(m_pData + i*m_nRecordSize);
		for(int j = 0; j < m_Header.GetCount()+1; j++)
		{
			double*	pVal	= pRec + j;
			fscanf(pFile, "%lf;", pVal);
		}
	}
	fclose(pFile);
}

*/
void	Accumulation::LoadExcell(QFile& in)
{ // ВАРИАНТ ДЛЯ ПРОБЕЛОВ
/*	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	//Определяем имя файла и переоткрываем его в FILE
	QString	FileName	= in.fileName();
	in.close();

	FILE*	pFile	= fopen(FileName.toStdString().c_str(), "rt");
	if(!pFile)	return;

	m_Type	= Acc_Excell;

	//Читаем заголовок
	char	line[4096];
	fgets(line, 4096, pFile);
	
	QString	HeadLine	= line;
	{
		//Разделитель ;
		QString	Path;
		int	Offset	= 0;
		int	curPos	= 0;
		Path	= HeadLine.Tokenize(";", curPos);

		//Пропускаем время
		Path	= HeadLine.Tokenize(";", curPos);
		while(Path != "")
		{
			//Вырезаем из пути имя и комментарий
			QString	name	= Path;
			QString	comm;

			//Заносим все элементы в список
			HeaderElement	h;
			Offset	+= sizeof(double);
			h.Offset	= Offset;
			Level	L;
			L.nIcon		= 2;
			L.Name		= name;
			L.Comment	= "";
			h.Desc.push_back(L);

			m_Header.push_back(h);

			Path	= HeadLine.Tokenize(";", curPos);
			Path.Trim();
		}
	}

	//Определяем размер данных
	int		nLines	= 0;	
	while(!feof(pFile))	{	fgets(line, 4096, pFile);	nLines++;}
	
	//Возвращаем файл в начало и убираем заголовок
	fseek(pFile, 0, SEEK_SET);	
	fgets(line, 4096, pFile);

	m_nRecCount		= nLines-1;	//По количеству строк
	m_nRecordSize	= (m_Header.size()+1)*sizeof(double);

	//Выделяем массив данных
	if(m_pData)	delete[] m_pData;
	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];

	//Читаем построчно
	for(int i = 0; i < m_nRecCount; i++)
	{
		double*	pRec	= (double*)(m_pData + i*m_nRecordSize);
		for(size_t j = 0; j < m_Header.size()+1; j++)
		{
			double*	pVal	= pRec + j;
			fscanf(pFile, "%lf;", pVal);
		}
	}
	fclose(pFile);*/
}

void	Accumulation::LoadKARP(QFile& in)
{
/*	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();
	m_Type	= Acc_KARP;

	//Определяем имя файла и переоткрываем его в FILE
	QString	FileName	= in.GetFilePath();
	if(FileName.Find(".bin") != -1)
	{
		//Читаем как готовый двоичный файл
		int	Count;
		in.read((char*)&Count, sizeof(int));

		//Читаем параметры
		char	buf[1024];
		for(int i = 0; i < Count; i++)
		{
			HeaderElement	H;
			in.read((char*)&H.Offset, sizeof(H.Offset));
			in.read((char*)&H.Length, sizeof(H.Length));

			Level	L;
			L.nIcon	= 2;
			int	size;
			//Читаем имя
			in.read((char*)&size, sizeof(int));
			in.read((char*)buf, size);	
			buf[size]	= 0;
			L.Name		= buf;

			//Читаем комментарий
			in.read((char*)&size, sizeof(int));
			in.read((char*)buf, size);	
			buf[size]	= 0;
			L.Comment	= buf;

			H.Desc.push_back(L);
			m_Header.push_back(H);
		}

		//Читаем буфер данных
		int	size;
		in.read((char*)&size, sizeof(int));
		if(m_pData)	delete[] m_pData;
		m_pData	= new BYTE[size];
		if(m_pData == 0)
		{
			AfxMessageBox("Не хватило памяти под КАРП-Р", MB_ICONERROR);
			return;
		}
		in.read((char*)m_pData, size);
		
		m_nRecCount		= -1;	
		m_nRecordSize	= -1;

		return;
	}
	in.Close();

	FILE*	pFile	= fopen(FileName, "rt");
	if(!pFile)	return;

	char	line[4096];

	//Определяем размер данных
	int		ElemCount	= 0;	//Количество элементов
	int		Length		= 0;	//Длина элемента
	int		Total		= 0;	//Суммарная длина
	int		Offset		= 0;	//Начало записи
	float	Time		= 0;
	float	OldTime;
	float	Value;
	HeaderElement	h;	h.Length	= -1;	//Признак начала
	Level	L;

	while(!feof(pFile))	
	{
		fgets(line, 4096, pFile);
		OldTime	= Time;

		//Проверяем строку
		int	res	= sscanf(line, "%f %f", &Time, &Value);
		if(!res)
		{
			//Это новый элемент. Заносим старый с посчитанной длиной
			if(h.Length != -1)
			{
				h.Offset	= Offset;
				h.Length	= Length;
				h.Desc.clear();
				h.Desc.push_back(L);
				m_Header.push_back(h);
				Offset	+= Length*2*sizeof(float);
				
				Length		= 0;
				L.nIcon		= 2;
				int n		= 0;
				while(line[n] != ';')	if(line[n])	n++; else	break;
				line[n]		= 0;
				L.Name		= line;
				L.Comment	= line+n+2;	L.Comment.TrimRight();
			}
			else
			{
				L.nIcon		= 2;
				int n		= 0;
				while(line[n] != ';')	if(line[n])	n++; else	break;
				line[n]		= 0;
				L.Name		= line;
				L.Comment	= line+n+2;	L.Comment.TrimRight();
				h.Length	= 0;
			}
		}
		else
		{
			//Строка с данными
			if(abs(Time-OldTime) > 100)
			{
				//Это разрыв записи.
				Total	-= Length;
				Length	= 0;
			}

			Length++;
			Total++;
		}
	}
	//Заносим последний элемент
	h.Offset	= Offset;
	h.Length	= Length;
	h.Desc.clear();
	h.Desc.push_back(L);
	m_Header.push_back(h);

	//Выделяем память
	if(m_pData)	delete[] m_pData;
	m_pData	= new BYTE[Total*2*sizeof(float)];
	if(m_pData == 0)
	{
		AfxMessageBox("Не хватило памяти под КАРП-Р", MB_ICONERROR);
		return;
	}

	m_nRecCount		= -1;	
	m_nRecordSize	= -1;

	//Возвращаем файл в начало и читаем заново
	fseek(pFile, 0, SEEK_SET);
	Total	= 0;
	Length	= 0;
	Time	= 0;
	float*	pData	= (float*)m_pData;
	while(!feof(pFile))	
	{
		fgets(line, 4096, pFile);
		OldTime	= Time;

		//Проверяем строку
		int	res	= sscanf(line, "%f %f", &Time, &Value);
		if(res)
		{
			if(abs(Time-OldTime) > 100)
			{
				//Это разрыв записи.
				Total	-= Length;
				Length	= 0;
			}
			*(pData+2*Total)	= Time;	
			*(pData+2*Total+1)	= Value;
			Length++;
			Total++;
		}
		else
		{
			Length	= 0;
		}
	}
	fclose(pFile);

	//Тут же пишем его в двоичном виде
	FileName.Replace(".txt", ".bin");
	QFile	out;
	if(out.Open(FileName, QFile::modeCreate | QFile::modeWrite))
	{
		//Пишем количество параметров
		size_t	Count	= m_Header.size();
		out.write((char*)&Count, sizeof(int));
		
		//Пишем сами параметры
		for(size_t pos = 0; pos < m_Header.size(); pos++)
		{
			HeaderElement&	h	= m_Header[pos];
			out.write((char*)&h.Offset, sizeof(h.Offset));
			out.write((char*)&h.Length, sizeof(h.Length));
			Level&	L	= h.Desc.front();
			int	size	= L.Name.GetLength();
			out.write((char*)&size, sizeof(int));
			out.write((char*)L.Name, size);
			size		= L.Comment.GetLength();
			out.write((char*)&size, sizeof(int));
			out.write((char*)L.Comment, size);
		}

		//И сам буфер данных
		int	size	= Total*2*sizeof(float);
		out.write((char*)&size, sizeof(int));
		out.write((char*)m_pData, size);
		out.Flush();
		out.Close();
	}*/
}

QString	Accumulation::GetName(int AccIndex) const
{
	if(AccIndex == -1) return QString("");

	const Accumulation::LevelsList&	List	= m_Header[AccIndex].Desc;

	//Возврат последнего имени. Для векторов - особый случай
	if(List.size() > 2)
	{
		const Level&	last	= List[List.size()-1];
		const Level&	prev	= List[List.size()-2];
		
		if(prev.nIcon	== 3 || prev.nIcon == 4)
		{
			//Это вектор
			return	prev.Name + "." + last.Name;
		}
		else
			return List.back().Name;
	}
	else	
		return List.back().Name;
}

QString	Accumulation::GetPath(int AccIndex) const
{
	if(AccIndex == -1) return QString("");

	const Accumulation::LevelsList&	List	= m_Header[AccIndex].Desc;
	
	//Получим полный путь
	QString	Path;
	for(size_t pos = 0; pos < List.size(); pos++)
	{
		const Accumulation::Level& L	= List[pos];
		Path += L.Name + "\\";
	}

	return Path;
}

void Accumulation::SavePart(QFile& file, double Time0, double Time1, TempFileHolder& holder) const
{
	if(m_Type == Acc_KARP)	return;

	//Определим отображаемый диапазон записей
	BYTE*	pData		= 0;
	int		nRecBegin	= 0;
	int		nRecEnd		= 0;
	bool	bBegFound	= false;
	
	if(m_Type != Acc_Orion)
	{
		for(int i = 0; i < m_nRecCount; i++)
		{
			//Берем запись
			double	t;
			switch(m_Type)
			{
			case Acc_SAPR:	
			case Acc_MIG:	
			case Acc_Excell:t	= *(double*)(m_pData + i*m_nRecordSize);		break;
			case Acc_TRF:	t	= *(float*) (m_pData + i*m_nRecordSize + 1);	break;
			case Acc_MIG_4:
			case Acc_CCS:	t	= *(float*) (m_pData + i*m_nRecordSize);		break;
			}

			if(t >= Time0 && !bBegFound)
			{
				pData		= (BYTE*)(m_pData + i*m_nRecordSize);
				bBegFound	= true;
				nRecBegin	= i;
				if(nRecBegin)	nRecBegin--;	//На шаг раньше
			}

			if(t >= Time1)
			{
				nRecEnd	= i;
				break;
			}
		}
		if(!nRecEnd)	
		{
			nRecEnd	= m_nRecCount-1;
			if(m_Type == Acc_CCS)	nRecEnd	= m_nRecCount-2;
		}

		//Проверяем, что найдено все корректно
		if((!pData || !bBegFound || !nRecEnd) && (m_Type != Acc_Orion))
		{
			//Сообщение об ошибке
			QString	msg = "Ошибка поиска данных!\nФайл не записан.";
			QMessageBox::critical(0, "Сохранение части данных", msg);
			return;
		}
	}

	//Данные определены
	switch(m_Type)
	{
	case Acc_CCS:
		{
			//Пишем целиком
			SaveHeader_CCS(file, nRecBegin, nRecEnd, pData, holder);
		}break;

	case Acc_SAPR:
		{
			//Пишем заголовок
			SaveHeader_SAPR(file, nRecBegin, nRecEnd);

			//Буфер данных
			file.write((char*)pData, (nRecEnd-nRecBegin)*m_nRecordSize);

			//Сохраняем информацию о файле
			TempFileHolder::Info	info;
			info.name	= file.fileName();
			info.size	= file.size();
			info.type	= TempFileHolder::Info::Sapr_File;
			info.comment= m_Name;
			holder.data.push_back(info);
		}break;

	case Acc_TRF:
		{
			//Пишем заголовок
			SaveHeader_TRF(file, nRecBegin, nRecEnd);
			
			//Буфер данных
			file.write((char*)pData, (nRecEnd-nRecBegin)*m_nRecordSize);

			//Сохраняем информацию о файле
			TempFileHolder::Info	info;
			info.name	= file.fileName();
			info.size	= file.size();
			info.type	= TempFileHolder::Info::TRF_File;
			info.comment= m_Name;
			holder.data.push_back(info);
		}break;

	case Acc_Excell:
		{
			//Пишем заголовок
			SaveHeader_SAPR(file, nRecBegin, nRecEnd);

			//Буфер данных
			file.write((char*)pData, (nRecEnd-nRecBegin)*m_nRecordSize);

			//Сохраняем информацию о файле
			TempFileHolder::Info	info;
			info.name	= file.fileName();
			info.size	= file.size();
			info.type	= TempFileHolder::Info::Excell_File;
			info.comment= m_Name;
			holder.data.push_back(info);
		}break;

	case Acc_MIG:
	case Acc_MIG_4:
		{
			//Пишем заголовок
			if(m_pTRF_Head && m_nTRF_Hsize)	file.write((char*)m_pTRF_Head, m_nTRF_Hsize);
			else	QMessageBox::critical(0, "Ошибка записи заголовка MIG!", "Сохранение части данных");

			//Буфер данных
			file.write((char*)pData, (nRecEnd-nRecBegin)*m_nRecordSize);

			//Сохраняем информацию о файле
			TempFileHolder::Info	info;
			info.name	= file.fileName();
			info.size	= file.size();
			info.type	= TempFileHolder::Info::MIG_File;
			info.comment= m_Name;
			holder.data.push_back(info);
		}break;

	case Acc_Orion:
		{
			SaveOrionPart(file, Time0, Time1);

			//Сохраняем информацию о файле
			TempFileHolder::Info	info;
			info.name	= file.fileName();
			info.size	= file.size();
			info.type	= TempFileHolder::Info::Orion_File;
			info.comment= m_Name;
			holder.data.push_back(info);
		}break;
	}
}

void	Accumulation::SaveHeader_SAPR(QFile& file, int nRecBegin, int nRecEnd) const
{
	struct Head
	{
		int		nParams;		//Количество параметров
		int		nRecSize;		//Размер записи
		int		nRecs;			//Количество записей
	} H;

	H.nParams	= m_Header.size();
	H.nRecs		= nRecEnd - nRecBegin;
	H.nRecSize	= m_nRecordSize;

	file.write((char*)&H, sizeof(H));

	//Пишем описания элементов
	for(size_t pos = 0; pos < m_Header.size(); pos++)
	{
		const HeaderElement&	H		= m_Header[pos];
		const LevelsList&		List	= H.Desc;

		//Получим полный путь
		QString	Path;
		QString	Icons;
		QString	Comment;
		for(size_t pos2 = 0; pos2 < List.size(); pos2++)
		{
			const Accumulation::Level& L	= List[pos2];
			Path	+= L.Name + "\\";
			Icons	+= QString("%1,").arg(L.nIcon);
			Comment	= L.Comment;
		}

		//Описание элемента
		int	LenPath		= Path.length()+1;
		int	LenIcons	= Icons.length()+1;
		file.write((char*)&LenPath, sizeof(int));
		file.write((char*)&LenIcons, sizeof(int));

		file.write((char*)&H.Offset, sizeof(H.Offset));
		file.write((char*)Path.toStdString().c_str(), LenPath);
		file.write((char*)Icons.toStdString().c_str(), LenIcons);

		//Для Ориона дополнительно пишем комментарий
		//if(g_bOrion)
		//{
		//	int	LenComm		= Comment.GetLength()+1;
		//	file.write((char*)&LenComm, sizeof(int));
		//	file.write((char*)Comment, LenComm);
		//}
	}
}

void	Accumulation::SaveHeader_TRF(QFile& file, int nRecBegin, int nRecEnd) const
{
	if(m_pTRF_Head && m_nTRF_Hsize)
	{
		//Изменяем в заголовке количество записей
		*(int*)m_pTRF_Head	= nRecEnd - nRecBegin;

		//Пишем остальное без изменений
		file.write((char*)m_pTRF_Head, m_nTRF_Hsize);
	}
	else
	{
		//Это ошибка!
		QMessageBox::critical(0, "Ошибка записи заголовка TRF!", "Сохранение части данных");
	}
}

void	Accumulation::SaveHeader_CCS(QFile& file, int nRecBegin, int nRecEnd, BYTE* pData, TempFileHolder& holder) const
{
/*	if(m_sCCS_Head.IsEmpty())
	{
		//Это ошибка!
		MessageBox(GetFocus(), "Ошибка записи заголовка CCS!", "Сохранение части данных", MB_ICONERROR);
		return;
	}

	//Пишем файл описателя
	QString	FileInf = file.fileName();
	int		n		= FileInf.ReverseFind('\\');
	FileInf.Insert(n+1, "info_");
	FileInf.Replace(".dat", ".txt");

	QFile	fileINF;
	if(!fileINF.Open(FileInf, QFile::modeCreate | QFile::modeWrite))
	{
		MessageBox(GetFocus(), "Ошибка создания файла описателя!", "Сохранение части данных", MB_ICONERROR);
		return;
	}

	//Пишем в него сохраненный кусок
	fileINF.write((char*)m_sCCS_Head, m_sCCS_Head.GetLength());

	//Добавляем информацию о файле
	TempFileHolder::Info	infoINF;
	infoINF.name	= fileINF.GetFilePath();
	infoINF.size	= fileINF.GetLength();
	infoINF.type	= TempFileHolder::Info::CCS_Head_File;
	holder.data.push_back(infoINF);

	fileINF.Flush();
	fileINF.Close();

	//Пишем данные
	QString	CCS_Head(m_sCCS_Head);
	int	nBoolBlocks	= CCS_Head.Replace("RK+", "RK+");

	//Длина резерва
	int	ReserveLen	= (m_nRecordSize-sizeof(float))/5;

	//Длина части float
	int	FloatLen	= ReserveLen - 32*nBoolBlocks;

	//Длина записи в файле
	int	DatLen		= sizeof(float) + 5*(FloatLen + nBoolBlocks*sizeof(DWORD));

	//////////////////////////////////////////////////////////////////////////
	//Читаем файл описателя
	vector<FileInfo>	FileInfoList;
	{
		QString		Inf(m_sCCS_Head);

		//Делим описатель на строки
		QStringArray	Lines;
		int			curPos	= 0;
		QString		Line	= Inf.Tokenize("\n\r\t", curPos);
		while (Line != "")
		{
			Lines.Add(Line);
			Line	= Inf.Tokenize("\n\r\t", curPos);
		};

		//Полученный массив строк формируем в структуру
		bool			IsBool		= false;		//Признак bool-ов
		bool			BoolBlock_Finished	= false;//Признак завершившего блока РК

		FileInfo	file_info;
		for(int i = 0; i < Lines.GetCount(); i++)
		{
			//Перебираем строки
			QString	Line		= Lines.GetAt(i);
			QString	NextLine;
			if(i+1 < Lines.GetCount()) NextLine	= Lines.GetAt(i+1);

			//Анализируем вложенность блоков
			if(Line	== "{")  
			{
			}
			else if(Line == "}")	
			{			
			}
			else 
			{
				if(NextLine == "{")
				{
				}
				else 
				{
					if(Line == "RK+")
					{
						//Начало блока bool
						IsBool		= true;
						file_info.nBoolBlocks++;
						BoolBlock_Finished	= false;
						continue;
					}
					if(Line == "RK-") 
					{
						//Пересчитываем сдвиг bool
						IsBool	= false;

						//Признак законченного блока РК
						BoolBlock_Finished	= true;
						continue;
					}

					//Проверяем bool-float
					if(BoolBlock_Finished)
					{
						BoolBlock_Finished	= false;

						//Сохраняем разделение bool-float
						FileInfoList.push_back(file_info);

						//Готовим следующий блок
						file_info.FloatLength	= 0;
						file_info.nBoolBlocks	= 0;
						file_info.pBoolData		= 0;
					}

					//Определяем тип переменной
					if(!IsBool)
					{
						//Float
						file_info.FloatLength += sizeof(float);
					}
					else
					{
						//Bool
					}
				}
			}
		}

		//Сохраняем разделение bool-float
		FileInfoList.push_back(file_info);
	}

	//////////////////////////////////////////////////////////////////////////
	//Пишем одиночными записями
	BYTE*	FileRecord	= new BYTE[DatLen];
	for(int i = nRecBegin; i <= nRecEnd; i++)
	{
		//Указатель на МОЮ запись
		BYTE*	pRec	= pData + (i-nRecBegin)*m_nRecordSize;

		//Копируем время
		memcpy(FileRecord, pRec, sizeof(float));

		//Копируем резервы
		for(int j = 0; j < 5; j++)
		{
			int	SBI			= 0;
			int	SBI_file	= 0;
			for(size_t pos = 0; pos < FileInfoList.size(); pos++)
			{
				FileInfo&	file_info	= FileInfoList[pos];

				//Указатель на МОЙ резерв
				BYTE*	pRsrv	= pRec + sizeof(float) + SBI + j*ReserveLen;

				//Указатель на резерв в файле
				BYTE*	pFile	= FileRecord + sizeof(float) + SBI_file + j*(FloatLen + nBoolBlocks*sizeof(DWORD));

				//Часть float
				memcpy(pFile, pRsrv, file_info.FloatLength);

				//Часть bool
				for(int k = 0; k < file_info.nBoolBlocks; k++)
				{
					//Указатель на очередные 32 сигнала
					BYTE*	pBoolBlock	= pRsrv + file_info.FloatLength + k*32;

					//Указатель на float в файле
					BYTE*	pFileBool	= pFile + file_info.FloatLength + k*sizeof(DWORD);

					//Преобразование 32 bool'а в единственный float
					*(float*)pFileBool	= BoolToFloat((const bool*)pBoolBlock);
				}

				SBI	+= file_info.FloatLength;
				SBI	+= file_info.nBoolBlocks*32;

				SBI_file	+= file_info.FloatLength;
				SBI_file	+= file_info.nBoolBlocks*sizeof(DWORD);
			}
		}

		//Пишем запись в файл
		file.write((char*)FileRecord, DatLen);
	}
	delete[] FileRecord;

	//Добавляем информацию о файле
	TempFileHolder::Info	info;
	info.name	= file.GetFilePath();
	info.size	= file.GetLength();
	info.type	= TempFileHolder::Info::CCS_File;
	info.comment= m_Name;
	holder.data.push_back(info);*/
}

void	Accumulation::GetTimeLimits(double* pMin, double* pMax) const
{
	//Определение диапазона времени
	if(!m_pData)	{*pMin	= *pMax	= 0; return;}

	const BYTE*		pFirstRec	= m_pData;
	const BYTE*		pLastRec	= m_pData + (m_nRecCount-1)*m_nRecordSize;

	switch(m_Type)
	{
	case Acc_SAPR:
	case Acc_MIG:
	case Acc_Excell:
		{
			*pMin	= *(double*)(pFirstRec);
			*pMax	= *(double*)(pLastRec);
		}break;

	case Acc_TRF:
		{
			*pMin	= *(float*)(pFirstRec + 1);
			*pMax	= *(float*)(pLastRec + 1);
		}break;

	case Acc_MIG_4:
	case Acc_CCS:
		{
			*pMin	= *(float*)(pFirstRec);
			*pMax	= *(float*)(pLastRec);
		}break;
	}
}

void	Accumulation::SetStartTime(double Time0) const
{
	double	T0;
	double	Tk;
	GetTimeLimits(&T0, &Tk);

	//В цикле по всем записям корректируем время
	switch(m_Type)
	{
	case Acc_SAPR:
	case Acc_MIG:
	case Acc_Excell:
		{
			//Нормальная запись САПР
			for(int i = 0; i < m_nRecCount; i++)
			{
				double&	time	= *(double*)(m_pData + i*m_nRecordSize);
				time	+= Time0 - T0;
			}
		}break;

	case Acc_MIG_4:
	case Acc_CCS:
		{
			//Накопление КСУ
			for(int i = 0; i < m_nRecCount; i++)
			{
				float&	time	= *(float*)(m_pData + i*m_nRecordSize);
				time	+= Time0 - T0;
			}
		}break;

	case Acc_TRF:
		{
			//Значит это TRF
			for(int i = 0; i < m_nRecCount; i++)
			{
				float&	time	= *(float*)(m_pData + 1 + i*m_nRecordSize);
				time	+= Time0 - T0;
			}
		}break;
	}
}


void	Accumulation::Print(LPCSTR FileName, bool bHead/* = true*/)
{/*
	QFile	out;
	if(FAILED(out.Create(FileName, GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS)))
	{
		QString	msg = "Ошибка записи файла накопления!\n";
		msg += "Повторите запись в другой файл.";
		MessageBox(GetFocus(),msg,"Сохранение накопления",MB_OK);
		return;
	}

	QString	file;
	QString	Line;

	//Печатаем разбитый заголовок
	if(bHead)
	{
		bool	bEmpty	= true;	//Признак пустого уровня
		int		i		= 0;	//Текущий уровень
		do{
			bEmpty	= true;
			if(!i)	Line	= "Time\t";
			else	Line	= "\t";

			//Печатаем текущий уровень
			for(size_t pos = 0; pos < m_Header.size(); pos++)
			{
				HeaderElement&	E		= m_Header[pos];
				QString		PartName	= "";
				PartName	= E.Desc[i].Name;
				if(PartName.GetLength()) bEmpty	= false;
				Line.AppendFormat("%s\t", PartName);
			}
			file	+= Line + "\n";

			//Переход на следующий при хотя бы одном непустом
			i++;
		}while(!bEmpty);
	}

	//Распечатываем блок
	for(int i = 0; i < m_nRecCount; i++)
	{
		//Получаем указатель на текущую запись
		BYTE*	pRecord	= m_pData + i*m_nRecordSize;

		//Печатаем данные из текущей записи
		Line.Format("%g\t", *(double*)pRecord);
		for(size_t pos = 0; pos < m_Header.size(); pos++)
		{
			HeaderElement&	E	= m_Header[pos];
			BYTE*	pDatum		= pRecord + E.Offset;
			switch(E.Desc.back().nIcon)
			{
			case 2:		Line.AppendFormat("%g\t", *(double*)pDatum);	break;
			case 1:		Line.AppendFormat("%d\t", *(int*)pDatum);		break;
			case 0:		Line.AppendFormat("%d\t", *(bool*)pDatum);		break;
			default:
				throw;
			}
		}
		file	+= Line + "\n";
	}

	out.write((char*)file, file.GetLength());*/
}

void	Accumulation::LoadMIG(QFile& trf)
{/*
#pragma pack(push, 1)
	struct MiGHead
	{
		QString		REGXXX;		//Признак системы обработки: xxx – имя системы из 3-х любых символов
		QString		PlaneName;	//Имя самолета
		BYTE		Type;		//Тип работы: 1-полет, 2-гонка, 3-контр. запись.
		short		FlightNum;	//Номер полета, гонки или контрольной записи
		QString		FlightDate;	//Дата работы в виде: ддммгг
		QString		Pilot;		//Фамилия летчика
		QString		Weapon;		//Вариант подвесок
		short		nPar;		//Число параметров в каждом кадре файла
	};

	struct MiGParm
	{
		QString		name;
		QString		comm;
		char		type;
		float		min;
		float		max;
	};
#pragma pack(pop)

	//Переоткрываем файл
	QString	FileName	= trf.GetFilePath();

	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	m_Type	= Acc_MIG;

	//Читаем заголовок файла
	MiGHead		header;
	
	char	line[4096];
	trf.read((char*)line, 6);	line[6]	= 0;	header.REGXXX		= line;	header.REGXXX.OemToAnsi();		header.REGXXX.TrimRight();
	trf.read((char*)line, 8);	line[8]	= 0;	header.PlaneName	= line;	header.PlaneName.OemToAnsi();	header.PlaneName.TrimRight();
	trf.read((char*)&header.Type, sizeof(header.Type));
	trf.read((char*)&header.FlightNum, sizeof(header.FlightNum));
	trf.read((char*)line, 6);	line[6]	= 0;	header.FlightDate	= line; header.FlightDate.OemToAnsi();	header.FlightDate.TrimRight();
	trf.read((char*)line, 20);	line[20]= 0;	header.Pilot		= line;	header.Pilot.OemToAnsi();		header.Pilot.TrimRight();
	trf.read((char*)line, 35);	line[35]= 0;	header.Weapon		= line;	header.Weapon.OemToAnsi();		header.Weapon.TrimRight();
	trf.read((char*)&header.nPar, sizeof(header.nPar));

	m_MiGInfo	= "\n\n";
	m_MiGInfo.AppendFormat("Регистратор:\t%s\n", header.REGXXX);
	m_MiGInfo.AppendFormat("Бортовой номер:\t%s\n", header.PlaneName);
	m_MiGInfo.AppendFormat("Полет №:\t%d\n", header.FlightNum);
	m_MiGInfo.AppendFormat("Дата:\t\t%s\n", header.FlightDate);
	m_MiGInfo.AppendFormat("Летчик:\t\t%s\n", header.Pilot);
	m_MiGInfo.AppendFormat("Подвески:\t\t%s\n", header.Weapon);

	MiGParm		param;
	QStringList	NameList;
	m_nRecordSize	= 0;

	//Теперь читаем описания элементов, пропуская время
	for(int i = 0; i < header.nPar; i++)
	{
		HeaderElement	h;
		QString			Path;

		trf.read((char*)line, 6);	line[6]	= 0;	param.name	= line;	param.name.OemToAnsi();	param.name.TrimRight();
		trf.read((char*)line, 6);	line[6]	= 0;	param.comm	= line;	param.comm.OemToAnsi();	param.comm.TrimRight();
		trf.read((char*)&param.type, sizeof(param.type));
		trf.read((char*)&param.min, sizeof(param.min));
		trf.read((char*)&param.max, sizeof(param.max));

		QString	Name	= param.name;
		QString	Comm	= param.comm;
		NameList.AddTail(Name);

		h.Offset		= m_nRecordSize;
		Level	L		= {12, Name, Comm};

		switch(param.type)
		{
		case 'd':	{m_nRecordSize += 8; L.nIcon	= 2;}	break;
		case 'e':	{m_nRecordSize += 4; L.nIcon	= 12;}	break;
		case 'm':	
			{
				m_nRecordSize += 2;
				L.nIcon	= 14;
				h.K	= (max(abs(param.min),abs(param.max)))/32767.;
			}break;
		case 'p':	{m_nRecordSize += 4; L.nIcon	= 1;}	break;
		case 'i':	{m_nRecordSize += 2; L.nIcon	= 14;}	break;
		}

		h.Desc.push_back(L);
		
		if(i == 0)
		{
			//Определяем формат времени
			if(param.type == 'e')	m_Type	= Acc_MIG_4;
		}
		else
		{
			//Добавляем в список
			m_Header.push_back(h);
		}
	}
    
	//Читаем количество кадров в блоке
	int	nCadr	= 0;
	trf.read((char*)&nCadr, sizeof(nCadr));
	
	//Определили длину записи. Считаем их количество
	ULONGLONG	size	= trf.GetLength();
	size_t	head_size	= 80 + 21*header.nPar + 4;

	//Запоминаем заголовок файла для последующего сохранения
	m_nTRF_Hsize	= head_size;
	if(m_pTRF_Head)	{delete[] m_pTRF_Head;	m_pTRF_Head	= 0;}
	m_pTRF_Head		= new BYTE[m_nTRF_Hsize];
	trf.SeekToBegin();
	trf.read((char*)m_pTRF_Head, m_nTRF_Hsize);

	//Определяем количество блоков
	int nRecMax	= (size - head_size)/m_nRecordSize;
	if(nCadr)	m_nRecCount	= nCadr;
	else		m_nRecCount	= nRecMax;

	int delta	= nRecMax - nCadr;
	if(nCadr != 0 && delta != 0)
	{
		QString	msg = "Ошибка чтения файла *.mig!\n";
		msg.AppendFormat("Избыток записей = %d", delta);
		MessageBox(GetFocus(), msg,"Чтение *.mig", MB_OK | MB_ICONERROR);
		return;
	}

	//Выделяем массив данных
	if(m_pData)	{delete[] m_pData; m_pData	= 0;}
	m_pData	= new BYTE[m_nRecordSize*m_nRecCount];
	trf.read((char*)m_pData, m_nRecordSize*m_nRecCount);

	trf.Close();

	//Информация о полете
	MessageBox(GetFocus(), m_MiGInfo,"Чтение *.mig", MB_OK | MB_ICONINFORMATION);*/
}
/*
void	Accumulation::ConvertOrion(QFile* pFile, QString DirName, QString PacketName, IXMLDOMNode*	attr, bool bFirst, double& ShiftT0, COrionLoadDlg* pDlg)
{
	//Выставляем тип
	m_Type	= Acc_Orion;

	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
	m_Header.clear();

	CComPtr<IXMLCDHelper>	Xml;	
	Xml.CoCreateInstance(L"XMLCDHelper.XMLCDHelperImpl");
	IXMLCDHelper*&			pXml	= Xml.p;

	//Набор графиков
	int	Offset	= sizeof(double);
	IXMLDOMNodeList*	list	= 0;
	int	nAxes	= 0;
	HRESULT	hr	= pXml->GetNameList(attr, "Сигнал", &nAxes, &list);
	for(int i = 0; i < nAxes; i++)
	{
		HeaderElement	h;
		Level			L;

		//Берем аттрибут сигнала
		IXMLDOMNode*	axe_attr	= 0;
		hr	= pXml->SetIndex(list, i, &axe_attr);

		//Получаем набор полей
		IXMLDOMNamedNodeMap*	map	= 0;
		hr	= pXml->GetNodeMap(axe_attr, &map);

		//Читаем имя сигнала
		char	buf[1024];
		hr	= pXml->GetNodeItem(map, "Name", buf);
		L.Name	= buf;

		//Ищем замену имени
		if(!FAILED(pXml->GetNodeItem(map, "Rename", buf)))	L.Rename	= buf;
		else												L.Rename	= L.Name;

		//Комментарий
		hr	= pXml->GetNodeItem(map, "Comment", buf);
		L.Comment	= buf;

		//Определяем тип сигнала
		hr	= pXml->GetNodeItem(map, "Size", buf);
		QString	size	= buf;
		hr	= pXml->GetNodeItem(map, "Type", buf);
		QString	type	= buf;

		//Определяем, есть ли "-" в размере
		bool	bMultiBit	= size.Find('-') != -1;

		h.Offset	= Offset;
		if(type	== "Double")
		{
			L.nIcon	= 2;
			Offset	+= sizeof(double);
		}
		else if(bMultiBit && ( 
			type == "UInt16" ||
			type == "Byte" ||
			type == "UInt32" ||
			type == "Int32"
			))
		{
			L.nIcon	= 1;
			Offset	+= sizeof(int);
		}
		else if(!bMultiBit)
		{
			L.nIcon	= 0;
			Offset	+= sizeof(bool);
		}
		else
		{
			int a = 0;
		}

		//Добавляем в список
		Level	L_Packet;
		L_Packet.Name	= PacketName;
		L_Packet.nIcon	= 9;
		h.Desc.push_back(L_Packet);
		h.Desc.push_back(L);
		h.OrionFilePos	= 0;
		h.OrionFileTime	= 0;
		m_Header.push_back(h);
	}

	//Получили размер записи
	m_nRecordSize	= Offset;

	//Запоминаем положение заголовка в файле
	ULONGLONG	head_pos	= pFile->GetPosition();

	//Сразу же пишем заголовок файла
	size_t	nCount	= m_Header.size();
	pFile->write((char*)&nCount, sizeof(nCount));
	for(size_t pos = 0; pos < m_Header.size(); pos++)
	{
		HeaderElement&	H		= m_Header[pos];
		const LevelsList&		List	= H.Desc;

		//Получим полный путь
		QString	Path;
		QString	Icons;
		QString	Comment;
		for(size_t pos2 = 0; pos2 < List.size(); pos2++)
		{
			const Accumulation::Level& L	= List[pos2];
			if(pos2 < List.size())	Path	+= L.Name + "\\";
			else					Path	+= L.Rename + "\\";
			Icons.AppendFormat("%d,",L.nIcon);
			Comment	= L.Comment;
		}

		//Описание элемента
		int	LenPath		= Path.GetLength();
		int	LenIcons	= Icons.GetLength();
		pFile->write((char*)&LenPath, sizeof(int));
		pFile->write((char*)&LenIcons, sizeof(int));

		pFile->write((char*)&H.Offset, sizeof(H.Offset));
		pFile->write((char*)Path, LenPath);
		pFile->write((char*)Icons, LenIcons);

		int	LenComm		= Comment.GetLength();
		pFile->write((char*)&LenComm, sizeof(int));
		pFile->write((char*)Comment, LenComm);

		//Положение в большом файле. Пока ноль
		pFile->write((char*)&H.OrionFilePos, sizeof(H.OrionFilePos));
		pFile->write((char*)&H.OrionFileTime, sizeof(H.OrionFileTime));
	}

	//По первому файлу определяем количество записей
	HeaderElement&	h	= m_Header.front();
	Level&			L	= h.Desc.back();

	QFile	in;
	if(!in.Open(DirName + L.Name + ".dat", QFile::modeRead))
	{
		QString	msg = "Ошибка чтения файла\n";
		msg += L.Name;
		MessageBox(GetFocus(), msg,"Чтение накопления", MB_OK | MB_ICONERROR);
		return;
	}

	m_nRecCount	= in.GetLength()/sizeof(double)/2;
	pFile->write((char*)&m_nRecCount, sizeof(m_nRecCount));

	//Выделям блок памяти для чтения файла целиком
	BYTE*	pData	= new BYTE[in.GetLength()];
	double*	pTime	= new double[m_nRecCount];
	double*	pVal	= new double[m_nRecCount];
	int*	pInt	= new int[m_nRecCount];
	bool*	pBool	= new bool[m_nRecCount];

	if(!pData || !pTime || !pVal || !pInt || !pBool)
	{
		QString	msg = "Невозможно выделить память даже под один сигнал!!!\n";
		msg += L.Name;
		MessageBox(GetFocus(), msg,"Чтение накопления", MB_OK | MB_ICONERROR);
		return;
	}

	in.read((char*)pData, in.GetLength());
	in.Close();

	//По этому же файлу читаем и корректируем время
	for(int i = 0; i < m_nRecCount; i++)
	{
		double	t	= *(double*)(pData + 2*i*sizeof(double));

		//Обработка времени
		if(bFirst && !i)	ShiftT0	= t;
		pTime[i] = t - ShiftT0;
	}
	
	//Запоминаем положение времени для этого пакета
	ULONGLONG	pos_time	= pFile->GetPosition();

	//Запись времени в большой файл
	pFile->write((char*)pTime, m_nRecCount*sizeof(double));

	//Загружаем данные
	pDlg->m_Progress.SetRange32(0, m_Header.size());
	pDlg->m_Progress.SetPos(0);
	pDlg->RedrawWindow();
	int	nProgress	= 0;

	for(size_t i = 0; i < m_Header.size(); i++)
	{
		HeaderElement&	h	= m_Header[i];
		Level&			L	= h.Desc.back();

		QFile	in;
		if(!in.Open(DirName + L.Name + ".dat", QFile::modeRead))
		{
			QString	msg = "Ошибка чтения файла\n";
			msg += L.Name;
			MessageBox(GetFocus(), msg,"Чтение накопления", MB_OK | MB_ICONERROR);
			return;
		}

		in.read((char*)pData, in.GetLength());
		in.Close();

		L.Name	= L.Rename;
		h.OrionFileTime	= pos_time;
		h.OrionFilePos	= pFile->GetPosition();

		//Последовательно переписываем данные
		switch(L.nIcon)
		{
		case 2:	
			{
				for(int i = 0; i < m_nRecCount; i++)
					pVal[i]	= *(double*)(pData + sizeof(double) + 2*i*sizeof(double));
				pFile->write((char*)pVal, m_nRecCount*sizeof(double));
			} break;
		case 1:	
			{
				for(int i = 0; i < m_nRecCount; i++)	
					pInt[i]	= *(double*)(pData + sizeof(double) + 2*i*sizeof(double));
				pFile->write((char*)pInt, m_nRecCount*sizeof(int));
			} break;
		case 0:	
			{
				for(int i = 0; i < m_nRecCount; i++)	
					pBool[i]	= *(double*)(pData + sizeof(double) + 2*i*sizeof(double)) != 0;
				pFile->write((char*)pBool, m_nRecCount*sizeof(bool));
			} break;
		}

		nProgress++;
		pDlg->m_Progress.SetPos(nProgress);
	}

	//Освобождаем буферы памяти
	delete[]	pData;
	delete[]	pTime;
	delete[]	pVal;
	delete[]	pInt;
	delete[]	pBool;

	//Запоминаем, где закончились данные
	ULONGLONG	pos_end	= pFile->GetPosition();

	//Повторно пишем заголовок для сохранения позиций элементов
	if(pFile->Seek(head_pos, QFile::begin) != head_pos)
	{
		int a = 0;
	}
	{
		size_t	nCount	= m_Header.size();
		pFile->write((char*)&nCount, sizeof(nCount));
		for(size_t pos = 0; pos < m_Header.size(); pos++)
		{
			const HeaderElement&	H		= m_Header[pos];
			const LevelsList&		List	= H.Desc;

			//Получим полный путь
			QString	Path	= "";
			QString	Icons	= "";
			QString	Comment	= "";
			for(size_t pos2 = 0; pos2 < List.size(); pos2++)
			{
				const Accumulation::Level& L	= List[pos2];
				Path	+= L.Name + "\\";
				Icons.AppendFormat("%d,",L.nIcon);
				Comment	= L.Comment;
			}

			//Описание элемента
			int	LenPath		= Path.GetLength();
			int	LenIcons	= Icons.GetLength();
			pFile->write((char*)&LenPath, sizeof(int));
			pFile->write((char*)&LenIcons, sizeof(int));

			pFile->write((char*)&H.Offset, sizeof(H.Offset));
			pFile->write((char*)Path, LenPath);
			pFile->write((char*)Icons, LenIcons);

			int	LenComm		= Comment.GetLength();
			pFile->write((char*)&LenComm, sizeof(int));
			pFile->write((char*)Comment, LenComm);

			//Положение в большом файле, ради чего все и затевалось
			pFile->write((char*)&H.OrionFilePos, sizeof(H.OrionFilePos));
			pFile->write((char*)&H.OrionFileTime, sizeof(H.OrionFileTime));
		}
	}

	pFile->write((char*)&m_nRecCount, sizeof(m_nRecCount));

	ULONGLONG	pos_cur	= pFile->GetPosition();
	if(pos_cur != pos_time)
	{
		int a = 0;
	}

	//Восстанавливаем положение в файле
	if(pFile->Seek(pos_end, QFile::begin) != pos_end)
	{
		int a = 0;
	}
}
*/
void	Accumulation::LoadOrion(QString& FileName)
{
	//Выставляем тип
	m_Type	= Acc_Orion;

	//Для начала все стираем
	if(m_pData){delete[] m_pData; m_pData = 0;}
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
		QMessageBox::critical(0,"Чтение Орион", msg);
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

void	Accumulation::LoadOrionPacket()
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
		HeaderElement	h;
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

void	Accumulation::FreeOrionData()
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

BYTE*	Accumulation::GetOrionData(const HeaderElement& h) const
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

double*	Accumulation::GetOrionTime(const HeaderElement& h) const
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

void	Accumulation::SaveOrionPart(QFile& file, double Time0, double Time1) const
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

void	Accumulation::SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const
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
