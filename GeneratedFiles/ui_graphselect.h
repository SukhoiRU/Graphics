/********************************************************************************
** Form generated from reading UI file 'graphselect.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHSELECT_H
#define UI_GRAPHSELECT_H

#include <Dialogs/qgridtree.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_GraphSelect
{
public:
    QHBoxLayout *horizontalLayout;
    QGridTree *treeView;

    void setupUi(QDialog *GraphSelect)
    {
        if (GraphSelect->objectName().isEmpty())
            GraphSelect->setObjectName(QStringLiteral("GraphSelect"));
        GraphSelect->resize(535, 597);
        GraphSelect->setModal(true);
        horizontalLayout = new QHBoxLayout(GraphSelect);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        treeView = new QGridTree(GraphSelect);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setProperty("uniformRowHeights", QVariant(true));
        treeView->setProperty("headerHidden", QVariant(false));

        horizontalLayout->addWidget(treeView);


        retranslateUi(GraphSelect);

        QMetaObject::connectSlotsByName(GraphSelect);
    } // setupUi

    void retranslateUi(QDialog *GraphSelect)
    {
        GraphSelect->setWindowTitle(QApplication::translate("GraphSelect", "\320\222\321\213\320\261\320\276\321\200 \320\263\321\200\320\260\321\204\320\270\320\272\320\260", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GraphSelect: public Ui_GraphSelect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHSELECT_H
