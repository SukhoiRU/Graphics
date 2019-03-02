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

QT_BEGIN_NAMESPACE

class Ui_GAxe_dialog
{
public:
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *lineEdit;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_11;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout1;
    QGridLayout *gridLayout;
    QLabel *label_7;
    QLabel *label_4;
    QLineEdit *lineEdit_3;
    QLabel *label_6;
    QLabel *label_5;
    QPushButton *pushButton;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_4;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboBox;
    QComboBox *comboBox_2;
    QLabel *label_8;
    QLineEdit *lineEdit_5;
    QLabel *label_9;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;
    QDialogButtonBox *buttonBox;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton_3;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *GAxe_dialog)
    {
        if (GAxe_dialog->objectName().isEmpty())
            GAxe_dialog->setObjectName(QStringLiteral("GAxe_dialog"));
        GAxe_dialog->resize(614, 740);
        gridLayout_3 = new QGridLayout(GAxe_dialog);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
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

        lineEdit = new QLineEdit(GAxe_dialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_2 = new QLabel(GAxe_dialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(GAxe_dialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        label_11 = new QLabel(GAxe_dialog);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_2->addWidget(label_11, 1, 1, 1, 1);

        label_10 = new QLabel(GAxe_dialog);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_2->addWidget(label_10, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);


        verticalLayout_2->addLayout(verticalLayout);

        groupBox = new QGroupBox(GAxe_dialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout1 = new QVBoxLayout(groupBox);
        verticalLayout1->setObjectName(QStringLiteral("verticalLayout1"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 3, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        lineEdit_3 = new QLineEdit(groupBox);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));

        gridLayout->addWidget(lineEdit_3, 1, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 2, 0, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(pushButton, 3, 1, 1, 1);

        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        gridLayout->addWidget(lineEdit_2, 0, 1, 1, 1);

        lineEdit_4 = new QLineEdit(groupBox);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));

        gridLayout->addWidget(lineEdit_4, 2, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout_2->addWidget(comboBox);

        comboBox_2 = new QComboBox(groupBox);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        horizontalLayout_2->addWidget(comboBox_2);


        gridLayout->addLayout(horizontalLayout_2, 4, 1, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        lineEdit_5 = new QLineEdit(groupBox);
        lineEdit_5->setObjectName(QStringLiteral("lineEdit_5"));

        gridLayout->addWidget(lineEdit_5, 5, 1, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 5, 0, 1, 1);


        verticalLayout1->addLayout(gridLayout);


        verticalLayout_2->addWidget(groupBox);


        gridLayout_3->addLayout(verticalLayout_2, 0, 0, 3, 1);

        pushButton_2 = new QPushButton(GAxe_dialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        gridLayout_3->addWidget(pushButton_2, 3, 2, 1, 1);

        pushButton_4 = new QPushButton(GAxe_dialog);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        gridLayout_3->addWidget(pushButton_4, 2, 2, 1, 1);

        buttonBox = new QDialogButtonBox(GAxe_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_3->addWidget(buttonBox, 0, 2, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(205, 326, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_4, 0, 1, 1, 1);

        pushButton_3 = new QPushButton(GAxe_dialog);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        gridLayout_3->addWidget(pushButton_3, 1, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 3, 0, 1, 1);

        buttonBox->raise();
        pushButton_2->raise();
        pushButton_3->raise();
        pushButton_4->raise();
        groupBox->raise();

        retranslateUi(GAxe_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GAxe_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GAxe_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GAxe_dialog);
    } // setupUi

    void retranslateUi(QDialog *GAxe_dialog)
    {
        GAxe_dialog->setWindowTitle(QApplication::translate("GAxe_dialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\260 \320\276\321\201\320\265\320\271", Q_NULLPTR));
        label->setText(QApplication::translate("GAxe_dialog", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265", Q_NULLPTR));
        label_2->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\320\272\321\201\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_3->setText(QApplication::translate("GAxe_dialog", "\320\234\320\270\320\275\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_11->setText(QApplication::translate("GAxe_dialog", "\320\234\320\270\320\275\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        label_10->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\320\272\321\201\320\270\320\274\321\203\320\274:", Q_NULLPTR));
        groupBox->setTitle(QString());
        label_7->setText(QApplication::translate("GAxe_dialog", "\320\246\320\262\320\265\321\202", Q_NULLPTR));
        label_4->setText(QApplication::translate("GAxe_dialog", "\320\235\320\260\321\207\320\260\320\273\320\276", Q_NULLPTR));
        label_6->setText(QApplication::translate("GAxe_dialog", "\320\224\320\273\320\270\320\275\320\260", Q_NULLPTR));
        label_5->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261", Q_NULLPTR));
        pushButton->setText(QApplication::translate("GAxe_dialog", "PushButton", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("GAxe_dialog", "\320\232\320\262\320\260\320\264\321\200\320\260\321\202", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\232\321\200\321\203\320\263", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\242\321\200\320\265\321\203\320\263\320\276\320\273\321\214\320\275\320\270\320\272", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\232\321\200\320\265\321\201\321\202", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\227\320\262\320\265\320\267\320\264\320\276\321\207\320\272\320\260", Q_NULLPTR)
        );
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("GAxe_dialog", "\320\237\321\203\321\201\321\202\320\276\320\271", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\241\320\277\320\273\320\276\321\210\320\275\320\276\320\271", Q_NULLPTR)
         << QApplication::translate("GAxe_dialog", "\320\241 \320\272\320\276\320\275\321\202\321\203\321\200\320\276\320\274", Q_NULLPTR)
        );
        label_8->setText(QApplication::translate("GAxe_dialog", "\320\234\320\260\321\200\320\272\320\265\321\200", Q_NULLPTR));
        label_9->setText(QApplication::translate("GAxe_dialog", "\320\224\320\260\320\275\320\275\321\213\320\265 \342\204\226", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("GAxe_dialog", "\320\224\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\320\276 >>", Q_NULLPTR));
        pushButton_4->setText(QApplication::translate("GAxe_dialog", "\320\241\321\202\320\260\321\202\320\270\321\201\321\202\320\270\320\272\320\260", Q_NULLPTR));
        pushButton_3->setText(QApplication::translate("GAxe_dialog", "\320\227\320\260\320\274\320\265\320\275\320\260", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GAxe_dialog: public Ui_GAxe_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAXE_DIALOG_H
