#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_STUDENT_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_STUDENT_H
#pragma once

#include "../Person.h"
#include "StudyLevel.h"

class Student : public Person {
public:
    StudyLevel level{BACHELOR};
    float gpa{-1.0f};

    Student() = default;
    Student(uint32_t id, const char* name, const char* surname, int age, Country nationality, StudyLevel level)
        : Person(id, name, surname, age, nationality), level(level), gpa(-1.0f) {}

    Person* clone() const override {
        return new Student(*this); // Safe polymorphic deep copy 
    }

    void display() const override {
        std::cout << "[Student] ID: " << id
                  << " | " << (data.name ? data.name : "?")
                  << " "   << (data.surname ? data.surname : "?")
                  << " | Age: " << data.age
                  << " | GPA: " << gpa << '\n';
    }
};

#endif