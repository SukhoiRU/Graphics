/********************************************************************************
** Form generated from reading UI file 'PageSetup.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGESETUP_H
#define UI_PAGESETUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout;
    QLineEdit *borderTop;
    QLineEdit *borderRight;
    QLineEdit *borderLeft;
    QLineEdit *borderBottom;
    QHBoxLayout *hs1;
    QSpacerItem *hs3;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_10;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout_11;
    QLineEdit *gridWidth;
    QLabel *label_4;
    QLabel *label_2;
    QLineEdit *gridHeight;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_2;
    QLineEdit *graphTop;
    QLineEdit *graphRight;
    QLineEdit *graphLeft;
    QLineEdit *graphBottom;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_7;
    QSpacerItem *horizontalSpacer_4;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QLineEdit *pageHeight;
    QLineEdit *pageZoom;
    QLabel *label_7;
    QLineEdit *pageWidth;
    QLabel *label_3;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QSlider *zoomSlider;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(297, 523);
        layoutWidget = new QWidget(Dialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 2, 2));
        verticalLayout_4 = new QVBoxLayout(layoutWidget);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4 = new QGridLayout(Dialog);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        borderTop = new QLineEdit(groupBox);
        borderTop->setObjectName(QStringLiteral("borderTop"));

        gridLayout->addWidget(borderTop, 0, 1, 1, 1);

        borderRight = new QLineEdit(groupBox);
        borderRight->setObjectName(QStringLiteral("borderRight"));

        gridLayout->addWidget(borderRight, 1, 2, 1, 1);

        borderLeft = new QLineEdit(groupBox);
        borderLeft->setObjectName(QStringLiteral("borderLeft"));

        gridLayout->addWidget(borderLeft, 1, 0, 1, 1);

        borderBottom = new QLineEdit(groupBox);
        borderBottom->setObjectName(QStringLiteral("borderBottom"));

        gridLayout->addWidget(borderBottom, 2, 1, 1, 1);


        gridLayout_5->addLayout(gridLayout, 2, 0, 1, 1);


        gridLayout_4->addWidget(groupBox, 6, 0, 1, 1);

        hs1 = new QHBoxLayout();
        hs1->setSpacing(6);
        hs1->setObjectName(QStringLiteral("hs1"));
        hs1->setContentsMargins(0, 0, 0, 0);
        hs3 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hs1->addItem(hs3);

        okButton = new QPushButton(Dialog);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setAutoDefault(true);

        hs1->addWidget(okButton);

        cancelButton = new QPushButton(Dialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setAutoDefault(false);

        hs1->addWidget(cancelButton);

        applyButton = new QPushButton(Dialog);
        applyButton->setObjectName(QStringLiteral("applyButton"));
        applyButton->setAutoDefault(false);

        hs1->addWidget(applyButton);


        gridLayout_4->addLayout(hs1, 10, 0, 1, 1);

        groupBox_4 = new QGroupBox(Dialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_10 = new QGridLayout(groupBox_4);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_10->addItem(horizontalSpacer_2, 0, 0, 1, 1);

        gridLayout_11 = new QGridLayout();
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        gridWidth = new QLineEdit(groupBox_4);
        gridWidth->setObjectName(QStringLiteral("gridWidth"));

        gridLayout_11->addWidget(gridWidth, 0, 1, 1, 1);

        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_11->addWidget(label_4, 1, 0, 1, 1);

        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_11->addWidget(label_2, 0, 0, 1, 1);

        gridHeight = new QLineEdit(groupBox_4);
        gridHeight->setObjectName(QStringLiteral("gridHeight"));

        gridLayout_11->addWidget(gridHeight, 1, 1, 1, 1);


        gridLayout_10->addLayout(gridLayout_11, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_10->addItem(horizontalSpacer_3, 0, 2, 1, 1);


        gridLayout_4->addWidget(groupBox_4, 8, 0, 1, 1);

        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_6 = new QGridLayout(groupBox_2);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        graphTop = new QLineEdit(groupBox_2);
        graphTop->setObjectName(QStringLiteral("graphTop"));

        gridLayout_2->addWidget(graphTop, 0, 1, 1, 1);

        graphRight = new QLineEdit(groupBox_2);
        graphRight->setObjectName(QStringLiteral("graphRight"));

        gridLayout_2->addWidget(graphRight, 1, 2, 1, 1);

        graphLeft = new QLineEdit(groupBox_2);
        graphLeft->setObjectName(QStringLiteral("graphLeft"));

        gridLayout_2->addWidget(graphLeft, 1, 0, 1, 1);

        graphBottom = new QLineEdit(groupBox_2);
        graphBottom->setObjectName(QStringLiteral("graphBottom"));

        gridLayout_2->addWidget(graphBottom, 2, 1, 1, 1);


        gridLayout_6->addLayout(gridLayout_2, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 7, 0, 1, 1);

        groupBox_3 = new QGroupBox(Dialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_7 = new QGridLayout(groupBox_3);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_4, 0, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label = new QLabel(groupBox_3);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        pageHeight = new QLineEdit(groupBox_3);
        pageHeight->setObjectName(QStringLiteral("pageHeight"));

        gridLayout_3->addWidget(pageHeight, 1, 1, 1, 1);

        pageZoom = new QLineEdit(groupBox_3);
        pageZoom->setObjectName(QStringLiteral("pageZoom"));

        gridLayout_3->addWidget(pageZoom, 2, 1, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_3->addWidget(label_7, 2, 0, 1, 1);

        pageWidth = new QLineEdit(groupBox_3);
        pageWidth->setObjectName(QStringLiteral("pageWidth"));

        gridLayout_3->addWidget(pageWidth, 0, 1, 1, 1);

        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_3->addWidget(label_3, 1, 0, 1, 1);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_3->addWidget(label_8, 2, 2, 1, 1);

        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_3->addWidget(label_9, 1, 2, 1, 1);

        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_3->addWidget(label_10, 0, 2, 1, 1);


        gridLayout_7->addLayout(gridLayout_3, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer, 0, 2, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer, 9, 0, 1, 1);

        zoomSlider = new QSlider(Dialog);
        zoomSlider->setObjectName(QStringLiteral("zoomSlider"));
        zoomSlider->setMinimum(100);
        zoomSlider->setMaximum(1000);
        zoomSlider->setOrientation(Qt::Horizontal);

        gridLayout_4->addWidget(zoomSlider, 4, 0, 1, 1);

        QWidget::setTabOrder(pageWidth, borderLeft);
        QWidget::setTabOrder(borderLeft, borderTop);
        QWidget::setTabOrder(borderTop, borderBottom);
        QWidget::setTabOrder(borderBottom, borderRight);
        QWidget::setTabOrder(borderRight, graphLeft);
        QWidget::setTabOrder(graphLeft, graphTop);
        QWidget::setTabOrder(graphTop, graphBottom);
        QWidget::setTabOrder(graphBottom, graphRight);
        QWidget::setTabOrder(graphRight, gridWidth);
        QWidget::setTabOrder(gridWidth, gridHeight);
        QWidget::setTabOrder(gridHeight, okButton);
        QWidget::setTabOrder(okButton, cancelButton);

        retranslateUi(Dialog);
        QObject::connect(okButton, SIGNAL(clicked()), Dialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), Dialog, SLOT(reject()));

        applyButton->setDefault(true);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \321\201\321\202\321\200\320\260\320\275\320\270\321\206\321\213", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Dialog", "\320\236\321\202\321\201\321\202\321\203\320\277\321\213 \321\200\320\260\320\274\320\272\320\270 \320\276\321\202 \320\273\320\270\321\201\321\202\320\260", Q_NULLPTR));
        okButton->setText(QApplication::translate("Dialog", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("Dialog", "Cancel", Q_NULLPTR));
        applyButton->setText(QApplication::translate("Dialog", "\320\237\321\200\320\270\320\274\320\265\320\275\320\270\321\202\321\214", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("Dialog", "\320\240\320\260\320\267\320\274\320\265\321\200\321\213 \320\272\320\273\320\265\321\202\320\272\320\270", Q_NULLPTR));
        label_4->setText(QApplication::translate("Dialog", "\320\222\321\213\321\201\320\276\321\202\320\260", Q_NULLPTR));
        label_2->setText(QApplication::translate("Dialog", "\320\250\320\270\321\200\320\270\320\275\320\260", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("Dialog", "\320\236\321\202\321\201\321\202\321\203\320\277\321\213 \320\277\320\276\320\273\321\217 \320\263\321\200\320\260\321\204\320\270\320\272\320\276\320\262 \320\276\321\202 \321\200\320\260\320\274\320\272\320\270", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("Dialog", "\320\241\321\202\321\200\320\260\320\275\320\270\321\206\320\260", Q_NULLPTR));
        label->setText(QApplication::translate("Dialog", "\320\250\320\270\321\200\320\270\320\275\320\260", Q_NULLPTR));
        label_7->setText(QApplication::translate("Dialog", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261", Q_NULLPTR));
        label_3->setText(QApplication::translate("Dialog", "\320\222\321\213\321\201\320\276\321\202\320\260", Q_NULLPTR));
        label_8->setText(QApplication::translate("Dialog", "\320\277\320\270\320\272\321\201\320\265\320\273\321\214/\320\274\320\274", Q_NULLPTR));
        label_9->setText(QApplication::translate("Dialog", "\320\274\320\274", Q_NULLPTR));
        label_10->setText(QApplication::translate("Dialog", "\320\274\320\274", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGESETUP_H
