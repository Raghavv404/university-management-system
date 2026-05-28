#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_SEMESTER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_SEMESTER_H
#pragma once

// A Course is offered in a specific Semester of a specific year.
// Two enum values are sufficient because at most one of each runs per year.
enum SemesterTerm {
    WINTER,   // typical Oct - Feb period
    SUMMER    // typical Feb - Jun period
};

struct Semester {
    SemesterTerm term;
    int year;            // four-digit year, e.g. 2025
};

#endif
