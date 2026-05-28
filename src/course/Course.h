#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_COURSE_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_COURSE_H
#pragma once

#include <cstdint>

#include "../utils/Semester.h"
#include "../building/Building.h"
#include "../faculty/Faculty.h"
#include "../person/student/Student.h"
#include "../person/lecturer/Lecturer.h"

// EnrollmentEntry is one student's participation in a course.
// We keep our own intrusive singly-linked list (mirroring MatchSquadEntry)
// instead of std::list to demonstrate manual pointer bookkeeping.
//   student_id   - id of the enrolled student (mirrors Student->Person->id).
//   final_grade  - grade on the 2.0 - 5.0 Polish scale; -1.0f means "not graded yet".
//   next         - next entry in the list.
struct EnrollmentEntry {
    uint32_t student_id{0};
    float final_grade{-1.0f};
    EnrollmentEntry* next{nullptr};
};

// CourseData describes one offering of one course.
//   name        - heap-owned, e.g. "Algorithms and Data Structures".
//   semester    - WINTER/SUMMER + year.
//   ects        - ECTS credit points awarded on completion (1..30 valid range).
//   faculty     - non-owning pointer to the Faculty that runs the course.
//   lecturer    - non-owning pointer to the Lecturer assigned to teach it.
//   building    - non-owning pointer to the Building where lectures happen.
//                 May be nullptr (online/unassigned).
//   enrollments - head pointer of the singly-linked list of EnrollmentEntries.
//                 CourseManager owns the entries (allocates and frees them).
struct CourseData {
    char* name{nullptr};
    Semester semester{};
    int ects{0};
    Faculty* faculty{nullptr};
    Lecturer* lecturer{nullptr};
    Building* building{nullptr};
    EnrollmentEntry* enrollments{nullptr};
};

// Course is a node in CourseManager's linked list. `id` is unique per CourseManager.
struct Course {
    uint32_t id{0};
    CourseData data{};
    Course* next{nullptr};
};

#endif
