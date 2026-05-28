#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_PERSON_MANAGER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_PERSON_MANAGER_H
#pragma once

#include <vector>
#include <functional>

#include "Person.h"
#include "student/Student.h"
#include "lecturer/Lecturer.h"
#include "../utils/IdGenerator.h"

class CourseManager;

class PersonManager {
private:
    Person* head{nullptr};           // Head of the polymorphic intrusive linked list [cite: 320]
    IdGenerator idGen;               // Generates unique human IDs [cite: 321]
    CourseManager* courseManager{nullptr}; // Held for cascading deletion [cite: 322]

    static void clearPersonMemory(Person* person);
    static Person* copyPerson(const Person* other);

public:
    explicit PersonManager(CourseManager* cm = nullptr);
    ~PersonManager();

    // Rule of Three [cite: 317-318]
    PersonManager(const PersonManager& other);
    PersonManager& operator=(const PersonManager& other);

    // Polymorphic Factory Methods
    Student* addStudent(const char* name, const char* surname, int age, Country nationality, StudyLevel level);
    Lecturer* addLecturer(const char* name, const char* surname, int age, Country nationality, AcademicTitle title, const char* specialization);

    // Updates base human components in place [cite: 337-338]
    static void updatePerson(Person* p, const char* name, const char* surname, int age, Country nationality);

    // Cascading Deletion [cite: 312-315]
    bool deletePerson(uint32_t personId);
    void deleteAllPeople();

    // Query Snapshots [cite: 339-340]
    std::vector<Person*> getAllPeopleCollection() const;
    static std::vector<Person*> filterPeople(const std::vector<Person*>& people, const std::function<bool(Person*)>& predicate);
    Person* findPersonById(uint32_t id) const;
    
    static std::vector<Person*> findPeopleByName(const char* name, const char* surname, const std::vector<Person*>& people);
    static std::vector<Person*> findPeopleByAge(int age, const std::vector<Person*>& people);
    static std::vector<Person*> findPeopleYoungerThan(int age, const std::vector<Person*>& people);
    static std::vector<Person*> findPeopleOlderThan(int age, const std::vector<Person*>& people);
    static std::vector<Person*> findPeopleByNationality(Country nationality, const std::vector<Person*>& people);

    // Display Methods (Now properly declared!)
    static void displayPerson(const Person* p);
    void displayPeopleList() const;

    void setCourseManager(CourseManager* mgr) { courseManager = mgr; }
};

#endif