cmake_minimum_required(VERSION 3.14)

project(MetaRecordQt
  LANGUAGES CXX
  VERSION 1.0.0)

option(BUILD_TESTS "builds tests" ON)
option(WITH_TABLE_MODEL "with MetaRecordTableModel (needs QtQml and QI18n)" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)

message("Building ${PROJECT_NAME} using Qt${QT_VERSION_MAJOR}")

set(source_files
  metarecord-qt/metarecordable.cpp
  metarecord-qt/metarecordcomparable.cpp
  metarecord-qt/metarecordnotifiable.cpp
)
set(header_files
  metarecord-qt/MetaRecordQt_global.h
  metarecord-qt/metarecordable.h
  metarecord-qt/metarecordcomparable.h
  metarecord-qt/metarecordvalidatable.h
  metarecord-qt/metarecordnotifiable.h
  metarecord-qt/hasOne.h
  metarecord-qt/vector.h
  metarecord-qt/relationship.h
)

if (WITH_TABLE_MODEL)
  message("Building ${PROJECT_NAME} with MetaRecordTableModel")
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Qml REQUIRED)
  if (NOT DEFINED QI18N_LIBRARIES)
    pkg_check_modules(QI18N REQUIRED Qt${QT_VERSION_MAJOR}I18n>=1.0.0)
  endif()
  set(TABLE_MODEL_LIBS Qt${QT_VERSION_MAJOR}::Qml ${QI18N_LIBRARIES})
  set(source_files ${source_files} metarecord-qt/tablemodel.cpp)
  set(header_files ${header_files} metarecord-qt/tablemodel.h)
  string(REPLACE ";" " " TABLEMODEL_PC_CFLAGS "${QI18N_CFLAGS}")
  string(REPLACE ";" " " TABLEMODEL_PC_LDFLAGS "${QI18N_LDFLAGS}")
else()
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
endif()

add_library(MetaRecordQt${QT_VERSION_MAJOR} SHARED
  ${source_files}
  ${header_files}
)

target_link_libraries(MetaRecordQt${QT_VERSION_MAJOR} PRIVATE Qt${QT_VERSION_MAJOR}::Core ${TABLE_MODEL_LIBS})

target_compile_definitions(MetaRecordQt${QT_VERSION_MAJOR} PRIVATE METARECORDQT_LIBRARY)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

configure_file(meta-record-qt.pc.in meta-record-qt${QT_VERSION_MAJOR}.pc @ONLY)

install(FILES ${header_files} DESTINATION include/metarecord-qt)
install(TARGETS MetaRecordQt${QT_VERSION_MAJOR} LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/meta-record-qt${QT_VERSION_MAJOR}.pc DESTINATION lib/pkgconfig)
