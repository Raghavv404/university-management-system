University Management System
C++ project for the EOOP Laboratory — topic: University.

The system models the administrative ecosystem of a university. The base entity is Person, from which Student (study level + GPA) and Lecturer (academic title + specialization) inherit using true C++ polymorphic inheritance. Building represents physical locations, Faculty is the central organizational entity that tracks student/lecturer enrollment and owns buildings, and Course represents an individual subject offering in a given semester, managing its own embedded enrollment list.

The architecture leverages a clean object-oriented layout: pure data representations live in src/.../X.h, and dedicated controllers live in src/.../XManager.{h,cpp}. PersonManager serves as the central polymorphic repository, managing an intrusive linked list of Person* nodes without object slicing via the Virtual Clone Pattern. Cross-manager references use raw pointers; deletions cascade through the system using virtual destructors to prevent dangling pointers and memory leaks.

Building
Bash
mkdir build && cd build
cmake ..
make university_demo
./university_demo            # runs the integration scenario
Tests
The test suite uses GoogleTest. If you have GoogleTest installed (apt install libgtest-dev then build it, or vcpkg/conan), CMake will automatically pick it up:

Bash
cmake -DUMS_BUILD_TESTS=ON ..
make university_tests
ctest --output-on-failure
If you don't have GoogleTest available, the project ships a header-only shim at test/minimal_gtest.h with the same API. To build the tests against the shim, the simplest approach is the clean one-liner used during development:

Bash
g++ -std=c++17 -Wall -Wextra -O0 -g -fsanitize=address \
    -Itest -Isrc \
    src/utils/IdGenerator.cpp \
    src/person/PersonManager.cpp \
    src/building/BuildingManager.cpp \
    src/faculty/FacultyManager.cpp \
    src/course/CourseManager.cpp \
    test/utils/IdGeneratorTest.cpp \
    test/person/PersonManagerTest.cpp \
    test/building/BuildingManagerTest.cpp \
    test/faculty/FacultyManagerTest.cpp \
    test/course/CourseManagerTest.cpp \
    test/test_main.cpp \
    -o run_tests
./run_tests
All core operational tests pass flawlessly under both regular configurations and AddressSanitizer builds.

Documentation
docs/EOOP_Preliminary_Project_University.docx — the deliverable document with all four required sections.

docs/structures_map.png — instance-level pointer diagram (case study).

docs/managers_map.png — manager-level cascading-deletion diagram.

docs/structures_map.dot, docs/managers_map.dot — Graphviz sources.

Domain rules enforced
Building / Faculty names are unique. Re-creating with the same name updates the existing record instead of inserting a duplicate.

A Course cannot be scheduled before 1980 or more than 10 years after the current year.

ECTS must be in [1, 30].

A Student cannot be enrolled in the same Course twice.

A Course's enrollment cannot exceed its Building's maxCapacity.

GPA must be -1.0 (ungraded) or in [2.0, 5.0] (Polish scale).