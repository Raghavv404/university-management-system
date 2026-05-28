#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_ACADEMIC_TITLE_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_ACADEMIC_TITLE_H
#pragma once

// Academic rank of a lecturer.
enum AcademicTitle {
    ASSISTANT,           // junior teaching staff, no PhD yet
    DOCTOR,              // PhD holder
    DOCTOR_HABILITATED,  // post-doctoral degree (Polish system)
    PROFESSOR
};

#endif
