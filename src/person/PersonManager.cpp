#include "PersonManager.h" //

#include <iostream> //
#include <cstring> //

#include "../faculty/FacultyManager.h" //
#include "../course/CourseManager.h" //

using namespace std; //

// ---- helpers ---------------------------------------------------------------

void PersonManager::clearPersonMemory(Person* person) { //
    if (!person) return; //
    delete person; // Triggers polymorphic deletion via virtual destructor!
} //

Person* PersonManager::copyPerson(const Person* other) { //
    if (!other) return nullptr; //
    return other->clone(); // Safely copies exact Student/Lecturer types without slicing
} //

// ---- ctors / dtor / copy (Rule of Three) -----------------------------------

PersonManager::PersonManager(CourseManager* courseManager) //
    : head(nullptr), courseManager(courseManager) {} //

PersonManager::~PersonManager() { //
    deleteAllPeople(); //
} //

PersonManager::PersonManager(const PersonManager& other) //
    : head(nullptr), idGen(other.idGen), courseManager(other.courseManager) { //
    // Replicate the manager's native push-front pattern to align list topology
    for (Person* cur = other.head; cur; cur = cur->next) { //
        Person* clone = copyPerson(cur); //
        if (clone) { //
            clone->next = head; //
            head = clone; //
        } //
    } //
} //

PersonManager& PersonManager::operator=(const PersonManager& other) { //
    if (this == &other) return *this; //
    deleteAllPeople(); //
    idGen = other.idGen; //
    courseManager = other.courseManager; //
    head = nullptr; //
    
    // Replicate the manager's native push-front pattern to align list topology
    for (Person* cur = other.head; cur; cur = cur->next) { //
        Person* clone = copyPerson(cur); //
        if (clone) { //
            clone->next = head; //
            head = clone; //
        } //
    } //
    return *this; //
} //

// ---- Factory Methods (Polymorphic CRUD) ------------------------------------

Student* PersonManager::addStudent(const char* name, const char* surname, int age, Country nationality, StudyLevel level) { //
    uint32_t id = idGen.generateId(); //
    Student* s = new Student(id, name, surname, age, nationality, level); //
    s->next = head; //
    head = s; //
    return s; //
} //

Lecturer* PersonManager::addLecturer(const char* name, const char* surname, int age, Country nationality, AcademicTitle title, const char* specialization) { //
    uint32_t id = idGen.generateId(); //
    Lecturer* l = new Lecturer(id, name, surname, age, nationality, title, specialization); //
    l->next = head; //
    head = l; //
    return l; //
} //

void PersonManager::updatePerson(Person* p, const char* name, const char* surname, int age, Country nationality) { //
    if (!p) return; //
    if (name) { //
        delete[] p->data.name; //
        p->data.name = new char[strlen(name) + 1]; //
        strcpy(p->data.name, name); //
    } //
    if (surname) { //
        delete[] p->data.surname; //
        p->data.surname = new char[strlen(surname) + 1]; //
        strcpy(p->data.surname, surname); //
    } //
    if (age >= 0) p->data.age = age; //
    p->data.nationality = nationality; //
} //

vector<Person*> PersonManager::getAllPeopleCollection() const { //
    vector<Person*> out; //
    for (Person* cur = head; cur; cur = cur->next) out.push_back(cur); //
    return out; // Non-owning wrapper list
} //

vector<Person*> PersonManager::filterPeople(const vector<Person*>& people, //
                                            const function<bool(Person*)>& predicate) { //
    vector<Person*> out; //
    for (Person* p : people) if (p && predicate(p)) out.push_back(p); //
    return out; //
} //

Person* PersonManager::findPersonById(uint32_t id) const { //
    for (Person* cur = head; cur; cur = cur->next) if (cur->getId() == id) return cur; //
    return nullptr; //
} //

vector<Person*> PersonManager::findPeopleByName(const char* name, const char* surname, //
                                                const vector<Person*>& people) { //
    return filterPeople(people, [name, surname](Person* p) { //
        const bool nameMatch    = !name    || (p->data.name    && strcmp(p->data.name,    name)    == 0); //
        const bool surnameMatch = !surname || (p->data.surname && strcmp(p->data.surname, surname) == 0); //
        return nameMatch && surnameMatch; //
    }); //
} //

vector<Person*> PersonManager::findPeopleByAge(int age, const vector<Person*>& people) { //
    return filterPeople(people, [age](Person* p){ return p->data.age == age; }); //
} //

vector<Person*> PersonManager::findPeopleYoungerThan(int age, const vector<Person*>& people) { //
    return filterPeople(people, [age](Person* p){ return p->data.age < age; }); //
} //

vector<Person*> PersonManager::findPeopleOlderThan(int age, const vector<Person*>& people) { //
    return filterPeople(people, [age](Person* p){ return p->data.age > age; }); //
} //

vector<Person*> PersonManager::findPeopleByNationality(Country nationality, const vector<Person*>& people) { //
    return filterPeople(people, [nationality](Person* p){ return p->data.nationality == nationality; }); //
} //

// ---- Cascading Deletion ----------------------------------------------------

bool PersonManager::deletePerson(uint32_t personId) { //
    Person* prev = nullptr; //
    for (Person* cur = head; cur; prev = cur, cur = cur->next) { //
        if (cur->getId() != personId) continue; //

        // ---- cascading deletion ---- 
        if (cur->getFaculty()) { //
            FacultyManager::removePersonFromFaculty(cur, cur->getFaculty()); //
        } //
        if (courseManager) { //
            courseManager->removePersonFromCourseData(personId); //
        } //

        // ---- unlink and free ----
        if (prev) prev->next = cur->next; //
        else      head       = cur->next; //
        clearPersonMemory(cur); //
        return true; //
    } //
    return false; //
} //

void PersonManager::deleteAllPeople() { //
    while (head) { //
        Person* doomed = head; //
        head = head->next; //
        clearPersonMemory(doomed); //
    } //
} //

// ---- Display (Leverages Runtime Polymorphism) ------------------------------

void PersonManager::displayPerson(const Person* p) { //
    if (!p) return; //
    p->display(); // Triggers overrides down the object tree dynamically!
} //

void PersonManager::displayPeopleList() const { //
    cout << "--- Polymorphic People List ---\n"; //
    for (Person* cur = head; cur; cur = cur->next) displayPerson(cur); //
    cout << "-------------------------------\n"; //
} //