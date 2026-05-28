#include "FacultyManager.h" //
#include <iostream> //
#include <cstring> //
#include "../course/CourseManager.h" //

using namespace std; //

// ---- helpers ---------------------------------------------------------------

void FacultyManager::clearFacultyMemory(Faculty* f) const { //
    if (!f) return; //
    delete[] f->data.name; //
    delete[] f->data.city; //
    delete f; //
} //

Faculty* FacultyManager::copyFaculty(const Faculty* other) { //
    if (!other) return nullptr; //
    Faculty* f = new Faculty(); //
    f->data.foundedYear = other->data.foundedYear; //
    f->data.country = other->data.country; //
    
    if (other->data.name) { //
        f->data.name = new char[strlen(other->data.name) + 1]; //
        strcpy(f->data.name, other->data.name); //
    } //
    if (other->data.city) { //
        f->data.city = new char[strlen(other->data.city) + 1]; //
        strcpy(f->data.city, other->data.city); //
    } //
    
    // Vectors hold non-owning references, so copy them as-is 
    f->data.buildings = other->data.buildings; //
    f->data.students = other->data.students; //
    f->data.lecturers = other->data.lecturers; //
    return f; //
} //

// ---- ctors / dtor / copy ---------------------------------------------------

FacultyManager::FacultyManager(CourseManager* cm) : head(nullptr), courseManager(cm) {} //

FacultyManager::~FacultyManager() { //
    deleteAllFaculties(); //
} //

FacultyManager::FacultyManager(const FacultyManager& other) //
    : head(nullptr), courseManager(other.courseManager) { //
    Faculty* tail = nullptr; //
    for (Faculty* cur = other.head; cur; cur = cur->next) { //
        Faculty* clone = copyFaculty(cur); //
        if (!head) head = clone; //
        else       tail->next = clone; //
        tail = clone; //
    } //
} //

FacultyManager& FacultyManager::operator=(const FacultyManager& other) { //
    if (this == &other) return *this; //
    deleteAllFaculties(); //
    courseManager = other.courseManager; //
    Faculty* tail = nullptr; //
    for (Faculty* cur = other.head; cur; cur = cur->next) { //
        Faculty* clone = copyFaculty(cur); //
        if (!head) head = clone; //
        else       tail->next = clone; //
        tail = clone; //
    } //
    return *this; //
} //

// ---- CRUD ------------------------------------------------------------------

void FacultyManager::faculty(const char* name, Country country, const char* city, int foundedYear) { //
    Faculty* existing = findFacultyByName(name); //
    if (existing) { //
        if (city) { //
            delete[] existing->data.city; //
            existing->data.city = new char[strlen(city) + 1]; //
            strcpy(existing->data.city, city); //
        } //
        existing->data.country = country; //
        if (foundedYear >= 0) existing->data.foundedYear = foundedYear; //
        return; //
    } //

    Faculty* f = new Faculty(); //
    f->data.country = country; //
    f->data.foundedYear = foundedYear; //
    if (name) { //
        f->data.name = new char[strlen(name) + 1]; //
        strcpy(f->data.name, name); //
    } //
    if (city) { //
        f->data.city = new char[strlen(city) + 1]; //
        strcpy(f->data.city, city); //
    } //
    f->next = head; //
    head = f; //
} //

void FacultyManager::faculty(const char* name, Country country, const char* city) { faculty(name, country, city, -1); } //
void FacultyManager::faculty(const char* name, Country country, int foundedYear) { faculty(name, country, nullptr, foundedYear); } //
void FacultyManager::faculty(const char* name, Country country) { faculty(name, country, nullptr, -1); } //

vector<Faculty*> FacultyManager::getAllFacultiesCollection() const { //
    vector<Faculty*> out; //
    for (Faculty* cur = head; cur; cur = cur->next) out.push_back(cur); //
    return out; //
} //

Faculty* FacultyManager::findFacultyByName(const char* name) const { //
    if (!name) return nullptr; //
    for (Faculty* cur = head; cur; cur = cur->next) { //
        if (cur->data.name && strcmp(cur->data.name, name) == 0) return cur; //
    } //
    return nullptr; //
} //

// ---- Filtering Helpers -----------------------------------------------------

vector<Faculty*> FacultyManager::filterFaculties(const vector<Faculty*>& faculties, const function<bool(Faculty*)>& predicate) { //
    vector<Faculty*> out; //
    for (Faculty* f : faculties) if (f && predicate(f)) out.push_back(f); //
    return out; //
} //

vector<Faculty*> FacultyManager::findFacultiesByCountry(Country country, const vector<Faculty*>& faculties) { //
    return filterFaculties(faculties, [country](Faculty* f) { return f->data.country == country; }); //
} //

vector<Faculty*> FacultyManager::findFacultiesByCity(const char* city, const vector<Faculty*>& faculties) { //
    return filterFaculties(faculties, [city](Faculty* f) { return city && f->data.city && strcmp(f->data.city, city) == 0; }); //
} //

vector<Faculty*> FacultyManager::findFacultiesByFoundedYear(int year, const vector<Faculty*>& faculties) { //
    return filterFaculties(faculties, [year](Faculty* f) { return f->data.foundedYear == year; }); //
} //

vector<Faculty*> FacultyManager::findFacultiesNewerThan(int year, const vector<Faculty*>& faculties) { //
    return filterFaculties(faculties, [year](Faculty* f) { return f->data.foundedYear > year; }); //
} //

vector<Faculty*> FacultyManager::findFacultiesOlderThan(int year, const vector<Faculty*>& faculties) { //
    return filterFaculties(faculties, [year](Faculty* f) { return f->data.foundedYear < year; }); //
} //

// ---- Cascading Deletion ----------------------------------------------------

bool FacultyManager::deleteFaculty(Faculty* faculty) { //
    if (!faculty) return false; //

    // 1) Clear every course run by this faculty 
    if (courseManager) { //
        courseManager->removeFacultyFromCourseData(faculty); //
    } //

    // 2) Detach all owned buildings 
    for (Building* b : faculty->data.buildings) { //
        if (b) b->ownedBy = nullptr; //
    } //

    // 3) Detach employed persons 
    for (Student* s : faculty->data.students) { //
        if (s) s->employedBy = nullptr; //
    } //
    for (Lecturer* l : faculty->data.lecturers) { //
        if (l) l->employedBy = nullptr; //
    } //

    // 4) Unlink node from the list
    Faculty* prev = nullptr; //
    for (Faculty* cur = head; cur; prev = cur, cur = cur->next) { //
        if (cur == faculty) { //
            if (prev) prev->next = cur->next; //
            else      head       = cur->next; //
            clearFacultyMemory(cur); //
            return true; //
        } //
    } //
    return false; //
} //

void FacultyManager::deleteAllFaculties() { //
    while (head) { //
        Faculty* doomed = head; //
        head = head->next; //
        clearFacultyMemory(doomed); //
    } //
} //

// ---- Polymorphic Relationship Integration ---------------------------------

void FacultyManager::addBuildingToFaculty(Building* b, Faculty* f) { //
    if (!b || !f) return; //

    // Fix 1: Idempotency Protection - If building is already added here, stop!
    if (b->ownedBy == f) return; //

    // Fix 2: Transfer Control - If owned by another faculty, unlink it safely first!
    if (b->ownedBy != nullptr) { //
        removeBuildingFromFaculty(b, b->ownedBy); //
    } //

    f->data.buildings.push_back(b); //
    b->ownedBy = f; //
} //

void FacultyManager::addStudentToFaculty(Student* s, Faculty* f) { //
    if (!s || !f) return; //
    f->data.students.push_back(s); //
    s->employedBy = f; //
} //

void FacultyManager::addLecturerToFaculty(Lecturer* l, Faculty* f) { //
    if (!l || !f) return; //
    f->data.lecturers.push_back(l); //
    l->employedBy = f; //
} //

bool FacultyManager::removeBuildingFromFaculty(Building* b, Faculty* f) { //
    if (!b || !f) return false; //
    for (auto it = f->data.buildings.begin(); it != f->data.buildings.end(); ++it) { //
        if (*it == b) { //
            f->data.buildings.erase(it); //
            b->ownedBy = nullptr; //
            return true; //
        } //
    } //
    return false; //
} //

bool FacultyManager::removePersonFromFaculty(Person* p, Faculty* f) { //
    if (!p || !f) return false; //

    for (auto it = f->data.students.begin(); it != f->data.students.end(); ++it) { //
        if (*it == p) { //
            f->data.students.erase(it); //
            p->employedBy = nullptr; //
            return true; //
        } //
    } //

    for (auto it = f->data.lecturers.begin(); it != f->data.lecturers.end(); ++it) { //
        if (*it == p) { //
            f->data.lecturers.erase(it); //
            p->employedBy = nullptr; //
            return true; //
        } //
    } //
    return false; //
} //

int FacultyManager::getFacultyBuildingsCount(const Faculty* f) { return f ? static_cast<int>(f->data.buildings.size()) : 0; } //
int FacultyManager::getFacultyStudentsCount(const Faculty* f)  { return f ? static_cast<int>(f->data.students.size()) : 0; } //
int FacultyManager::getFacultyLecturersCount(const Faculty* f) { return f ? static_cast<int>(f->data.lecturers.size()) : 0; } //

void FacultyManager::displayFaculty(const Faculty* f) { //
    if (!f) return; //
    cout << "Faculty: " << (f->data.name ? f->data.name : "?") //
         << " | City: " << (f->data.city ? f->data.city : "?") //
         << " | Established: " << f->data.foundedYear //
         << " | Buildings: " << getFacultyBuildingsCount(f) //
         << " | Students: " << getFacultyStudentsCount(f) //
         << " | Lecturers: " << getFacultyLecturersCount(f) << '\n'; //
} //

void FacultyManager::displayFacultyList() const { //
    cout << "--- Faculty List ---\n"; //
    for (Faculty* cur = head; cur; cur = cur->next) displayFaculty(cur); //
    cout << "--------------------\n"; //
} //

void FacultyManager::setCourseManager(CourseManager* mgr) { courseManager = mgr; } //