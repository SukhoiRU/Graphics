cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
project(Graphics VERSION 2.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets Xml Gui PrintSupport REQUIRED)

set(NAME_SRC
    Dialogs/colorbutton.cpp
    Dialogs/colorbutton.h
    Dialogs/gaxe_dialog.cpp
    Dialogs/gaxe_dialog.h
    Dialogs/gaxe_dialog.ui
    Dialogs/graphselect.cpp
    Dialogs/graphselect.h
    Dialogs/graphselect.ui
    Dialogs/graphSettings.cpp
    Dialogs/graphSettings.h
    Dialogs/graphSettings.ui
    Dialogs/locator_item.cpp
    Dialogs/locator_item.h
    Dialogs/locator_model.cpp
    Dialogs/locator_model.h
    Dialogs/locator_view.cpp
    Dialogs/locator_view.h
    Dialogs/pageSetup.cpp
    Dialogs/pageSetup.h
    Dialogs/PageSetup.ui
    Dialogs/panelselect.cpp
    Dialogs/panelselect.h
    Dialogs/panelselect.ui
    Dialogs/qgridtree.cpp
    Dialogs/qgridtree.h
    Dialogs/treeitem.cpp
    Dialogs/treeitem.h
    Dialogs/treemodel.cpp
    Dialogs/treemodel.h

    Graph/GAxe.cpp
    Graph/GAxe.h
    Graph/GAxeArg.cpp
    Graph/GAxeArg.h
    Graph/GraphObject.cpp
    Graph/GraphObject.h
    Graph/GTextLabel.cpp
    Graph/GTextLabel.h

    shaders/fbo.frag
    shaders/fbo.vert
    shaders/gaxe_cross.frag
    shaders/gaxe_cross.vert
    shaders/gaxe_data_bool.geom
    shaders/gaxe_data.frag
    shaders/gaxe_data.geom
    shaders/gaxe_data.vert
    shaders/gaxe_selection.frag
    shaders/gaxe_selection.vert
    shaders/gaxe.frag
    shaders/gaxe.vert
    shaders/gaxearg.frag
    shaders/gaxearg.vert
    shaders/markers.frag
    shaders/markers.vert
    shaders/simple.frag
    shaders/simple.vert
    shaders/textlabel.frag
    shaders/textlabel.vert

    Accumulation.cpp
    Accumulation.h
    glad.cpp
    graphics.qrc
    Graphics.rc
    graphicsdoc.cpp
    graphicsdoc.h
    graphicsdoc.ui
    graphicsview.cpp
    graphicsview.h
    main.cpp
    Orion_Accumulation.cpp
    Orion_Accumulation.h
    resource.h
    Sapr_Accumulation.cpp
    Sapr_Accumulation.h
    Sapr.h
    Sapr.inl
    stdafx.cpp
    stdafx.h
    TRF_Accumulation.cpp
    TRF_Accumulation.h
)


add_executable(${PROJECT_NAME} WIN32 ${NAME_SRC})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${NAME_SRC})

#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::PrintSupport)
if(linux)
	target_link_libraries(${PROJECT_NAME} PUBLIC dl)
else()
	target_link_libraries(${PROJECT_NAME} PUBLIC glu32 opengl32)
endif(linux)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

include(cmake/cotire.cmake)
cotire(${PROJECT_NAME})

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")  
	#precompiled header
#	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")    
#	set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")

	#Debugging    
#	set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND "\"$(SAPR_Dir)\\Exe\\ModelView_$(Configuration).exe\"")
#	set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"/path:$(ProjectDir)$(Configuration)\\$(ProjectName).dll\" \"/bufdir:${PROJECT_SOURCE_DIR}/../../project\" /utf8 /mdl:../../0.mdl")
#	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)       		       
endif()
