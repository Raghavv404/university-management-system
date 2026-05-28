#include "CourseManager.h"
#include <iostream>
#include <cstring>

using namespace std;

// ---- constructor / destructor / copy ---------------------------------------

CourseManager::CourseManager() : head(nullptr), currentYear(2026) {}

CourseManager::~CourseManager() {
    deleteAllCourses();
}

CourseManager::CourseManager(const CourseManager& other)
    : head(nullptr), currentYear(other.currentYear) {
    Course* tail = nullptr;
    for (Course* cur = other.head; cur; cur = cur->next) {
        Course* clone = new Course();
        clone->id = cur->id;
        
        // Deep copy course name string
        if (cur->data.name) {
            clone->data.name = new char[strlen(cur->data.name) + 1];
            strcpy(clone->data.name, cur->data.name);
        }
        clone->data.semester = cur->data.semester;
        clone->data.ects = cur->data.ects;
        clone->data.faculty = cur->data.faculty;
        clone->data.lecturer = cur->data.lecturer;
        clone->data.building = cur->data.building;
        
        // Deep copy the intrusive enrollment linked list [cite: 564-565]
        EnrollmentEntry* eTail = nullptr;
        for (EnrollmentEntry* eCur = cur->data.enrollments; eCur; eCur = eCur->next) {
            EnrollmentEntry* eClone = new EnrollmentEntry{eCur->student_id, eCur->final_grade, nullptr};
            if (!clone->data.enrollments) clone->data.enrollments = eClone;
            else                          eTail->next = eClone;
            eTail = eClone;
        }
        
        if (!head) head = clone;
        else       tail->next = clone;
        tail = clone;
    }
}

CourseManager& CourseManager::operator=(const CourseManager& other) {
    if (this == &other) return *this;
    deleteAllCourses();
    currentYear = other.currentYear;
    
    Course* tail = nullptr;
    for (Course* cur = other.head; cur; cur = cur->next) {
        Course* clone = new Course();
        clone->id = cur->id;
        if (cur->data.name) {
            clone->data.name = new char[strlen(cur->data.name) + 1];
            strcpy(clone->data.name, cur->data.name);
        }
        clone->data.semester = cur->data.semester;
        clone->data.ects = cur->data.ects;
        clone->data.faculty = cur->data.faculty;
        clone->data.lecturer = cur->data.lecturer;
        clone->data.building = cur->data.building;
        
        EnrollmentEntry* eTail = nullptr;
        for (EnrollmentEntry* eCur = cur->data.enrollments; eCur; eCur = eCur->next) {
            EnrollmentEntry* eClone = new EnrollmentEntry{eCur->student_id, eCur->final_grade, nullptr};
            if (!clone->data.enrollments) clone->data.enrollments = eClone;
            else                          eTail->next = eClone;
            eTail = eClone;
        }
        
        if (!head) head = clone;
        else       tail->next = clone;
        tail = clone;
    }
    return *this;
}

// ---- Domain Rules & CRUD Operations ---------------------------------------

Course* CourseManager::course(const char* name, Semester semester, int ects,
                              Faculty* faculty, Lecturer* lecturer, Building* building) {
    // 1) Validate name [cite: 634]
    if (!name) {
        cout << "[course] rejected: name cannot be null\n";
        return nullptr;
    }
    // 2) Validate semester date limits [cite: 16, 568-569]
    if (semester.year < 1980 || semester.year > currentYear + 10) {
        cout << "[course] rejected: semester year " << semester.year << " is out of allowed range (1980 .. " << (currentYear + 10) << ")\n";
        return nullptr;
    }
    // 3) Validate ECTS value boundaries [cite: 17, 570]
    if (ects < 1 || ects > 30) {
        cout << "[course] rejected: ects " << ects << " out of range [1,30]\n";
        return nullptr;
    }
    // 4) Soft warning if lecturer isn't employed by this faculty [cite: 574]
    if (lecturer && faculty && lecturer->employedBy != faculty) {
        cout << "[course] warning: assigned lecturer is not employed by this faculty\n";
    }

    Course* c = new Course();
    c->id = idGen.generateId();
    c->data.name = new char[strlen(name) + 1];
    strcpy(c->data.name, name);
    c->data.semester = semester;
    c->data.ects = ects;
    c->data.faculty = faculty;
    c->data.lecturer = lecturer;
    c->data.building = building;
    c->data.enrollments = nullptr;

    c->next = head;
    head = c;
    return c;
}

bool CourseManager::enrollStudent(Course* course, uint32_t studentId) {
    if (!course) return false;

    // Check duplicate registrations [cite: 18, 571]
    int currentEnrollmentCount = 0;
    for (EnrollmentEntry* cur = course->data.enrollments; cur; cur = cur->next) {
        if (cur->student_id == studentId) {
            cout << "[enrollStudent] rejected: student already enrolled\n";
            return false;
        }
        currentEnrollmentCount++;
    }

    // Check building physical capacity constraints [cite: 18, 572-573]
    if (course->data.building && currentEnrollmentCount >= course->data.building->data.maxCapacity) {
        cout << "[enrollStudent] rejected: building capacity reached\n";
        return false;
    }

    // Prepend new node onto the intrusive array [cite: 593]
    EnrollmentEntry* entry = new EnrollmentEntry{studentId, -1.0f, course->data.enrollments};
    course->data.enrollments = entry;
    return true;
}

bool CourseManager::gradeStudent(Course* course, uint32_t studentId, float grade) {
    if (!course) return false;
    // Validate grading standards (Polish 2.0-5.0 system scale) [cite: 263, 389-390]
    if ((grade < 2.0f || grade > 5.0f) && grade != -1.0f) {
        cout << "[gradeStudent] rejected: grade out of range\n";
        return false;
    }
    for (EnrollmentEntry* cur = course->data.enrollments; cur; cur = cur->next) {
        if (cur->student_id == studentId) {
            cur->final_grade = grade;
            return true;
        }
    }
    return false;
}

bool CourseManager::unenrollStudent(Course* course, uint32_t studentId) {
    if (!course) return false;
    EnrollmentEntry* prev = nullptr;
    for (EnrollmentEntry* cur = course->data.enrollments; cur; prev = cur, cur = cur->next) {
        if (cur->student_id == studentId) {
            if (prev) prev->next = cur->next;
            else      course->data.enrollments = cur->next;
            delete cur;
            return true;
        }
    }
    return false;
}

void CourseManager::updateCourse(Course* c, const char* name, Semester* semester, int ects,
                                 Faculty* faculty, Lecturer* lecturer, Building* building) {
    if (!c) return;
    if (name) {
        delete[] c->data.name;
        c->data.name = new char[strlen(name) + 1];
        strcpy(c->data.name, name);
    }
    if (semester) c->data.semester = *semester;
    if (ects >= 1 && ects <= 30) c->data.ects = ects;
    if (faculty) c->data.faculty = faculty;
    if (lecturer) c->data.lecturer = lecturer;
    if (building) c->data.building = building;
}

// ---- Selection Snapshots ---------------------------------------------------

vector<Course*> CourseManager::getAllCoursesCollection() const {
    vector<Course*> out;
    for (Course* cur = head; cur; cur = cur->next) out.push_back(cur);
    return out;
}

Course* CourseManager::findCourseById(uint32_t id, const vector<Course*>& courses) {
    for (Course* c : courses) if (c && c->id == id) return c;
    return nullptr;
}

vector<Course*> CourseManager::filterCourses(const vector<Course*>& courses, const function<bool(Course*)>& predicate) {
    vector<Course*> out;
    for (Course* c : courses) if (c && predicate(c)) out.push_back(c);
    return out;
}

vector<Course*> CourseManager::findCoursesBySemester(Semester semester, const vector<Course*>& courses) {
    return filterCourses(courses, [semester](Course* c) {
        return c->data.semester.term == semester.term && c->data.semester.year == semester.year;
    });
}

vector<Course*> CourseManager::findCoursesByFaculty(Faculty* faculty, const vector<Course*>& courses) {
    return filterCourses(courses, [faculty](Course* c) { return c->data.faculty == faculty; });
}

vector<Course*> CourseManager::findCoursesByLecturer(Lecturer* lecturer, const vector<Course*>& courses) {
    return filterCourses(courses, [lecturer](Course* c) { return c->data.lecturer == lecturer; });
}

vector<Course*> CourseManager::findCoursesByBuilding(Building* building, const vector<Course*>& courses) {
    return filterCourses(courses, [building](Course* c) { return c->data.building == building; });
}

vector<Course*> CourseManager::findCoursesByStudent(uint32_t studentId, const vector<Course*>& courses) {
    return filterCourses(courses, [studentId](Course* c) {
        for (EnrollmentEntry* e = c->data.enrollments; e; e = e->next) {
            if (e->student_id == studentId) return true;
        }
        return false;
    });
}

// ---- Memory Cleanups -------------------------------------------------------

void CourseManager::clearEnrollmentMemory(EnrollmentEntry*& headRef) {
    while (headRef) {
        EnrollmentEntry* doomed = headRef;
        headRef = headRef->next;
        delete doomed;
    }
}

bool CourseManager::deleteCourse(Course* course) {
    if (!course) return false;
    Course* prev = nullptr;
    for (Course* cur = head; cur; prev = cur, cur = cur->next) {
        if (cur == course) {
            if (prev) prev->next = cur->next;
            else      head       = cur->next;
            
            clearEnrollmentMemory(cur->data.enrollments);
            delete[] cur->data.name;
            delete cur;
            return true;
        }
    }
    return false;
}

void CourseManager::deleteAllCourses() {
    while (head) {
        Course* doomed = head;
        head = head->next;
        clearEnrollmentMemory(doomed->data.enrollments);
        delete[] doomed->data.name;
        delete doomed;
    }
}

// ---- FIXED CASCADING HOOKS FOR INHERITANCE POLYMORPHISM --------------------

void CourseManager::removeFacultyFromCourseData(const Faculty* faculty) const {
    for (Course* cur = head; cur; cur = cur->next) {
        if (cur->data.faculty == faculty) cur->data.faculty = nullptr;
    }
}

void CourseManager::removeBuildingFromCourseData(const Building* building) const {
    for (Course* cur = head; cur; cur = cur->next) {
        if (cur->data.building == building) cur->data.building = nullptr;
    }
}

// Look how clean! We inspect cur->data.lecturer directly without a .person hop!
void CourseManager::removePersonFromCourseData(uint32_t personId) const {
    for (Course* cur = head; cur; cur = cur->next) {
        // 1) If the person was the lecturer, clear the reference safely
        if (cur->data.lecturer && cur->data.lecturer->id == personId) {
            cur->data.lecturer = nullptr;
        }
        
        // 2) Purge the student's registration from the intrusive enrollment list
        EnrollmentEntry* prevEntry = nullptr;
        EnrollmentEntry* curEntry = cur->data.enrollments;
        while (curEntry) {
            if (curEntry->student_id == personId) {
                EnrollmentEntry* doomed = curEntry;
                curEntry = curEntry->next;
                if (prevEntry) prevEntry->next = curEntry;
                else           cur->data.enrollments = curEntry;
                delete doomed;
            } else {
                prevEntry = curEntry;
                curEntry = curEntry->next;
            }
        }
    }
}

// ---- Display & Utilities --------------------------------------------------

void CourseManager::displayCourse(const Course* c) {
    if (!c) return;
    cout << "Course: " << (c->data.name ? c->data.name : "?")
         << " | ECTS: " << c->data.ects
         << " | Term: " << (c->data.semester.term == WINTER ? "Winter " : "Summer ") << c->data.semester.year
         << " | Faculty: " << (c->data.faculty && c->data.faculty->data.name ? c->data.faculty->data.name : "Unassigned")
         << " | Lecturer: " << (c->data.lecturer && c->data.lecturer->data.name ? c->data.lecturer->data.name : "Unassigned")
         << " | Building: " << (c->data.building && c->data.building->data.name ? c->data.building->data.name : "Online/None") << '\n';
}

void CourseManager::displayCoursesList() const {
    cout << "--- Course Ledger List ---\n";
    for (Course* cur = head; cur; cur = cur->next) displayCourse(cur);
    cout << "--------------------------\n";
}

void CourseManager::setCurrentYear(int year) { currentYear = year; }