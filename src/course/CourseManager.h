#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_COURSE_MANAGER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_COURSE_MANAGER_H
#pragma once

#include <vector>
#include <functional>

#include "Course.h"
#include "../utils/IdGenerator.h"

// CourseManager owns every Course in the system, including the
// EnrollmentEntry linked lists nested inside their CourseData.
//
// Domain rules enforced here:
//   - A Course cannot be scheduled before 1980 or further than 10 years
//     into the future of `currentYear` (default 2026).
//   - ECTS must be in the inclusive range [1, 30].
//   - The same student cannot be enrolled in the same course twice.
//   - The number of enrolled students must not exceed the building's
//     maxCapacity, when a building is set.
//   - The lecturer must be employed by the course's faculty (warning, soft).
//
// CourseManager exposes "removeXFromCourseData" methods that the other
// managers call during cascading deletion to keep the references clean.
class CourseManager {
    Course* head;
    IdGenerator idGen;
    int currentYear{2026};   // anchor for the +10y / -inf time-range check

public:
    CourseManager();
    ~CourseManager();

    CourseManager(const CourseManager& other);
    CourseManager& operator=(const CourseManager& other);

    // Schedules a new Course. Returns nullptr (and prints a diagnostic) if any
    // domain rule fails. Strings are deep-copied.
    Course* course(const char* name, Semester semester, int ects,
                   Faculty* faculty, Lecturer* lecturer, Building* building);

    // Adds a student to a course's enrollment list. Returns false on rule
    // violation (duplicate, capacity, or ungraded inputs). Score = -1.0 = ungraded.
    static bool enrollStudent(Course* course, uint32_t studentId);
    // Sets the final_grade for a previously enrolled student.
    static bool gradeStudent(Course* course, uint32_t studentId, float grade);
    // Removes a student's EnrollmentEntry from the course (no grading).
    static bool unenrollStudent(Course* course, uint32_t studentId);

    // Updates fields in-place. Pass nullptr / -1 to leave a field unchanged.
    static void updateCourse(Course* c, const char* name, Semester* semester, int ects,
                             Faculty* faculty, Lecturer* lecturer, Building* building);

    std::vector<Course*> getAllCoursesCollection() const;

    static Course* findCourseById(uint32_t id, const std::vector<Course*>& courses);
    static std::vector<Course*> filterCourses(const std::vector<Course*>& courses,
                                              const std::function<bool(Course*)>& predicate);
    static std::vector<Course*> findCoursesBySemester(Semester semester, const std::vector<Course*>& courses);
    static std::vector<Course*> findCoursesByFaculty(Faculty* faculty, const std::vector<Course*>& courses);
    static std::vector<Course*> findCoursesByLecturer(Lecturer* lecturer, const std::vector<Course*>& courses);
    static std::vector<Course*> findCoursesByBuilding(Building* building, const std::vector<Course*>& courses);
    static std::vector<Course*> findCoursesByStudent(uint32_t studentId, const std::vector<Course*>& courses);

    // Frees an entire EnrollmentEntry linked list.
    static void clearEnrollmentMemory(EnrollmentEntry*& head);

    bool deleteCourse(Course* course);
    void deleteAllCourses();

    static void displayCourse(const Course* c);
    void displayCoursesList() const;

    // ---- cascading-deletion hooks called by the other managers --------------
    // Set every reference to `faculty` / `building` to nullptr in all CourseData.
    void removeFacultyFromCourseData (const Faculty*  faculty)  const;
    void removeBuildingFromCourseData(const Building* building) const;
    // Remove the person from every Course: clear lecturer pointer if it was
    // them, and erase any EnrollmentEntry with this id.
    void removePersonFromCourseData  (uint32_t personId)        const;

    // Test helper - lets unit tests pin a "current year" so domain validation
    // is reproducible regardless of the wall clock.
    void setCurrentYear(int year);
};

#endif
