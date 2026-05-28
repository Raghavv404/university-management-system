#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_FACULTY_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_FACULTY_H
#pragma once

#include <vector>
#include "../utils/Country.h"
#include "../building/Building.h"
#include "../person/student/Student.h"   // Now links to real derived classes
#include "../person/lecturer/Lecturer.h" // Now links to real derived classes

struct FacultyData {
    char* name{nullptr};
    Country country{OTHER_COUNTRY};
    char* city{nullptr};
    int foundedYear{0};

    // Non-owning tracking vectors pointing straight to master data nodes
    std::vector<Building*> buildings; 
    std::vector<Student*>  students;   // Direct pointers to Student objects
    std::vector<Lecturer*> lecturers;  // Direct pointers to Lecturer objects
};

struct Faculty {
    FacultyData data;
    Faculty* next{nullptr};            // Intrusive linked list pointer
};

#endif