/********************************************************************************
** Form generated from reading UI file 'panelselect.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANELSELECT_H
#define UI_PANELSELECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PanelSelect
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonDelete;
    QPushButton *pushButtonCopy;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *PanelSelect)
    {
        if (PanelSelect->objectName().isEmpty())
            PanelSelect->setObjectName(QStringLiteral("PanelSelect"));
        PanelSelect->resize(754, 23);
        horizontalLayout = new QHBoxLayout(PanelSelect);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(PanelSelect);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(PanelSelect);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setMinimumSize(QSize(100, 0));
        comboBox->setBaseSize(QSize(100, 0));
        comboBox->setEditable(true);

        horizontalLayout->addWidget(comboBox);

        pushButtonAdd = new QPushButton(PanelSelect);
        pushButtonAdd->setObjectName(QStringLiteral("pushButtonAdd"));

        horizontalLayout->addWidget(pushButtonAdd);

        pushButtonDelete = new QPushButton(PanelSelect);
        pushButtonDelete->setObjectName(QStringLiteral("pushButtonDelete"));

        horizontalLayout->addWidget(pushButtonDelete);

        pushButtonCopy = new QPushButton(PanelSelect);
        pushButtonCopy->setObjectName(QStringLiteral("pushButtonCopy"));

        horizontalLayout->addWidget(pushButtonCopy);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        horizontalLayout->setStretch(1, 1);
        horizontalLayout->setStretch(5, 2);

        retranslateUi(PanelSelect);

        QMetaObject::connectSlotsByName(PanelSelect);
    } // setupUi

    void retranslateUi(QWidget *PanelSelect)
    {
        PanelSelect->setWindowTitle(QApplication::translate("PanelSelect", "Form", Q_NULLPTR));
        label->setText(QApplication::translate("PanelSelect", "\320\237\320\260\320\275\320\265\320\273\321\214:", Q_NULLPTR));
        pushButtonAdd->setText(QApplication::translate("PanelSelect", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", Q_NULLPTR));
        pushButtonDelete->setText(QApplication::translate("PanelSelect", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214", Q_NULLPTR));
        pushButtonCopy->setText(QApplication::translate("PanelSelect", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PanelSelect: public Ui_PanelSelect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANELSELECT_H
