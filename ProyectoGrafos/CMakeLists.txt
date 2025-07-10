cmake_minimum_required(VERSION 3.31)
project(Proyecto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Proyecto main.cpp
        GrafosPesados/GrafoPesado.tpp
        GrafosPesados/GrafoPesado.h
        GrafosPesados/DiGrafoPesado.tpp
        GrafosPesados/DiGrafoPesado.h
        GrafosPesados/utils/FloydWarshall.tpp
        GrafosPesados/utils/FloydWarshall.h
        GrafosPesados/Marcado.h
        GrafosPesados/Marcado.cpp
        GrafoWidget.cpp
        GrafoWidget.h
        MainWindow.cpp
        MainWindow.h
        qresource.qrc)
target_link_libraries(Proyecto
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

