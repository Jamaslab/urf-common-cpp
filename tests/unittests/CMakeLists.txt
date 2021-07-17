set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(GTest REQUIRED)

set(UNIT_TEST_SRC
    containers/SharedObjectTests.cpp
    properties/ObservablePropertyTests.cpp
)

set(UT_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/src
    ${PROJECT_SOURCE_DIR}/src
    ${GTest_INCLUDE_DIRS}
)

set(UT_LIBRARIES
    urf_common
    ${GTest_LIBRARIES}
)

add_executable(common-unittest ${UNIT_TEST_SRC})
target_link_libraries(common-unittest ${UT_LIBRARIES})
target_include_directories(common-unittest SYSTEM PUBLIC ${UT_INCLUDE_DIRS})

add_test(NAME commonTests
         COMMAND common-unittest
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
