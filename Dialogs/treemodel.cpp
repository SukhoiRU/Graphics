#include "stdafx.h"
#include "treeitem.h"
#include "treemodel.h"
#include "Accumulation.h"
#include <QtGui>
#include <QtWidgets>

TreeModel::TreeModel(QObject *parent):
QAbstractItemModel(parent)
{
	TreeItem::Data	data;
	data.type	= TreeItem::Item_Chapter;
	data.name	= "Имя";
    data.comm   = "Описание";
	m_rootItem	= new TreeItem(data);

	//Загружаем иконки
	QPixmap		IconSystem;
	QPixmap		IconSystem_Disabled;
	QPixmap		IconSystem_hasDisabled;
	QPixmap		IconContext;
	QPixmap		IconStruct;
	QPixmap		IconBool;
	QPixmap		IconInt;
	QPixmap		IconDouble;
	QPixmap		IconVector2D;
	QPixmap		IconVector3D;
	QPixmap		IconMatrix2D;
	QPixmap		IconMatrix3D;
	QPixmap		IconEnum;
	QPixmap		IconBank;
	QPixmap		IconChapter;

    IconSystem.load(":/Resources/tree/icon_system.bmp");
    IconSystem_Disabled.load(":/Resources/tree/icon_system_off.bmp");
    IconSystem_hasDisabled.load(":/Resources/tree/icon_system_has_off.bmp");
    IconContext.load(":/Resources/tree/icon_context.bmp");
    IconStruct.load(":/Resources/tree/icon_struct.bmp");
    IconBool.load(":/Resources/tree/icon_bool.bmp");
    IconInt.load(":/Resources/tree/icon_int.bmp");
    IconDouble.load(":/Resources/tree/icon_double.bmp");
    IconVector2D.load(":/Resources/tree/icon_vector2D.bmp");
    IconVector3D.load(":/Resources/tree/icon_vector3D.bmp");
    IconMatrix2D.load(":/Resources/tree/icon_matrix2D.bmp");
    IconMatrix3D.load(":/Resources/tree/icon_matrix3D.bmp");
    IconEnum.load(":/Resources/tree/icon_enum.bmp");
    IconBank.load(":/Resources/tree/icon_enum.bmp");
    IconChapter.load(":/Resources/tree/icon_chapter.bmp");

	IconSystem		= IconSystem.scaled(16,16);		IconSystem.setMask(IconSystem.createHeuristicMask());
	IconContext		= IconContext.scaled(16,16);	IconContext.setMask(IconContext.createHeuristicMask());
	IconStruct		= IconStruct.scaled(16,16);		IconStruct.setMask(IconStruct.createHeuristicMask());
	IconBool		= IconBool.scaled(16,16);		IconBool.setMask(IconBool.createHeuristicMask());
	IconInt			= IconInt.scaled(16,16);		IconInt.setMask(IconInt.createHeuristicMask());
	IconDouble		= IconDouble.scaled(16,16);		IconDouble.setMask(IconDouble.createHeuristicMask());
	IconVector2D	= IconVector2D.scaled(16,16);	IconVector2D.setMask(IconVector2D.createHeuristicMask());
	IconVector3D	= IconVector3D.scaled(16,16);	IconVector3D.setMask(IconVector3D.createHeuristicMask());
	IconMatrix2D	= IconMatrix2D.scaled(16,16);	IconMatrix2D.setMask(IconMatrix2D.createHeuristicMask());
	IconMatrix3D	= IconMatrix3D.scaled(16,16);	IconMatrix3D.setMask(IconMatrix3D.createHeuristicMask());
	IconEnum		= IconEnum.scaled(16,16);		IconEnum.setMask(IconEnum.createHeuristicMask());
	IconBank		= IconBank.scaled(16,16);		IconBank.setMask(IconBank.createHeuristicMask());
	IconChapter		= IconChapter.scaled(16,16);	IconChapter.setMask(IconChapter.createHeuristicMask());

	m_IconSystem			= QIcon(IconSystem);
	m_IconContext			= QIcon(IconContext);
	m_IconStruct			= QIcon(IconStruct);
	m_IconBool				= QIcon(IconBool);
	m_IconInt				= QIcon(IconInt);
	m_IconDouble			= QIcon(IconDouble);
	m_IconVector2D			= QIcon(IconVector2D);
	m_IconVector3D			= QIcon(IconVector3D);
	m_IconMatrix2D			= QIcon(IconMatrix2D);
	m_IconMatrix3D			= QIcon(IconMatrix3D);
	m_IconEnum				= QIcon(IconEnum);
	m_IconBank				= QIcon(IconBank);
	m_IconChapter			= QIcon(IconChapter);
}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

void	TreeModel::loadAcc(const vector<Accumulation*>* pBuffer)
{
    for(size_t n = 0; n < pBuffer->size(); n++)
    {
        const Accumulation*	pAcc	= pBuffer->at(n);

        //Создаем новый элемент в корне
        TreeItem::Data	data;
        data.type		= TreeItem::Item_Chapter;
        data.name		= pAcc->name();

        switch (pAcc->type())
        {
        case Accumulation::AccType::Acc_SAPR:      data.comm   = "САПР"; break;
        case Accumulation::AccType::Acc_TRF:       data.comm   = "TRF"; break;
        case Accumulation::AccType::Acc_CCS:       data.comm   = "КСУ"; break;
        case Accumulation::AccType::Acc_Excell:    data.comm   = "Excell"; break;
        case Accumulation::AccType::Acc_MIG:
        case Accumulation::AccType::Acc_MIG_4:     data.comm   = "МиГ"; break;
        case Accumulation::AccType::Acc_Orion:     data.comm   = "Орион"; break;
        default:
            break;
        }
        TreeItem*	pAccItem	= m_rootItem->addChildren(data);

        //Загружаем содержимое
        const vector<Accumulation::SignalInfo*>&	header	= pAcc->header();
        for(size_t i = 0; i < header.size(); i++)
        {
            //Перебираем все элементы заголовка
            TreeItem* pParent	= pAccItem;
            const Accumulation::SignalInfo*	info	= header.at(i);

			QStringList	names	= info->path.split('\\');
			if(names.size() != info->icons.size())	
				continue;

            for(size_t j = 0; j < names.size(); j++)
            {
                //Для каждого элемента перебираем описатель
                const QString&	name	= names.at(j);
				const int		nIcon	= info->icons.at(j);
                
				TreeItem::Data	data;
                data.name		= name;
				//Последнему в пути присваиваем комментарий
				if(j == names.size()-1)	data.comm	= info->comment;
				else					data.comm	= "";

                switch(nIcon)
                {
                case 0: data.type	= TreeItem::Item_Bool; break;
                case 1: data.type	= TreeItem::Item_Int; break;
                case 2: data.type	= TreeItem::Item_Double; break;
                case 3: data.type	= TreeItem::Item_Vector2D; break;
                case 4: data.type	= TreeItem::Item_Vector3D; break;
                case 5: data.type	= TreeItem::Item_Matrix2D; break;
                case 6: data.type	= TreeItem::Item_Matrix3D; break;
                case 7: data.type	= TreeItem::Item_Struct; break;
                case 8: data.type	= TreeItem::Item_Context; break;
                case 9: data.type	= TreeItem::Item_System; break;
    //            case 12: data.type	= TreeItem::Item_Fl; break;
                case 13: data.type	= TreeItem::Item_Enum; break;
                case 14: data.type	= TreeItem::Item_Chapter; break;
                default:
                    break;
                }

                //Ищем, куда прилепить путь
                TreeItem* item	= pParent->findChild(name);
                if(!item)
                {
                    //Не нашли. Создаем новый
                    pParent	= pParent->addChildren(data);
                }
                else
                {
                    //Добавлять не надо. Дальше лепим сюда
                    pParent	= item;
                }
            }
        }
    }
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return m_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())	return QVariant();

	TreeItem*		item	= getItem(index);
	TreeItem::Data*	pData	= item->GetData();

	switch(role)
	{
	case Qt::DisplayRole:
		{
			return item->data(index.column());
		}break;

	case Qt::DecorationRole:
		{
			if(index.column() == 0)
			{
				switch(pData->type)
				{
				case TreeItem::Item_System:		return m_IconSystem;
				case TreeItem::Item_Context:	return m_IconContext;
				case TreeItem::Item_Struct:		return m_IconStruct;
				case TreeItem::Item_Bool:		return m_IconBool;
				case TreeItem::Item_Int:		return m_IconInt;
				case TreeItem::Item_Double:		return m_IconDouble;
				case TreeItem::Item_Vector2D:	return m_IconVector2D;
				case TreeItem::Item_Vector3D:	return m_IconVector3D;
				case TreeItem::Item_Matrix2D:	return m_IconMatrix2D;
				case TreeItem::Item_Matrix3D:	return m_IconMatrix3D;
				case TreeItem::Item_Enum:		return m_IconEnum;
				case TreeItem::Item_Bank:		return m_IconBank;
				case TreeItem::Item_Chapter:	return m_IconChapter;
				default:
					return QVariant();
				}
			}
		}break;

	case Qt::TextAlignmentRole:
		{
			if(index.column() == 0)	return (Qt::AlignLeft + Qt::AlignVCenter);
            else					return (Qt::AlignLeft + Qt::AlignVCenter);
		}break;
	}

	return QVariant();
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */) const
{
	switch(role)
	{
	case Qt::DisplayRole:
	{
		switch(section)
		{
		case 0: return m_rootItem->GetData()->name;
		case 1: return m_rootItem->GetData()->comm;
		default:	return QVariant();
		}
	}break;

	default:
		break;
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())	return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; 
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if(index.isValid()) 
	{
        TreeItem*	item	= static_cast<TreeItem*>(index.internalPointer());
        if(item) return item;
    }
    return m_rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
		return QModelIndex();

	TreeItem*	parentItem	= getItem(parent);
	if(!parentItem->childCount())	return QModelIndex();
	TreeItem*	childItem	= parentItem->child(row);
    
	if(childItem)	return createIndex(row, column, childItem);
    else			return QModelIndex();
}

QModelIndex	TreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())	return QModelIndex();

    TreeItem*	childItem	= getItem(index);
    TreeItem*	parentItem	= childItem->parent();

    if(parentItem == m_rootItem)	return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem*	parentItem	= getItem(parent);

    return parentItem->childCount();
}

void	TreeModel::UpdateModel()
{
	emit layoutChanged();
}
