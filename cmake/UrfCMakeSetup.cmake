include_guard(GLOBAL)

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

set(CFLAGS ${CFLAGS} -DIS_BIG_ENDIAN=${IS_BIG_ENDIAN})
set(CXXFLAGS ${CXXFLAGS} -DIS_BIG_ENDIAN=${IS_BIG_ENDIAN})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

    if (MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
        endif()
    endif()
endif(WIN32)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")

    find_program(LCOV_BIN lcov)
    if((LCOV_BIN MATCHES "lcov$") AND (CMAKE_BUILD_TYPE MATCHES Debug))
        include(cmake/CodeCoverage.cmake)
        APPEND_COVERAGE_COMPILER_FLAGS()
        set(COVERAGE_LCOV_EXCLUDES
            '*build/*'
            '*tests/*'
            '*.conan/*'
            '*spdlog/*'
            '/usr/*'
            '/opt/*'
            '*googletest/*'
            '*googlemock/*'
        )
        message(STATUS "Excluded from test coverage report: ${COVERAGE_LCOV_EXCLUDES}")
        SETUP_TARGET_FOR_COVERAGE_LCOV(NAME test_coverage
                                    EXECUTABLE ctest)
    endif()
endif()

