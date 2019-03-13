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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "Dialogs/locator_view.h"
#include "graphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_GraphicsDoc
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *action_2;
    QAction *action_LoadOrion;
    QAction *actionAddAxe;
    QAction *actionPageInfo;
    QSplitter *splitter;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QScrollBar *verticalScrollBar;
    QScrollBar *horizontalScrollBar;
    GraphicsView *oglView;
    LocatorView *locator;
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
        GraphicsDoc->resize(449, 649);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/images/line.png"), QSize(), QIcon::Normal, QIcon::Off);
        GraphicsDoc->setWindowIcon(icon);
        actionOpen = new QAction(GraphicsDoc);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionSave = new QAction(GraphicsDoc);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Resources/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon2);
        action_2 = new QAction(GraphicsDoc);
        action_2->setObjectName(QStringLiteral("action_2"));
        action_LoadOrion = new QAction(GraphicsDoc);
        action_LoadOrion->setObjectName(QStringLiteral("action_LoadOrion"));
        actionAddAxe = new QAction(GraphicsDoc);
        actionAddAxe->setObjectName(QStringLiteral("actionAddAxe"));
        actionPageInfo = new QAction(GraphicsDoc);
        actionPageInfo->setObjectName(QStringLiteral("actionPageInfo"));
        splitter = new QSplitter(GraphicsDoc);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        centralwidget = new QWidget(splitter);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalScrollBar = new QScrollBar(centralwidget);
        verticalScrollBar->setObjectName(QStringLiteral("verticalScrollBar"));
        verticalScrollBar->setMaximum(297);
        verticalScrollBar->setSingleStep(5);
        verticalScrollBar->setPageStep(50);
        verticalScrollBar->setOrientation(Qt::Vertical);

        gridLayout->addWidget(verticalScrollBar, 0, 2, 1, 1);

        horizontalScrollBar = new QScrollBar(centralwidget);
        horizontalScrollBar->setObjectName(QStringLiteral("horizontalScrollBar"));
        horizontalScrollBar->setMaximum(210);
        horizontalScrollBar->setSingleStep(5);
        horizontalScrollBar->setPageStep(50);
        horizontalScrollBar->setOrientation(Qt::Horizontal);
        horizontalScrollBar->setInvertedControls(false);

        gridLayout->addWidget(horizontalScrollBar, 1, 0, 1, 1);

        oglView = new GraphicsView(centralwidget);
        oglView->setObjectName(QStringLiteral("oglView"));
        oglView->setMouseTracking(true);
        oglView->setFocusPolicy(Qt::ClickFocus);

        gridLayout->addWidget(oglView, 0, 0, 1, 1);

        splitter->addWidget(centralwidget);
        locator = new LocatorView(splitter);
        locator->setObjectName(QStringLiteral("locator"));
        locator->setEditTriggers(QAbstractItemView::NoEditTriggers);
        locator->setProperty("showDropIndicator", QVariant(false));
        locator->setSelectionMode(QAbstractItemView::NoSelection);
        locator->setRootIsDecorated(false);
        splitter->addWidget(locator);
        GraphicsDoc->setCentralWidget(splitter);
        menuBar = new QMenuBar(GraphicsDoc);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 449, 21));
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
        menu->addAction(actionPageInfo);
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
        actionPageInfo->setText(QApplication::translate("GraphicsDoc", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \321\201\321\202\321\200\320\260\320\275\320\270\321\206\321\213", Q_NULLPTR));
        menu->setTitle(QApplication::translate("GraphicsDoc", "\320\244\320\260\320\271\320\273", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("GraphicsDoc", "\320\224\320\260\320\275\320\275\321\213\320\265", Q_NULLPTR));
        menu_3->setTitle(QApplication::translate("GraphicsDoc", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", Q_NULLPTR));
        menu_4->setTitle(QApplication::translate("GraphicsDoc", "&\320\223\321\200\320\260\321\204\320\270\320\272", Q_NULLPTR));
        toolBarPanel->setWindowTitle(QApplication::translate("GraphicsDoc", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GraphicsDoc: public Ui_GraphicsDoc {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICSDOC_H
