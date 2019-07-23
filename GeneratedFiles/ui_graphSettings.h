/********************************************************************************
** Form generated from reading UI file 'graphSettings.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHSETTINGS_H
#define UI_GRAPHSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_graphSettings
{
public:
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget_2;
    QWidget *tab_3;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QLabel *label_4;
    QLabel *label_3;
    QLineEdit *lineEdit_sel_alias;
    QSpacerItem *verticalSpacer_3;
    QLineEdit *lineEdit_sel_width;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QLineEdit *lineEdit_width;
    QLabel *label;
    QSpacerItem *verticalSpacer_2;
    QLineEdit *lineEdit_alias;
    QLabel *label_2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_6;
    QLineEdit *lineEdit_interp_width;
    QLabel *label_7;
    QSpacerItem *verticalSpacer_5;
    QLineEdit *lineEdit_interp_alias;
    QLabel *label_8;
    QWidget *tab_5;
    QWidget *tab_4;
    QWidget *tab_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *graphSettings)
    {
        if (graphSettings->objectName().isEmpty())
            graphSettings->setObjectName(QStringLiteral("graphSettings"));
        graphSettings->resize(530, 422);
        horizontalLayout = new QHBoxLayout(graphSettings);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(graphSettings);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tabWidget_2 = new QTabWidget(tab);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_2 = new QGridLayout(tab_3);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox_2 = new QGroupBox(tab_3);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_4->addWidget(label_4, 2, 0, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_4->addWidget(label_3, 0, 0, 1, 1);

        lineEdit_sel_alias = new QLineEdit(groupBox_2);
        lineEdit_sel_alias->setObjectName(QStringLiteral("lineEdit_sel_alias"));

        gridLayout_4->addWidget(lineEdit_sel_alias, 3, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_3, 4, 0, 1, 1);

        lineEdit_sel_width = new QLineEdit(groupBox_2);
        lineEdit_sel_width->setObjectName(QStringLiteral("lineEdit_sel_width"));

        gridLayout_4->addWidget(lineEdit_sel_width, 1, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 2, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 3, 3, 1, 1);

        groupBox = new QGroupBox(tab_3);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        lineEdit_width = new QLineEdit(groupBox);
        lineEdit_width->setObjectName(QStringLiteral("lineEdit_width"));

        gridLayout_3->addWidget(lineEdit_width, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 4, 0, 1, 1);

        lineEdit_alias = new QLineEdit(groupBox);
        lineEdit_alias->setObjectName(QStringLiteral("lineEdit_alias"));

        gridLayout_3->addWidget(lineEdit_alias, 3, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 2, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(tab_3);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_6 = new QGridLayout(groupBox_3);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        lineEdit_interp_width = new QLineEdit(groupBox_3);
        lineEdit_interp_width->setObjectName(QStringLiteral("lineEdit_interp_width"));

        gridLayout_6->addWidget(lineEdit_interp_width, 1, 0, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_6->addWidget(label_7, 0, 0, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer_5, 4, 0, 1, 1);

        lineEdit_interp_alias = new QLineEdit(groupBox_3);
        lineEdit_interp_alias->setObjectName(QStringLiteral("lineEdit_interp_alias"));

        gridLayout_6->addWidget(lineEdit_interp_alias, 3, 0, 1, 1);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_6->addWidget(label_8, 2, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 2, 2, 1, 1);

        tabWidget_2->addTab(tab_3, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        tabWidget_2->addTab(tab_5, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        tabWidget_2->addTab(tab_4, QString());

        gridLayout->addWidget(tabWidget_2, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());

        horizontalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(graphSettings);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);

        QWidget::setTabOrder(tabWidget, tabWidget_2);
        QWidget::setTabOrder(tabWidget_2, lineEdit_width);
        QWidget::setTabOrder(lineEdit_width, lineEdit_sel_width);
        QWidget::setTabOrder(lineEdit_sel_width, lineEdit_interp_width);
        QWidget::setTabOrder(lineEdit_interp_width, lineEdit_alias);
        QWidget::setTabOrder(lineEdit_alias, lineEdit_sel_alias);
        QWidget::setTabOrder(lineEdit_sel_alias, lineEdit_interp_alias);

        retranslateUi(graphSettings);

        tabWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(graphSettings);
    } // setupUi

    void retranslateUi(QWidget *graphSettings)
    {
        graphSettings->setWindowTitle(QApplication::translate("graphSettings", "graphSettings", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("graphSettings", "\320\222\321\213\320\264\320\265\320\273\320\265\320\275\320\275\320\260\321\217", Q_NULLPTR));
        label_4->setText(QApplication::translate("graphSettings", "\320\241\320\263\320\273\320\260\320\266\320\270\320\262\320\260\320\275\320\270\320\265", Q_NULLPTR));
        label_3->setText(QApplication::translate("graphSettings", "\320\242\320\276\320\273\321\211\320\270\320\275\320\260", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("graphSettings", "\320\236\320\261\321\213\321\207\320\275\320\260\321\217", Q_NULLPTR));
        label->setText(QApplication::translate("graphSettings", "\320\242\320\276\320\273\321\211\320\270\320\275\320\260", Q_NULLPTR));
        label_2->setText(QApplication::translate("graphSettings", "\320\241\320\263\320\273\320\260\320\266\320\270\320\262\320\260\320\275\320\270\320\265", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("graphSettings", "\320\241\321\202\321\203\320\277\320\265\320\275\321\214\320\272\320\270", Q_NULLPTR));
        label_7->setText(QApplication::translate("graphSettings", "\320\242\320\276\320\273\321\211\320\270\320\275\320\260", Q_NULLPTR));
        label_8->setText(QApplication::translate("graphSettings", "\320\241\320\263\320\273\320\260\320\266\320\270\320\262\320\260\320\275\320\270\320\265", Q_NULLPTR));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QApplication::translate("graphSettings", "\320\233\320\270\320\275\320\270\320\270", Q_NULLPTR));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("graphSettings", "\320\241\321\202\321\200\320\260\320\275\320\270\321\206\320\260", Q_NULLPTR));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("graphSettings", "Tab 2", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("graphSettings", "\320\223\321\200\320\260\321\204\320\270\320\272\320\270", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("graphSettings", "Tab 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class graphSettings: public Ui_graphSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHSETTINGS_H
