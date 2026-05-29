# 🏛️ University Management System
> **Advanced C++ Memory Architecture Project**
> *Developed for the Effective Object-Oriented Programming (EOOP) Laboratory — Warsaw University of Technology*

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support/17)
[![Tests Status](https://img.shields.io/badge/Tests-42%20%2F%2042%20Passed-brightgreen.svg)](#-testing-framework)
[![Memory Safety](https://img.shields.io/badge/Memory-Asan%20Clean-success.svg)](#-architectural-highlights)

This production-grade system models the dynamic administrative ecosystem of a university. It is architected entirely using low-level memory mechanics, bypassing high-level container wrappers in favor of high-performance custom memory configurations.

---

## 🗺️ System Blueprint Maps

The project's architectural integrity is fully mapped out across three technical visualizations found in the `/docs` directory:

1. **Domain Entity Map (`docs/DOMAIN ENTITY.png`):** Shows class structures, two-way back-pointer connections, and true polymorphic hierarchies.
2. **Manager Dependency Map (`docs/Manager dependency.png`):** Details decoupled method boundaries and downstream cascade routing structures.
3. **Memory Ownership Sandbox Map (`docs/Screenshot...AM.png`):** Illustrates the clear separation between Stack-allocated Controllers and dynamic Heap-allocated Intrusive Chains.

---

## 💎 Architectural Highlights

*🚀 **Zero-Bloat Performance Engineering:** This system intentionally avoids heavy standard library container collections like `std::list` to maximize memory performance and tracking control.*

* **Intrusive Singly-Linked Lists:** Node sequence tracking wires (`next` pointers) are embedded directly inside the entity layouts. Managers hold legal authority over the head references, optimizing memory usage and traversal speeds.
* **True Polymorphic Subsystems:** `Student` and `Lecturer` inherit vertically from `Person`. The application uses a single, unified list structure of base pointers (`Person*`) and tracks exact runtime profiles dynamically via vptr/VTABLE dispatch hooks.
* **The Virtual Clone Pattern:** Replicates dynamic heap chains across copy constructors and assignment checks using deep-copy overrides, eliminating C++ object slicing risks.
* **Automated Cascading Deletions:** Custom multi-step cascade hooks drop broken pointer links across separate manager domains instantly during an isolation process. This isolates items cleanly before passing them to the virtual destructor stack, preventing dangling references and segmentation faults.

---

## 📁 Repository Directory Layout

```text
OG_UNIVERSITY_PROJECT/
├── build/                 # Build artifacts and executable binaries
├── docs/                  # System blueprints, diagrams, and project reports
├── src/                   # Main Source Subsystems
│   ├── building/          # Encapsulated Building structures and managers
│   ├── course/            # Course ledger and enrollment link frameworks
│   ├── faculty/           # Department allocation hubs and tracking arrays
│   ├── person/            # Polymorphic Person, Student, and Lecturer entities
│   └── utils/             # Identifiers, Country Enums, and Date structures
└── test/                  # Automated Validation Suites

# 1. Initialize and enter the build sandbox
mkdir -p build && cd build

# 2. Generate the local build configuration tree
cmake ..

# 3. Compile all executable targets cleanly
make

cd build
make
./university_tests

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