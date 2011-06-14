
cmake_minimum_required(VERSION 2.8)

project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings -Wfloat-equal -pedantic -Wno-long-long -ggdb -std=c++0x")

find_package(Qt4 REQUIRED)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTWEBKIT TRUE)
include(${QT_USE_FILE})
include_directories(SYSTEM ${QT_INCLUDES})
include_directories(./paradiseo-1.3-beta2/paradiseo-mo/src /usr/include/eo)

add_executable(exhaustive-search exhaustive-search.cc
    cli/exhaustive-search.cxx parser/csv.cc parser/hip2.cc
    utils/common_calculation_functions.cc star.cc)
target_link_libraries(exhaustive-search
    -losg -losgViewer)

add_executable(parser_hip2_TEST parser/hip2.cc parser/hip2_TEST.cc)
target_link_libraries(parser_hip2_TEST -lgtest -lpthread)

add_executable(star_TEST star.cc star_TEST.cc)
target_link_libraries(star_TEST -losg -losgViewer)

QT4_WRAP_UI(simplegui_UIS_H
    gui/simple.ui
    gui/simulated_annealing_cooling_schedule.ui)
QT4_WRAP_CPP(viewer_widget_MOC_SRCS
    gui/viewer_widget.hh gui/main_window.hh
    gui/parser_thread.hh gui/computation_thread.hh)

QT4_WRAP_UI(target_manager_UIS_H
    gui/target_manager.ui)
QT4_WRAP_CPP(target_manager_MOC_SRCS
    gui/target_manager.hh
    gui/data_loader_csv.hh
    gui/data_loader_hipparcos.hh)
QT4_ADD_RESOURCES(target_manager_RCS icons/icons.qrc)

QT4_WRAP_CPP(qtsoap_MOC_SRCS
    3rdparty/qtsoap/qtsoap.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
#add_executable(simplegui exhaustive-search-gui.cc
#    gui/main_window.cc
#    ${simplegui_UIS_H} ${viewer_widget_MOC_SRCS}
#    cli/exhaustive-search.cxx parser/csv.cc parser/hip2.cc
#    utils/common_calculation_functions.cc star.cc)
#target_link_libraries(simplegui
#    ${QT_LIBRARIES}
#    -losg -losgViewer -losgQt)

add_executable(moving-target-tsp moving-target-tsp.cc
    gui/main_window.cc
    gui/computation_thread.cc
    gui/target_manager.cc
    gui/data_loader_csv.cc
    gui/data_loader_hipparcos.cc
    ${simplegui_UIS_H} ${viewer_widget_MOC_SRCS}
    ${target_manager_UIS_H} ${target_manager_MOC_SRCS} ${target_manager_RCS}
    cli/exhaustive-search.cxx
    parser/csv.cc
    parser/hip.cc
    parser/hip2.cc
    utils/common_calculation_functions.cc
    3rdparty/qtsoap/qtsoap.cpp
    ${qtsoap_MOC_SRCS}
    star.cc)
target_link_libraries(moving-target-tsp
    ${QT_LIBRARIES}
    -losg -losgViewer -losgQt
    -leoutils -leo -les -lga -lcma)

add_executable(target_manager_TEST
    gui/target_manager_TEST.cc
    gui/target_manager.cc
    gui/data_loader_csv.cc
    gui/data_loader_hipparcos.cc
    parser/csv.cc
    parser/hip.cc
    parser/hip2.cc
    utils/common_calculation_functions.cc
    3rdparty/qtsoap/qtsoap.cpp
    ${target_manager_UIS_H} ${target_manager_MOC_SRCS} ${target_manager_RCS}
    ${qtsoap_MOC_SRCS})
target_link_libraries(target_manager_TEST
    ${QT_LIBRARIES})
