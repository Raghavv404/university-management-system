#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_FACULTY_MANAGER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_FACULTY_MANAGER_H
#pragma once

#include <vector>
#include <functional>

#include "Faculty.h"
#include "../person/student/Student.h"
#include "../person/lecturer/Lecturer.h"
#include "../building/Building.h"

class CourseManager;

class FacultyManager {
private:
    Faculty* head{nullptr};
    CourseManager* courseManager{nullptr};

    void clearFacultyMemory(Faculty* f) const;
    static Faculty* copyFaculty(const Faculty* other);

public:
    explicit FacultyManager(CourseManager* courseManager = nullptr);
    ~FacultyManager();

    // Rule of Three for deep-copying Faculty nodes
    FacultyManager(const FacultyManager& other);
    FacultyManager& operator=(const FacultyManager& other);

    // Overload sets for creating/updating Faculty records
    void faculty(const char* name, Country country, const char* city, int foundedYear);
    void faculty(const char* name, Country country, const char* city);
    void faculty(const char* name, Country country, int foundedYear);
    void faculty(const char* name, Country country);

    std::vector<Faculty*> getAllFacultiesCollection() const;
    Faculty* findFacultyByName(const char* name) const;

    static std::vector<Faculty*> filterFaculties(const std::vector<Faculty*>& faculties, const std::function<bool(Faculty*)>& predicate);
    static std::vector<Faculty*> findFacultiesByCountry(Country country, const std::vector<Faculty*>& faculties);
    static std::vector<Faculty*> findFacultiesByCity(const char* city, const std::vector<Faculty*>& faculties);
    static std::vector<Faculty*> findFacultiesByFoundedYear(int year, const std::vector<Faculty*>& faculties);
    static std::vector<Faculty*> findFacultiesNewerThan(int year, const std::vector<Faculty*>& faculties);
    static std::vector<Faculty*> findFacultiesOlderThan(int year, const std::vector<Faculty*>& faculties);

    // Cascading Deletion
    bool deleteFaculty(Faculty* faculty);
    void deleteAllFaculties();

    static void displayFaculty(const Faculty* f);
    void displayFacultyList() const;

    // Polymorphic Relationship Wiring (Now accepts direct Student* and Lecturer*)
    static void addBuildingToFaculty(Building* b, Faculty* f);
    static void addStudentToFaculty (Student* s, Faculty* f);
    static void addLecturerToFaculty(Lecturer* l, Faculty* f);

    static bool removeBuildingFromFaculty(Building* b, Faculty* f);
    static bool removePersonFromFaculty (Person* p, Faculty* f);

    static int getFacultyBuildingsCount(const Faculty* f);
    static int getFacultyStudentsCount (const Faculty* f);
    static int getFacultyLecturersCount(const Faculty* f);

    void setCourseManager(CourseManager* mgr);
};

#endif