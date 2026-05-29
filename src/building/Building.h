#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_H
#pragma once

#include <cstring>  // FIX: Added to clear errors with strlen and strcpy
#include "../utils/Country.h"

// Forward declaration to avoid circular include with Faculty.
struct Faculty;

// BuildingData holds raw state attributes that describe a physical building.
struct BuildingData {
    char* name{nullptr};
    Country country{OTHER_COUNTRY};
    char* city{nullptr};
    int roomCount{0};
    int maxCapacity{0};
};

// Building has been modernized into a true Encapsulated C++ Class.
class Building {
private:
    BuildingData data{};
    Faculty* ownedBy{nullptr};

public:
    // Intrusive singly-linked-list pointer kept public for manager splicing
    Building* next{nullptr};

    // ---- Constructors / Destructor ----
    Building() : ownedBy(nullptr), next(nullptr) {}

    Building(const char* name, Country country, const char* city, int roomCount, int maxCapacity)
        : ownedBy(nullptr), next(nullptr) {
        
        if (name) {
            data.name = new char[std::strlen(name) + 1];
            std::strcpy(data.name, name);
        }
        if (city) {
            data.city = new char[std::strlen(city) + 1];
            std::strcpy(data.city, city);
        }
        data.country = country;
        data.roomCount = roomCount;
        data.maxCapacity = maxCapacity;
    }

    // Custom Destructor: Crucial to free raw heap character buffers!
    ~Building() {
        delete[] data.name;
        delete[] data.city;
    }

    // ---- Rule of Three (Preventing Shallow Copy Memory Leaks) ----
    Building(const Building& other) : ownedBy(other.ownedBy), next(nullptr) {
        if (other.data.name) {
            data.name = new char[std::strlen(other.data.name) + 1];
            std::strcpy(data.name, other.data.name);
        }
        if (other.data.city) {
            data.city = new char[std::strlen(other.data.city) + 1];
            std::strcpy(data.city, other.data.city);
        }
        data.country = other.data.country;
        data.roomCount = other.data.roomCount;
        data.maxCapacity = other.data.maxCapacity;
    }

    Building& operator=(const Building& other) {
        if (this == &other) return *this;

        // Clear existing strings
        delete[] data.name;
        delete[] data.city;
        data.name = nullptr;
        data.city = nullptr;

        ownedBy = other.ownedBy;
        next = nullptr;

        if (other.data.name) {
            data.name = new char[std::strlen(other.data.name) + 1];
            std::strcpy(data.name, other.data.name);
        }
        if (other.data.city) {
            data.city = new char[std::strlen(other.data.city) + 1];
            std::strcpy(data.city, other.data.city);
        }
        data.country = other.data.country;
        data.roomCount = other.data.roomCount;
        data.maxCapacity = other.data.maxCapacity;

        return *this;
    }

    // ---- Virtual Clone Engine ----
    virtual Building* clone() const {
        return new Building(*this);
    }

    // ---- Getters & Setters (Controlled Interfaces) ----
    const char* getName() const { return data.name; }
    Country getCountry() const { return data.country; }
    const char* getCity() const { return data.city; }
    int getRoomCount() const { return data.roomCount; }
    int getMaxCapacity() const { return data.maxCapacity; }

    Faculty* getFaculty() const { return ownedBy; }
    void setFaculty(Faculty* faculty) { ownedBy = faculty; }

    // Update operational parameters safely
    void updateData(const char* newName, const char* newCity, int rooms, int capacity) {
        if (newName) {
            delete[] data.name;
            data.name = new char[std::strlen(newName) + 1];
            std::strcpy(data.name, newName);
        }
        if (newCity) {
            delete[] data.city;
            data.city = new char[std::strlen(newCity) + 1];
            std::strcpy(data.city, newCity);
        }
        if (rooms >= 0) data.roomCount = rooms;
        if (capacity >= 0) data.maxCapacity = capacity;
    }
};

#endif