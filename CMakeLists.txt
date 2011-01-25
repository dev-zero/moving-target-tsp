
cmake_minimum_required(VERSION 2.8)

project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings -Wfloat-equal -pedantic -Wno-long-long -ggdb -std=c++0x")

#find_package(Qt4 REQUIRED)
#set(QT_USE_QTGUI 1)
#include(${QT_USE_FILE})

#include_directories(${QT_INCLUDES})

add_executable(exhaustive-search exhaustive-search.cc
    cli/exhaustive-search.cxx parser/csv.cc common_calculation_functions.cc star.cc)
target_link_libraries(exhaustive-search ${QT_LIBRARIES} -losg -losgViewer)

add_executable(parser_hip2_TEST parser/hip2.cc parser/hip2_TEST.cc)
target_link_libraries(parser_hip2_TEST -lgtest -lpthread)

add_executable(star_TEST star.cc star_TEST.cc)
target_link_libraries(star_TEST -losg -losgViewer)


