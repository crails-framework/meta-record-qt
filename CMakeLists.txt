cmake_minimum_required(VERSION 3.14)

project(MetaRecordQt LANGUAGES CXX)

option(BUILD_TESTS "builds tests" ON)
option(WITH_TABLE_MODEL "uses table model (will pull QtQml and QI18n)" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(USE_QT_5)
  find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
else()
  find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
endif()

if (WITH_TABLE_MODELS)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Qml REQUIRED)
  set(TABLE_MODEL_LIBS Qt${QT_VERSION_MAJOR}::Qml QI18n)
  set(TABLE_MODEL_SRC
    metarecord/tablemodel.h
    metarecord/tablemodel.cpp)
else()
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
endif()

add_library(MetaRecordQt SHARED
  metarecord/MetaRecordQt_global.h
  metarecord/metarecordable.cpp
  metarecord/metarecordable.h
  metarecord/metarecordcomparable.cpp
  metarecord/metarecordcomparable.h
  metarecord/metarecordnotifiable.cpp
  metarecord/metarecordnotifiable.h
  metarecord/hasOne.h
  ${TABLE_MODEL_SRC}
)

target_link_libraries(MetaRecordQt PRIVATE Qt${QT_VERSION_MAJOR}::Core ${TABLE_MODEL_LIBS})

target_compile_definitions(MetaRecordQt PRIVATE METARECORDQT_LIBRARY)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
