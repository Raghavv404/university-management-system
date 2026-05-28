#include "BuildingManager.h"

#include <iostream>
#include <cstring>

#include "../faculty/FacultyManager.h"
#include "../course/CourseManager.h"

using namespace std;

// ---- helpers ---------------------------------------------------------------

void BuildingManager::clearBuildingMemory(const Building* b) {
    if (!b) return;
    delete[] b->data.name;
    delete[] b->data.city;
    delete b;
}

Building* BuildingManager::copyBuilding(const Building* other) {
    if (!other) return nullptr;
    Building* b = new Building();
    if (other->data.name) {
        b->data.name = new char[strlen(other->data.name) + 1];
        strcpy(b->data.name, other->data.name);
    }
    if (other->data.city) {
        b->data.city = new char[strlen(other->data.city) + 1];
        strcpy(b->data.city, other->data.city);
    }
    b->data.country = other->data.country;
    b->data.roomCount = other->data.roomCount;
    b->data.maxCapacity = other->data.maxCapacity;
    b->ownedBy = nullptr;   // back-pointer cleared in deep copies
    b->next = nullptr;
    return b;
}

// ---- ctors / dtor / copy ---------------------------------------------------

BuildingManager::BuildingManager(CourseManager* courseManager)
    : head(nullptr), courseManager(courseManager) {}

BuildingManager::~BuildingManager() {
    deleteAllBuildings();
}

BuildingManager::BuildingManager(const BuildingManager& other)
    : head(nullptr), courseManager(other.courseManager) {
    Building* tail = nullptr;
    for (Building* cur = other.head; cur; cur = cur->next) {
        Building* clone = copyBuilding(cur);
        if (!head) head = clone;
        else       tail->next = clone;
        tail = clone;
    }
}

BuildingManager& BuildingManager::operator=(const BuildingManager& other) {
    if (this == &other) return *this;
    deleteAllBuildings();
    courseManager = other.courseManager;
    Building* tail = nullptr;
    for (Building* cur = other.head; cur; cur = cur->next) {
        Building* clone = copyBuilding(cur);
        if (!head) head = clone;
        else       tail->next = clone;
        tail = clone;
    }
    return *this;
}

// ---- CRUD ------------------------------------------------------------------

void BuildingManager::building(const char* name, Country country, const char* city, int roomCount, int maxCapacity) {
    if (!name) return;

    // Update path: the name is unique, so a duplicate name updates in place.
    if (Building* existing = findBuildingByName(name)) {
        delete[] existing->data.city;
        existing->data.city = nullptr;
        if (city) {
            existing->data.city = new char[strlen(city) + 1];
            strcpy(existing->data.city, city);
        }
        existing->data.country = country;
        existing->data.roomCount = roomCount;
        existing->data.maxCapacity = maxCapacity;
        return;
    }

    Building* b = new Building();
    b->data.name = new char[strlen(name) + 1]; strcpy(b->data.name, name);
    if (city) {
        b->data.city = new char[strlen(city) + 1]; strcpy(b->data.city, city);
    }
    b->data.country = country;
    b->data.roomCount = roomCount;
    b->data.maxCapacity = maxCapacity;
    b->next = head;
    head = b;
}

void BuildingManager::building(const char* name, Country country, const char* city, int roomCount) {
    building(name, country, city, roomCount, 0);
}

void BuildingManager::building(const char* name, Country country, const char* city) {
    building(name, country, city, 0, 0);
}

vector<Building*> BuildingManager::getAllBuildingsCollection() const {
    vector<Building*> out;
    for (Building* cur = head; cur; cur = cur->next) out.push_back(cur);
    return out;
}

Building* BuildingManager::findBuildingByName(const char* name) const {
    if (!name) return nullptr;
    for (Building* cur = head; cur; cur = cur->next)
        if (cur->data.name && strcmp(cur->data.name, name) == 0) return cur;
    return nullptr;
}

vector<Building*> BuildingManager::filterBuildings(const vector<Building*>& buildings,
                                                   const function<bool(Building*)>& predicate) {
    vector<Building*> out;
    for (Building* b : buildings) if (b && predicate(b)) out.push_back(b);
    return out;
}

vector<Building*> BuildingManager::findBuildingsByCountry(Country country, const vector<Building*>& buildings) {
    return filterBuildings(buildings, [country](Building* b){ return b->data.country == country; });
}

vector<Building*> BuildingManager::findBuildingsByCity(const char* city, const vector<Building*>& buildings) {
    return filterBuildings(buildings, [city](Building* b){
        return city && b->data.city && strcmp(b->data.city, city) == 0;
    });
}

vector<Building*> BuildingManager::findBuildingsByMinCapacity(int minCapacity, const vector<Building*>& buildings) {
    return filterBuildings(buildings, [minCapacity](Building* b){ return b->data.maxCapacity >= minCapacity; });
}

vector<Building*> BuildingManager::findBuildingsByMaxCapacity(int maxCapacity, const vector<Building*>& buildings) {
    return filterBuildings(buildings, [maxCapacity](Building* b){ return b->data.maxCapacity <= maxCapacity; });
}

bool BuildingManager::deleteBuilding(Building* target) {
    if (!target) return false;
    Building* prev = nullptr;
    for (Building* cur = head; cur; prev = cur, cur = cur->next) {
        if (cur != target) continue;

        // Cascading deletion:
        //   - drop from owning Faculty's list (via back-pointer).
        if (cur->ownedBy) {
            FacultyManager::removeBuildingFromFaculty(cur, cur->ownedBy);
        }
        //   - clear from any Course that referenced this Building.
        if (courseManager) {
            courseManager->removeBuildingFromCourseData(cur);
        }

        if (prev) prev->next = cur->next;
        else      head       = cur->next;
        clearBuildingMemory(cur);
        return true;
    }
    return false;
}

void BuildingManager::deleteAllBuildings() {
    while (head) {
        Building* doomed = head;
        head = head->next;
        clearBuildingMemory(doomed);
    }
}

void BuildingManager::displayBuilding(const Building* b) {
    if (!b) return;
    cout << "Building | " << (b->data.name ? b->data.name : "?")
         << " | "         << (b->data.city ? b->data.city : "?")
         << " | rooms="   << b->data.roomCount
         << " | cap="     << b->data.maxCapacity << '\n';
}

void BuildingManager::displayBuildingList() const {
    cout << "--- Buildings ---\n";
    for (Building* cur = head; cur; cur = cur->next) displayBuilding(cur);
    cout << "-----------------\n";
}

void BuildingManager::setCourseManager(CourseManager* mgr) {
    courseManager = mgr;
}
