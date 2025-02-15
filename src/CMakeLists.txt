set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_definitions(SPDLOG_FMT_EXTERNAL)
add_compile_definitions(SPDLOG_COMPILED_LIB)

find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

include_directories()

set(sources
    common/logger/Logger.cpp
    common/containers/vector.cpp
    common/events/events.cpp
    common/properties/ObservableProperty.cpp
    common/properties/ObservablePropertyFactory.cpp
    common/components/IComponent.cpp
    common/components/ComponentStateMachine.cpp
    common/threading/ThreadPool.cpp
    )

set(libs
    ${Eigen3_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${spdlog_LIBRARIES}
    ${fmt_LIBRARIES})

set(include_dirs
    ${Eigen3_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS})

if (UNIX)
set(sources
    ${sources}
    common/containers/Linux/SharedObject.cpp)
endif(UNIX)

if (WIN32)
set(sources
    ${sources}
    common/containers/Windows/SharedObject.cpp)
endif(WIN32)


add_library(urf_common ${sources})
target_link_libraries(urf_common ${libs})
target_include_directories(urf_common
    PUBLIC
        ${include_dirs}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/)

if (WIN32)
    include(GenerateExportHeader)
    set(EXPORT_HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/urf/common/urf_common_export.h)
    generate_export_header(urf_common EXPORT_FILE_NAME ${EXPORT_HEADER_PATH})
    install(FILES "${EXPORT_HEADER_PATH}" DESTINATION include/urf/common)
endif(WIN32)

install(TARGETS urf_common EXPORT urf_common DESTINATION lib/)
install(DIRECTORY urf/ DESTINATION include/urf FILES_MATCHING PATTERN "*.hpp")

