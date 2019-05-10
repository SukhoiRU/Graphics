/********************************************************************************
** Form generated from reading UI file 'gaxe_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAXE_DIALOG_H
#define UI_GAXE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "Dialogs/colorbutton.h"

QT_BEGIN_NAMESPACE

class Ui_GAxe_dialog
{
public:
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_Additional;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *lineEdit_Name;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_Min;
    QLabel *label_Max;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButton_Replace;
    QPushButton *pushButton_Substract;
    QPushButton *pushButton_Stat;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout;
    QLabel *label_7;
    QLabel *label_4;
    QLineEdit *lineEdit_Scale;
    QLabel *label_6;
    QLabel *label_5;
    ColorButton *pushButton_Color;
    QLineEdit *lineEdit_Min;
    QLineEdit *lineEdit_Length;
    QLabel *label_8;
    QComboBox *comboStyle;
    QComboBox *comboType;
    QCheckBox *checkBox_Interpol;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *GAxe_dialog)
    {
        if (GAxe_dialog->objectName().isEmpty())
            GAxe_dialog->setObjectName(QStringLiteral("GAxe_dialog"));
        GAxe_dialog->resize(306, 389);
        GAxe_dialog->setSizeGripEnabled(true);
        gridLayout_3 = new QGridLayout(GAxe_dialog);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        pushButton_Additional = new QPushButton(GAxe_dialog);
        pushButton_Additional->setObjectName(QStringLiteral("pushButton_Additional"));

        gridLayout_3->addWidget(pushButton_Additional, 4, 1, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(GAxe_dialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        lineEdit_Name = new QLineEdit(GAxe_dialog);
        lineEdit_Name->setObjectName(QStringLiteral("lineEdit_Name"));
        lineEdit_Name->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lineEdit_Name);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_2 = new QLabel(GAxe_dialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(GAxe_dialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        label_Min = new QLabel(GAxe_dialog);
        label_Min->setObjectName(QStringLiteral("label_Min"));

        gridLayout_2->addWidget(label_Min, 1, 1, 1, 1);

        label_Max = new QLabel(GAxe_dialog);
        label_Max->setObjectName(QStringLiteral("label_Max"));

        gridLayout_2->addWidget(label_Max, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);


        verticalLayout_2->addLayout(verticalLayout);


        gridLayout_3->addLayout(verticalLayout_2, 0, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        pushButton_Replace = new QPushButton(GAxe_dialog);
        pushButton_Replace->setObjectName(QStringLiteral("pushButton_Replace"));
        pushButton_Replace->setMinimumSize(QSize(0, 80));

        verticalLayout_3->addWidget(pushButton_Replace);

        pushButton_Substract = new QPushButton(GAxe_dialog);
        pushButton_Substract->setObjectName(QStringLiteral("pushButton_Substract"));

        verticalLayout_3->addWidget(pushButton_Substract);

        pushButton_Stat = new QPushButton(GAxe_dialog);
        pushButton_Stat->setObjectName(QStringLiteral("pushButton_Stat"));

        verticalLayout_3->addWidget(pushButton_Stat);


        gridLayout_3->addLayout(verticalLayout_3, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 4, 0, 1, 1);

        groupBox = new QGroupBox(GAxe_dialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_4 = new QVBoxLayout(groupBox);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout->addWidget(label_7, 3, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        lineEdit_Scale = new QLineEdit(groupBox);
        lineEdit_Scale->setObjectName(QStringLiteral("lineEdit_Scale"));
        lineEdit_Scale->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEdit_Scale, 1, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 2, 0, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        pushButton_Color = new ColorButton(groupBox);
        pushButton_Color->setObjectName(QStringLiteral("pushButton_Color"));
        pushButton_Color->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(pushButton_Color, 3, 1, 1, 1);

        lineEdit_Min = new QLineEdit(groupBox);
        lineEdit_Min->setObjectName(QStringLiteral("lineEdit_Min"));
        lineEdit_Min->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEdit_Min, 0, 1, 1, 1);

        lineEdit_Length = new QLineEdit(groupBox);
        lineEdit_Length->setObjectName(QStringLiteral("lineEdit_Length"));
        lineEdit_Length->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lineEdit_Length, 2, 1, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        comboStyle = new QComboBox(groupBox);
        comboStyle->setObjectName(QStringLiteral("comboStyle"));

        gridLayout->addWidget(comboStyle, 5, 1, 1, 1);

        comboType = new QComboBox(groupBox);
        comboType->setObjectName(QStringLiteral("comboType"));

        gridLayout->addWidget(comboType, 4, 1, 1, 1);


        verticalLayout_4->addLayout(gridLayout);

        checkBox_Interpol = new QCheckBox(groupBox);
        checkBox_Interpol->setObjectName(QStringLiteral("checkBox_Interpol"));

        verticalLayout_4->addWidget(checkBox_Interpol);


        gridLayout_3->addWidget(groupBox, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(GAxe_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setLayoutDirection(Qt::RightToLeft);
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_3->addWidget(buttonBox, 0, 1, 2, 1);

        QWidget::setTabOrder(lineEdit_Name, lineEdit_Min);
        QWidget::setTabOrder(lineEdit_Min, lineEdit_Scale);
        QWidget::setTabOrder(lineEdit_Scale, lineEdit_Length);
        QWidget::setTabOrder(lineEdit_Length, comboType);
        QWidget::setTabOrder(comboType, comboStyle);
        QWidget::setTabOrder(comboStyle, pushButton_Replace);
        QWidget::setTabOrder(pushButton_Replace, pushButton_Stat);
        QWidget::setTabOrder(pushButton_Stat, pushButton_Additional);
        QWidget::setTabOrder(pushButton_Additional, pushButton_Color);

        retranslateUi(GAxe_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GAxe_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GAxe_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GAxe_dialog);
    } // setupUi

    void retranslateUi(QDialog *GAxe_dialog)
    {
        GAxe_dialog->setWindowTitle(QApplication::translate("GAxe_dialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\260 \320\276\321\201\320\265\320\271", Q_NULLPTR));
        pushButton_Additional->setText(QApplication::translate("GAxe_dialog", "\320\224\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\320\276 >>", Q_NULLPTR));
        label->setText(QApplication::translate("GAxe_dialog", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265", Q_NULLPTR));
        label_2->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\320\272\321\201\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_3->setText(QApplication::translate("GAxe_dialog", "\320\234\320\270\320\275\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_Min->setText(QApplication::translate("GAxe_dialog", "\320\234\320\270\320\275\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_Max->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\320\272\321\201\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        pushButton_Replace->setText(QApplication::translate("GAxe_dialog", "\320\227\320\260\320\274\320\265\320\275\320\260", Q_NULLPTR));
        pushButton_Substract->setText(QApplication::translate("GAxe_dialog", "\320\222\321\213\321\207\320\265\321\201\321\202\321\214", Q_NULLPTR));
        pushButton_Stat->setText(QApplication::translate("GAxe_dialog", "\320\241\321\202\320\260\321\202\320\270\321\201\321\202\320\270\320\272\320\260", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("GAxe_dialog", "\320\236\321\201\321\214", Q_NULLPTR));
        label_7->setText(QApplication::translate("GAxe_dialog", "\320\246\320\262\320\265\321\202", Q_NULLPTR));
        label_4->setText(QApplication::translate("GAxe_dialog", "\320\235\320\260\321\207\320\260\320\273\320\276", Q_NULLPTR));
        label_6->setText(QApplication::translate("GAxe_dialog", "\320\224\320\273\320\270\320\275\320\260", Q_NULLPTR));
        label_5->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261", Q_NULLPTR));
        pushButton_Color->setProperty("text", QVariant(QApplication::translate("GAxe_dialog", "PushButton", Q_NULLPTR)));
        label_8->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\321\200\320\272\320\265\321\200", Q_NULLPTR));
        comboStyle->clear();
        comboStyle->insertItems(0, QStringList()
         << QApplication::translate("GAxe_dialog", "\320\237\321\203\321\201\321\202\320\276\320\271", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\241\320\277\320\273\320\276\321\210\320\275\320\276\320\271", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\241 \320\272\320\276\320\275\321\202\321\203\321\200\320\276\320\274", Q_NULLPTR)
        );
        comboType->clear();
        comboType->insertItems(0, QStringList()
         << QApplication::translate("GAxe_dialog", "\320\232\320\262\320\260\320\264\321\200\320\260\321\202", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\232\321\200\321\203\320\263", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\242\321\200\320\265\321\203\320\263\320\276\320\273\321\214\320\275\320\270\320\272", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\232\321\200\320\265\321\201\321\202", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\227\320\262\320\265\320\267\320\264\320\276\321\207\320\272\320\260", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\250\320\265\320\262\321\200\320\276\320\275", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\232\320\273\320\265\320\262\320\265\321\200", Q_NULLPTR)
        );
        checkBox_Interpol->setText(QApplication::translate("GAxe_dialog", "\320\230\320\275\321\202\320\265\321\200\320\277\320\276\320\273\321\217\321\206\320\270\321\217", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GAxe_dialog: public Ui_GAxe_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAXE_DIALOG_H
