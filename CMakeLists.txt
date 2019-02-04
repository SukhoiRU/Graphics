cmake_minimum_required(VERSION 3.1)

project(NewGraphics LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_GENERATOR_PLATFORM x64)
#set(CMAKE_PREFIX_PATH /home/sukhoiru/Qt5.9.1/5.9.1/gcc_64)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
#set(CMAKE_BUILD_TYPE Release)

find_package(Qt5 REQUIRED
    Core
    Gui
    Widgets
)

set(NAME_SRC
    Accumulation.cpp
    glad.cpp
    graphics.qrc
    graphicsdoc.cpp
    graphicsdoc.ui
    graphicsview.cpp
    main.cpp
    stdafx.cpp

    Dialogs/graphselect.cpp
    Dialogs/panelselect.cpp
    Dialogs/pageSetup.cpp
    Dialogs/qgridtree.cpp
    Dialogs/treeitem.cpp
    Dialogs/treemodel.cpp
    Dialogs/graphselect.ui
    Dialogs/PageSetup.ui
    Dialogs/panelselect.ui

    Graph/GAxe.cpp
    Graph/GAxeArg.cpp
    Graph/GraphObject.cpp
    Graph/GTextLabel.cpp

    shaders/gaxe.frag
    shaders/gaxe.vert
    shaders/gaxearg.frag
    shaders/gaxearg.vert
    shaders/simple.frag
    shaders/simple.vert
    shaders/text.frag
    shaders/text.vert
    shaders/textlabel.frag
    shaders/textlabel.vert
)

set(NAME_HEADERS
    Accumulation.h
    graphicsdoc.h
    graphicsview.h
    stdafx.h

    Dialogs/graphselect.h
    Dialogs/pageSetup.h
    Dialogs/panelselect.h
    Dialogs/qgridtree.h
    Dialogs/treeitem.h
    Dialogs/treemodel.h

    Graph/GAxe.h
    Graph/GAxeArg.h
    Graph/GraphObject.h
    Graph/GTextLabel.h
)

add_executable(${PROJECT_NAME} ${NAME_SRC} ${NAME_HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:NO_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets)
