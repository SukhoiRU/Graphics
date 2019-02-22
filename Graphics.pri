HEADERS += \
    $$PWD/Dialogs/graphselect.h \
    $$PWD/Dialogs/pageSetup.h \
    $$PWD/Dialogs/panelselect.h \
    $$PWD/Dialogs/qgridtree.h \
    $$PWD/Dialogs/treeitem.h \
    $$PWD/Dialogs/treemodel.h \
    $$PWD/Dialogs/locator_item.h \
    $$PWD/Dialogs/locator_model.h \
    $$PWD/Dialogs/locator_view.h \
    $$PWD/GeneratedFiles/ui_graphicsdoc.h \
    $$PWD/GeneratedFiles/ui_graphselect.h \
    $$PWD/GeneratedFiles/ui_PageSetup.h \
    $$PWD/GeneratedFiles/ui_panelselect.h \
    $$PWD/Graph/GAxe.h \
    $$PWD/Graph/GAxeArg.h \
    $$PWD/Graph/GraphObject.h \
    $$PWD/Graph/GTextLabel.h \
    $$PWD/Accumulation.h \
    $$PWD/graphicsdoc.h \
    $$PWD/graphicsview.h \
    $$PWD/stdafx.h
SOURCES += \
    $$PWD/Dialogs/graphselect.cpp \
    $$PWD/Dialogs/pageSetup.cpp \
    $$PWD/Dialogs/panelselect.cpp \
    $$PWD/Dialogs/qgridtree.cpp \
    $$PWD/Dialogs/treeitem.cpp \
    $$PWD/Dialogs/treemodel.cpp \
    $$PWD/Dialogs/locator_item.cpp \
    $$PWD/Dialogs/locator_model.cpp \
    $$PWD/Dialogs/locator_view.cpp \
    $$PWD/Graph/GAxe.cpp \
    $$PWD/Graph/GAxeArg.cpp \
    $$PWD/Graph/GraphObject.cpp \
    $$PWD/Graph/GTextLabel.cpp \
    $$PWD/Accumulation.cpp \
    $$PWD/glad.cpp \
    $$PWD/graphicsdoc.cpp \
    $$PWD/graphicsview.cpp \
    $$PWD/main.cpp \
    $$PWD/stdafx.cpp
FORMS += $$PWD/Dialogs/graphselect.ui \
    $$PWD/Dialogs/panelselect.ui \
    $$PWD/graphicsdoc.ui \
    $$PWD/Dialogs/PageSetup.ui
RESOURCES += graphics.qrc
PRECOMPILED_HEADER = stdafx.h

DISTFILES += \
    $$PWD/shaders/gaxe_data.geom \
    $$PWD/shaders/gaxe_data_triangles.geom \
    $$PWD/shaders/gaxe.frag \
    $$PWD/shaders/gaxe_cross.frag \
    $$PWD/shaders/gaxe_data.frag \
    $$PWD/shaders/gaxearg.frag \
    $$PWD/shaders/simple.frag \
    $$PWD/shaders/textlabel.frag \
    $$PWD/shaders/gaxe.vert \
    $$PWD/shaders/gaxe_cross.vert \
    $$PWD/shaders/gaxe_data.vert \
    $$PWD/shaders/gaxearg.vert \
    $$PWD/shaders/simple.vert \
    $$PWD/shaders/textlabel.vert
