#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_ID_GENERATOR_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_ID_GENERATOR_H
#pragma once

#include <cstdint>

// Generates monotonically increasing unique identifiers.
// Each manager that needs IDs (Person, Course) owns its own instance,
// so IDs are unique per category, not globally.
class IdGenerator {
    uint32_t nextId = 1;   // Start from 1; 0 is reserved as "no id".
public:
    uint32_t generateId();
};

#endif
