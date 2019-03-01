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
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_GAxe_dialog
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *pushButton;

    void setupUi(QDialog *GAxe_dialog)
    {
        if (GAxe_dialog->objectName().isEmpty())
            GAxe_dialog->setObjectName(QStringLiteral("GAxe_dialog"));
        GAxe_dialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(GAxe_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        pushButton = new QPushButton(GAxe_dialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(70, 130, 75, 23));

        retranslateUi(GAxe_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GAxe_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GAxe_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GAxe_dialog);
    } // setupUi

    void retranslateUi(QDialog *GAxe_dialog)
    {
        GAxe_dialog->setWindowTitle(QApplication::translate("GAxe_dialog", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("GAxe_dialog", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GAxe_dialog: public Ui_GAxe_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAXE_DIALOG_H
