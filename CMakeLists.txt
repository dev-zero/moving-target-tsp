
cmake_minimum_required(VERSION 2.8)

project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings -Wfloat-equal -pedantic -Wno-long-long -ggdb -std=c++11 -ftemplate-depth=1200")

find_package(Qt4 REQUIRED)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTWEBKIT TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})

find_package(Boost 1.46.0)
find_program(CLI cli)

# mark all 3rdparty headers as system headers to disable warnings generated
# for them by the compiler
include_directories(SYSTEM ${QT_INCLUDES})

if(Boost_FOUND AND CLI)
    add_definitions(-DHAVE_BOOST)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

    add_custom_command(OUTPUT exhaustive-search.cxx
        COMMAND ${CLI} --generate-cxx ${CMAKE_CURRENT_SOURCE_DIR}/cli/exhaustive-search.cli
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cli/patch-source.sh" exhaustive-search
        DEPENDS ${CLI} cli/exhaustive-search.cli
        )

    add_executable(exhaustive-search exhaustive-search.cc
        exhaustive-search.cxx parser/csv.cc parser/hip2.cc
        utils/common_calculation_functions.cc gui/star.cc)
    target_link_libraries(exhaustive-search
        -losg -losgViewer -losgDB)
endif()

add_executable(parser_hip2_TEST parser/hip2.cc parser/hip2_TEST.cc)
target_link_libraries(parser_hip2_TEST -lgtest -lpthread)

add_executable(star_TEST gui/star.cc gui/star_TEST.cc)
target_link_libraries(star_TEST -losg -losgViewer -losgDB)

QT4_WRAP_UI(maingui_UIS_H
    gui/main.ui
    gui/simulated_annealing_cooling_schedule.ui)
QT4_WRAP_CPP(viewer_widget_MOC_SRCS
    gui/viewer_widget.hh gui/main_window.hh)

QT4_WRAP_UI(target_manager_UIS_H
    gui/target_manager.ui)
QT4_WRAP_CPP(target_manager_MOC_SRCS
    gui/target_manager.hh
    base/data_loader_csv.hh
    base/data_loader_hipparcos.hh
    base/data_loader_simbad.hh
    gui/target_data_filter_proxy_model.hh)
QT4_ADD_RESOURCES(target_manager_RCS icons/icons.qrc)

QT4_WRAP_CPP(computation_manager_MOC_SRCS
    base/computation_manager.hh
    base/exhaustive_search_recursive.hh
    base/simulated_annealing.hh)

QT4_WRAP_CPP(simulated_annealing_MOC_SRCS
    base/simulated_annealing.hh)

QT4_WRAP_CPP(qtsoap_MOC_SRCS
    3rdparty/qtsoap/qtsoap.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

if(CLI)
    add_custom_command(OUTPUT moving-target-tsp.cxx
        COMMAND ${CLI} --generate-cxx "${CMAKE_CURRENT_SOURCE_DIR}/cli/moving-target-tsp.cli"
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cli/patch-source.sh" moving-target-tsp
        DEPENDS ${CLI} cli/moving-target-tsp.cli
        )

    add_executable(moving-target-tsp-cli moving-target-tsp-cli.cc
        utils/common_calculation_functions.cc
        base/simulated_annealing.cc
        base/moving_target_tsp.cc
        moving-target-tsp.cxx
        ${simulated_annealing_MOC_SRCS}
        )
    target_link_libraries(moving-target-tsp-cli ${QT_LIBRARIES})
endif()

add_executable(moving-target-tsp moving-target-tsp.cc
    gui/main_window.cc
    gui/viewer_widget.cc
    gui/target_manager.cc
    base/data_loader_csv.cc
    base/data_loader_hipparcos.cc
    base/data_loader_simbad.cc
    gui/target_data_filter_proxy_model.cc
    base/computation_manager.cc
    base/exhaustive_search_recursive.cc
    base/simulated_annealing.cc
    base/moving_target_tsp.cc
    ${maingui_UIS_H} ${viewer_widget_MOC_SRCS}
    ${target_manager_UIS_H} ${target_manager_MOC_SRCS} ${target_manager_RCS}
    ${computation_manager_MOC_SRCS}
    parser/csv.cc
    parser/hip.cc
    parser/hip2.cc
    utils/common_calculation_functions.cc
    3rdparty/qtsoap/qtsoap.cpp
    ${qtsoap_MOC_SRCS}
    gui/star.cc)
target_link_libraries(moving-target-tsp
    ${QT_LIBRARIES}
    -losg -losgViewer -losgQt -losgGA -losgDB)

add_executable(target_manager_TEST
    gui/target_manager_TEST.cc
    gui/target_manager.cc
    base/data_loader_csv.cc
    base/data_loader_hipparcos.cc
    base/data_loader_simbad.cc
    gui/target_data_filter_proxy_model.cc
    parser/csv.cc
    parser/hip.cc
    parser/hip2.cc
    utils/common_calculation_functions.cc
    3rdparty/qtsoap/qtsoap.cpp
    ${target_manager_UIS_H} ${target_manager_MOC_SRCS} ${target_manager_RCS}
    ${qtsoap_MOC_SRCS})
target_link_libraries(target_manager_TEST
    ${QT_LIBRARIES})

add_executable(computation_manager_TEST
    base/computation_manager_TEST.cc
    base/computation_manager.cc
    base/exhaustive_search_recursive.cc
    base/simulated_annealing.cc
    base/moving_target_tsp.cc
    utils/common_calculation_functions.cc
    ${computation_manager_MOC_SRCS})

target_link_libraries(computation_manager_TEST
    ${QT_LIBRARIES})

