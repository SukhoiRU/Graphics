#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#ifdef _WIN32
#include <Windows.h>
#else
#define BYTE char
#define DWORD uint32_t
#define LPCSTR char*
#endif
#include <vector>
#include <QString>
using namespace std;
class QDomElement;
class QFile;

//Типы накопления
enum	AccType{Acc_SAPR, Acc_TRF, Acc_CCS, Acc_Excell, Acc_KARP, Acc_MIG, Acc_MIG_4, Acc_Orion};

//Обеспечивает накопление наблюдаемых переменных в процессе моделирования
class Accumulation
{
public:

	//Описание уровня вложенности переменной
	struct Level
	{
		int		nIcon;
		QString	Name;
		QString	Comment;
		QString	Rename;
	};
	typedef vector<Level>	LevelsList;

	//Структура элементов описателя накопления
	struct HeaderElement
	{
		qint64		Offset;			//Смещение переменной в векторе
		int			Length;			//Количество точек для КАРП-Р
		double		K;				//Масштабирующий коэффициент для *.mig
		qint64		OrionFilePos;	//Для Ориона начало данных в общем файле
		qint64		OrionFileTime;	//Для Ориона начало времени в общем файле
		LevelsList	Desc;			//Описание переменной

		void	operator = (const HeaderElement& h);
		HeaderElement()
		{
			Offset	= 0;
			Length	= 0;
			K		= 0;
			OrionFilePos	= 0;
			OrionFileTime	= 0;
		}
	};

	typedef vector<HeaderElement>	HeaderList;

	//Класс - хранитель временных файлов
	class TempFileHolder
	{
	public:
		struct Info
		{
			QString	name;
			int		size;
			enum FileType{Sapr_File, TRF_File, CCS_File, CCS_Head_File, Excell_File, MIG_File, Orion_File}	type;
			QString	comment;
		};
		vector<Info>	data;	//Перечень временных файлов

		~TempFileHolder()
		{
			//Удаляем временные файлы
			for(size_t i = 0; i < data.size(); i++)
			{
				QString&	filename	= data[i].name;
				remove(filename.toStdString().c_str());
			}
			data.clear();
		}
	};

	//Структура для большого файла Орион
	struct OrionHead
	{
		QString			name;		//Имя пакета
		qint64			pos;		//Начало пакета
        QDomElement*	attr;		//Описатель пакета
		
		void	operator = (const OrionHead& h)
		{
			name	= h.name;
			pos		= h.pos;
			attr	= h.attr;
		}
	};

	struct OrionData
	{
		qint64	pos;	//Положение в большом файле
		BYTE*	ptr;	//Указатель на считанные данные
	};

private:
    
	HeaderList		m_Header;		//Описатель данных в массиве	
	BYTE*			m_pData;		//Данные	
	int				m_nRecordSize;	//Длина вектора
	int				m_nRecCount;	//Количество записей
	AccType			m_Type;			//Тип накопления
	BYTE*			m_pTRF_Head;	//Заголовок TRF-файла для сохранения куска
	int				m_nTRF_Hsize;	//Размер заголовка TRF-файла
	QString			m_sCCS_Head;	//Заголовок CCS-файла
	QString			m_MiGInfo;		//Информация в заголовке mig-файла
	
	int					m_nOrionVersion;	//Номер версии файла Орион
	QFile*				m_pOrionFile;		//Указатель на единый файл Ориона
	vector<OrionHead>	m_OrionPacketList;	//Перечень пакетов в файле Ориона
	mutable vector<OrionData>	m_OrionData;		//Перечень считанных данных

	void			DwordToBool(bool* Data, const DWORD& Value);	//Преобразование данных
	float			BoolToFloat(const bool* Data) const;

	void			SaveHeader_SAPR(QFile& file, int nRecBegin, int nRecEnd) const;
	void			SaveHeader_TRF(QFile& file, int nRecBegin, int nRecEnd) const;
	void			SaveHeader_CCS(QFile& file, int nRecBegin, int nRecEnd, BYTE* pData, TempFileHolder& holder) const;
	void			LoadOrionPacket();					//Чтение Орион
	void			SaveOrionPart(QFile& file, double Time0, double Time1) const;	//Запись нужного куска
	void			SaveOrionPart_Packet(QFile* pFile, QString& packet_name, double Time0, double Time1) const;	//Запись нужного куска пакета

public:
	Accumulation();
	~Accumulation();

	bool		m_bIsEnabled;	//Признак отображаемого накопления
	QString		m_Name;			//Имя накопления
	
	void	Load(QFile& in);					//Чтение из двоичного файла
	void	LoadTRF(QFile& in);					//Чтение TRF-файлов
	void	LoadCCS(QFile& inf, QFile& dat);	//Чтение DAT-файлов
	void	LoadExcell(QFile& in);				//Чтение текстового файла из формата Су-25СМ
	void	LoadKARP(QFile& in);				//Чтение текстового файла в формате КАРП-Р
	void	LoadMIG(QFile& in);					//Чтение MIG-файлов
	void	LoadOrion(QString& FileName);	//Чтение большого файла Орион
//	void	ConvertOrion(QFile* pFile, QString DirName, QString PacketName, IXMLDOMNode*	attr, bool bFirst, double& ShiftT0, COrionLoadDlg* pDlg);//Перевод Ориона в большой файл

	double*	GetOrionTime(const HeaderElement& h) const;
	BYTE*	GetOrionData(const HeaderElement& h) const;
	void	FreeOrionData();	//Очистка всех выделенных областей памяти под Орион

	//Константный доступ
	const HeaderList&	GetHeader()		const	{return m_Header;}
	const int			GetRecCount()	const	{return m_nRecCount;}
	const int			GetRecSize()	const	{return m_nRecordSize;}
	const BYTE*			GetData()		const	{return m_pData;}
	AccType				GetType()		const	{return m_Type;}
	QString				GetMiGInfo()	const	{return m_MiGInfo;}

	//Получение имен
	QString				GetName(int AccIndex)	const;
	QString				GetPath(int AccIndex)	const;

	//Сохранение части данных
	void	SavePart(QFile& in, double Time0, double Time1, TempFileHolder& holder) const;

	//Управление временем
	void	GetTimeLimits(double* pMin, double* pMax) const;
	void	SetStartTime(double Time0) const;

	//Печать содержимого накопления (имена и данные)
	void	Print(LPCSTR FileName, bool bHead = true);
};

#endif // ACCUMULATION_H
