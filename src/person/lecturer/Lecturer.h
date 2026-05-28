#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_LECTURER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_LECTURER_H
#pragma once

#include "../Person.h"
#include "AcademicTitle.h"
#include <cstring>

class Lecturer : public Person {
public:
    AcademicTitle title{ASSISTANT};
    char* specialization{nullptr};

    Lecturer() = default;
    Lecturer(uint32_t id, const char* name, const char* surname, int age, Country nationality, AcademicTitle title, const char* specialization)
        : Person(id, name, surname, age, nationality), title(title) {
        if (specialization) {
            this->specialization = new char[std::strlen(specialization) + 1];
            std::strcpy(this->specialization, specialization);
        }
    }

    ~Lecturer() override {
        delete[] specialization;
    }

    // Custom copy constructor handling the specialized text string safely
    Lecturer(const Lecturer& other) : Person(other), title(other.title) {
        if (other.specialization) {
            specialization = new char[std::strlen(other.specialization) + 1];
            std::strcpy(specialization, other.specialization);
        }
    }

    Person* clone() const override {
        return new Lecturer(*this); // Safe polymorphic deep copy 
    }

    void display() const override {
        std::cout << "[Lecturer] ID: " << id
                  << " | " << (data.name ? data.name : "?")
                  << " "   << (data.surname ? data.surname : "?")
                  << " | Title: " << title
                  << " | Specialization: " << (specialization ? specialization : "?") << '\n';
    }
};

#endif