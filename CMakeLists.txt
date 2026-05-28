cmake_minimum_required(VERSION 3.16)
project(university_management_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- core sources (one library so both main and tests can link) -----------
set(CORE_SOURCES
    src/utils/IdGenerator.cpp
    src/person/PersonManager.cpp
    src/building/BuildingManager.cpp
    src/faculty/FacultyManager.cpp
    src/course/CourseManager.cpp
)

add_library(university_core STATIC ${CORE_SOURCES})
target_include_directories(university_core PUBLIC src)

# ---- demo executable ------------------------------------------------------
add_executable(university_demo src/main.cpp)
target_link_libraries(university_demo PRIVATE university_core)

# ---- tests (GoogleTest, optional - enabled when GTest is installed) -------
option(UMS_BUILD_TESTS "Build GoogleTest unit tests" ON)

if(UMS_BUILD_TESTS)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        enable_testing()
        add_executable(university_tests
            test/utils/IdGeneratorTest.cpp
            test/person/PersonManagerTest.cpp
            test/building/BuildingManagerTest.cpp
            test/faculty/FacultyManagerTest.cpp
            test/course/CourseManagerTest.cpp
        )
        target_link_libraries(university_tests PRIVATE university_core GTest::gtest GTest::gtest_main)
        include(GoogleTest)
        gtest_discover_tests(university_tests)
    else()
        message(STATUS "GTest not found - tests target skipped")
    endif()
endif()