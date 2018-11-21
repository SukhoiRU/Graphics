/********************************************************************************
** Form generated from reading UI file 'graphicsdoc.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICSDOC_H
#define UI_GRAPHICSDOC_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>

QT_BEGIN_NAMESPACE

class Ui_GraphicsDoc
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *action_2;
    QAction *action_LoadOrion;
    QAction *actionAddAxe;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QToolBar *toolBarPanel;

    void setupUi(QMainWindow *GraphicsDoc)
    {
        if (GraphicsDoc->objectName().isEmpty())
            GraphicsDoc->setObjectName(QStringLiteral("GraphicsDoc"));
        GraphicsDoc->resize(742, 630);
        actionOpen = new QAction(GraphicsDoc);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionSave = new QAction(GraphicsDoc);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        action_2 = new QAction(GraphicsDoc);
        action_2->setObjectName(QStringLiteral("action_2"));
        action_LoadOrion = new QAction(GraphicsDoc);
        action_LoadOrion->setObjectName(QStringLiteral("action_LoadOrion"));
        actionAddAxe = new QAction(GraphicsDoc);
        actionAddAxe->setObjectName(QStringLiteral("actionAddAxe"));
        menuBar = new QMenuBar(GraphicsDoc);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 742, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menu_2);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        GraphicsDoc->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GraphicsDoc);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        GraphicsDoc->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(GraphicsDoc);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GraphicsDoc->setStatusBar(statusBar);
        toolBarPanel = new QToolBar(GraphicsDoc);
        toolBarPanel->setObjectName(QStringLiteral("toolBarPanel"));
        GraphicsDoc->addToolBar(Qt::TopToolBarArea, toolBarPanel);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menu->addAction(actionOpen);
        menu->addAction(actionSave);
        menu_2->addAction(menu_3->menuAction());
        menu_3->addAction(action_2);
        menu_3->addAction(action_LoadOrion);
        menu_4->addAction(actionAddAxe);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionOpen);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSave);

        retranslateUi(GraphicsDoc);

        QMetaObject::connectSlotsByName(GraphicsDoc);
    } // setupUi

    void retranslateUi(QMainWindow *GraphicsDoc)
    {
        GraphicsDoc->setWindowTitle(QApplication::translate("GraphicsDoc", "Graphics", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("GraphicsDoc", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", Q_NULLPTR));
        actionSave->setText(QApplication::translate("GraphicsDoc", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", Q_NULLPTR));
        action_2->setText(QApplication::translate("GraphicsDoc", "\320\241\320\220\320\237\320\240", Q_NULLPTR));
        action_LoadOrion->setText(QApplication::translate("GraphicsDoc", "\320\236\321\200\320\270\320\276\320\275", Q_NULLPTR));
        actionAddAxe->setText(QApplication::translate("GraphicsDoc", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionAddAxe->setToolTip(QApplication::translate("GraphicsDoc", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\263\321\200\320\260\321\204\320\270\320\272", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionAddAxe->setShortcut(QApplication::translate("GraphicsDoc", "Ins", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        menu->setTitle(QApplication::translate("GraphicsDoc", "\320\244\320\260\320\271\320\273", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("GraphicsDoc", "\320\224\320\260\320\275\320\275\321\213\320\265", Q_NULLPTR));
        menu_3->setTitle(QApplication::translate("GraphicsDoc", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", Q_NULLPTR));
        menu_4->setTitle(QApplication::translate("GraphicsDoc", "\320\223\321\200\320\260\321\204\320\270\320\272", Q_NULLPTR));
        toolBarPanel->setWindowTitle(QApplication::translate("GraphicsDoc", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GraphicsDoc: public Ui_GraphicsDoc {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICSDOC_H
