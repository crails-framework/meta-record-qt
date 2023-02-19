set(QT5_MIN_VERSION 5.12)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Core REQUIRED)

enable_testing(true)
include_directories("..")

set(TEST_LINK_LIBRARIES
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Test)

add_executable(metarecordable       tst_metarecordable.cpp ../metarecord-qt/metarecordable.cpp)
add_test(NAME metarecordable COMMAND metarecordable)
target_link_libraries(metarecordable PRIVATE ${TEST_LINK_LIBRARIES})

add_executable(metarecordcomparable tst_metarecordcomparable.cpp ../metarecord-qt/metarecordable.cpp ../metarecord-qt/metarecordcomparable.cpp)
add_test(NAME metarecordcomparable COMMAND metarecordcomparable)
target_link_libraries(metarecordcomparable PRIVATE ${TEST_LINK_LIBRARIES})
