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
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>
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
    QListView *listView;
    QTableWidget *tableWidget;
    QTreeWidget *treeWidget;
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
        GraphicsDoc->resize(757, 637);
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
        listView = new QListView(splitter);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setModelColumn(0);
        splitter->addWidget(listView);
        tableWidget = new QTableWidget(splitter);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidget->rowCount() < 5)
            tableWidget->setRowCount(5);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(4, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget->setItem(0, 0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget->setItem(0, 1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget->setItem(1, 0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget->setItem(1, 1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidget->setItem(2, 0, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidget->setItem(2, 1, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidget->setItem(3, 0, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidget->setItem(3, 1, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidget->setItem(4, 0, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidget->setItem(4, 1, __qtablewidgetitem16);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setShowGrid(true);
        tableWidget->setGridStyle(Qt::SolidLine);
        tableWidget->setCornerButtonEnabled(false);
        splitter->addWidget(tableWidget);
        tableWidget->horizontalHeader()->setVisible(true);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(true);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(false);
        treeWidget = new QTreeWidget(splitter);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setRootIsDecorated(false);
        treeWidget->setUniformRowHeights(true);
        splitter->addWidget(treeWidget);
        treeWidget->header()->setProperty("showSortIndicator", QVariant(false));
        GraphicsDoc->setCentralWidget(splitter);
        menuBar = new QMenuBar(GraphicsDoc);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 757, 21));
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
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("GraphicsDoc", "\320\222\321\200\320\265\320\274\321\217", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("GraphicsDoc", "\320\227\320\275\320\260\321\207\320\265\320\275\320\270\320\265", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("GraphicsDoc", "1", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("GraphicsDoc", "2", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem4->setText(QApplication::translate("GraphicsDoc", "2", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem5->setText(QApplication::translate("GraphicsDoc", "2", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->verticalHeaderItem(4);
        ___qtablewidgetitem6->setText(QApplication::translate("GraphicsDoc", "3", Q_NULLPTR));

        const bool __sortingEnabled = tableWidget->isSortingEnabled();
        tableWidget->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget->item(0, 0);
        ___qtablewidgetitem7->setText(QApplication::translate("GraphicsDoc", "Ve", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget->item(0, 1);
        ___qtablewidgetitem8->setText(QApplication::translate("GraphicsDoc", "150.78", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidget->item(1, 0);
        ___qtablewidgetitem9->setText(QApplication::translate("GraphicsDoc", "Fi", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidget->item(1, 1);
        ___qtablewidgetitem10->setText(QApplication::translate("GraphicsDoc", "57.6", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidget->item(2, 0);
        ___qtablewidgetitem11->setText(QApplication::translate("GraphicsDoc", "L", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidget->item(2, 1);
        ___qtablewidgetitem12->setText(QApplication::translate("GraphicsDoc", "37.445618", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidget->item(3, 0);
        ___qtablewidgetitem13->setText(QApplication::translate("GraphicsDoc", "Vn", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidget->item(3, 1);
        ___qtablewidgetitem14->setText(QApplication::translate("GraphicsDoc", "17.6", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidget->item(4, 0);
        ___qtablewidgetitem15->setText(QApplication::translate("GraphicsDoc", "Vh", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidget->item(4, 1);
        ___qtablewidgetitem16->setText(QApplication::translate("GraphicsDoc", "-1.5", Q_NULLPTR));
        tableWidget->setSortingEnabled(__sortingEnabled);

        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("GraphicsDoc", "\320\227\320\275\320\260\321\207\320\265\320\275\320\270\320\265", Q_NULLPTR));
        ___qtreewidgetitem->setText(0, QApplication::translate("GraphicsDoc", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200", Q_NULLPTR));

        const bool __sortingEnabled1 = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(1, QApplication::translate("GraphicsDoc", "150.3", Q_NULLPTR));
        ___qtreewidgetitem1->setText(0, QApplication::translate("GraphicsDoc", "Ve", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget->topLevelItem(1);
        ___qtreewidgetitem2->setText(1, QApplication::translate("GraphicsDoc", "15.7", Q_NULLPTR));
        ___qtreewidgetitem2->setText(0, QApplication::translate("GraphicsDoc", "Vn", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem3 = treeWidget->topLevelItem(2);
        ___qtreewidgetitem3->setText(1, QApplication::translate("GraphicsDoc", "1.5", Q_NULLPTR));
        ___qtreewidgetitem3->setText(0, QApplication::translate("GraphicsDoc", "Vh", Q_NULLPTR));
        treeWidget->setSortingEnabled(__sortingEnabled1);

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
