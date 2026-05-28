#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_H
#pragma once

#include "../utils/Country.h"

// Forward declaration to avoid circular include with Faculty.
struct Faculty;

// BuildingData holds attributes that describe a physical building.
//   name           - heap-owned, must be unique system-wide (BuildingManager invariant).
//   country, city  - location information.
//   roomCount      - total number of teaching rooms in the building.
//   maxCapacity    - maximum simultaneous occupancy (used to validate course assignment).
struct BuildingData {
    char* name{nullptr};
    Country country{OTHER_COUNTRY};
    char* city{nullptr};
    int roomCount{0};
    int maxCapacity{0};
};

// Building is a node in BuildingManager's linked list.
//   ownedBy - non-owning back-pointer to the Faculty (or nullptr).
//             FacultyManager keeps a parallel forward pointer in its `buildings` list.
//   next    - intrusive singly-linked-list pointer.
struct Building {
    BuildingData data{};
    Faculty* ownedBy{nullptr};
    Building* next{nullptr};
};

#endif
