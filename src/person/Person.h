#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_PERSON_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_PERSON_H
#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include "../utils/Country.h"

struct Faculty;

struct PersonData {
    char* name{nullptr};
    char* surname{nullptr};
    int age{0};
    Country nationality{OTHER_COUNTRY};
};

class Person {
public:
    uint32_t id{0};
    PersonData data{};
    Faculty* employedBy{nullptr}; // Non-owning back-pointer [cite: 111]
    Person* next{nullptr};        // Intrusive linked list pointer [cite: 114]

    Person() = default;
    Person(uint32_t id, const char* name, const char* surname, int age, Country nationality)
        : id(id), employedBy(nullptr), next(nullptr) {
        data.age = age;
        data.nationality = nationality;
        if (name) {
            data.name = new char[std::strlen(name) + 1];
            std::strcpy(data.name, name);
        }
        if (surname) {
            data.surname = new char[std::strlen(surname) + 1];
            std::strcpy(data.surname, surname);
        }
    }

    virtual ~Person() {
        delete[] data.name;
        delete[] data.surname;
    }

    // Copy constructor used by cloning mechanisms to avoid object slicing
    Person(const Person& other) : id(other.id), employedBy(nullptr), next(nullptr) {
        data.age = other.data.age;
        data.nationality = other.data.nationality;
        if (other.data.name) {
            data.name = new char[std::strlen(other.data.name) + 1];
            std::strcpy(data.name, other.data.name);
        }
        if (other.data.surname) {
            data.surname = new char[std::strlen(other.data.surname) + 1];
            std::strcpy(data.surname, other.data.surname);
        }
    }

    Person& operator=(const Person& other) = delete;

    uint32_t getId() const { return id; }
    Faculty* getFaculty() const { return employedBy; }
    void setFaculty(Faculty* f) { employedBy = f; }

    virtual Person* clone() const = 0; // Fixed the missing clone error!
    virtual void display() const {
        std::cout << "ID: " << id
                  << " | " << (data.name ? data.name : "?")
                  << " "   << (data.surname ? data.surname : "?")
                  << " | Age: " << data.age
                  << " | Nationality: " << data.nationality << '\n';
    }
};

#endif