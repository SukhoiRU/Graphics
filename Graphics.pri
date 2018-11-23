HEADERS += ./Accumulation.h \
    ./stdafx.h \
    ./graphicsdoc.h \
    ./graphicsview.h \
    ./Dialogs/treeitem.h \
    ./Dialogs/graphselect.h \
    ./Dialogs/panelselect.h \
    ./Dialogs/qgridtree.h \
    ./Dialogs/pageSetup.h \
    ./Dialogs/treemodel.h
SOURCES += ./Accumulation.cpp \
    ./glad.c \
    ./graphicsdoc.cpp \
    ./graphicsview.cpp \
    ./main.cpp \
    ./stdafx.cpp \
    ./Dialogs/graphselect.cpp \
    ./Dialogs/pageSetup.cpp \
    ./Dialogs/panelselect.cpp \
    ./Dialogs/qgridtree.cpp \
    ./Dialogs/treeitem.cpp \
    ./Dialogs/treemodel.cpp
FORMS += ./Dialogs/graphselect.ui \
    ./Dialogs/panelselect.ui \
    ./graphicsdoc.ui \
    ./Dialogs/PageSetup.ui
RESOURCES += graphics.qrc
