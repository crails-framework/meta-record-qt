cmake_minimum_required(VERSION 3.14)

project(MetaRecordQt LANGUAGES CXX)

option(BUILD_TESTS "builds tests" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_library(MetaRecordQt SHARED
  metarecord/MetaRecordQt_global.h
  metarecord/metarecordable.cpp
  metarecord/metarecordable.h
  metarecord/metarecordcomparable.cpp
  metarecord/metarecordcomparable.h
  metarecord/hasOne.h
)

target_link_libraries(MetaRecordQt PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(MetaRecordQt PRIVATE METARECORDQT_LIBRARY)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
