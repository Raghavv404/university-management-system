#include "BuildingManager.h"
#include <iostream>
#include <cstring>
#include "../faculty/FacultyManager.h"
#include "../course/CourseManager.h"

using namespace std;

// ---- helpers ---------------------------------------------------------------

void BuildingManager::clearBuildingMemory(const Building* b) {
    if (!b) return;
    // FIX: Deleting the pointer now automatically invokes the Building class destructor, 
    // safely freeing name and city raw C-string char allocations.
    delete b;
}

Building* BuildingManager::copyBuilding(const Building* other) {
    if (!other) return nullptr;
    // FIX: Uses the Virtual Clone pattern to prevent breaches of private encapsulation boundaries.
    return other->clone();
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
        // FIX: Replaced direct struct manipulation with the clean, safe updateData() method interface.
        existing->updateData(name, city, roomCount, maxCapacity);
        return;
    }

    // FIX: Replaced explicit raw field allocations with the parameterized Building constructor.
    Building* b = new Building(name, country, city, roomCount, maxCapacity);
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
    for (Building* cur = head; cur; cur = cur->next) {
        // FIX: Used the getName() public accessor instead of un-encapsulated data struct parsing.
        if (cur->getName() && strcmp(cur->getName(), name) == 0) return cur;
    }
    return nullptr;
}

vector<Building*> BuildingManager::filterBuildings(const vector<Building*>& buildings,
                                                   const function<bool(Building*)>& predicate) {
    vector<Building*> out;
    for (Building* b : buildings) if (b && predicate(b)) out.push_back(b);
    return out;
}

vector<Building*> BuildingManager::findBuildingsByCountry(Country country, const vector<Building*>& buildings) {
    // FIX: Replaced b->data.country with b->getCountry() accessor.
    return filterBuildings(buildings, [country](Building* b){ return b->getCountry() == country; });
}

vector<Building*> BuildingManager::findBuildingsByCity(const char* city, const vector<Building*>& buildings) {
    // FIX: Replaced b->data.city with b->getCity() accessor.
    return filterBuildings(buildings, [city](Building* b){
        return city && b->getCity() && strcmp(b->getCity(), city) == 0;
    });
}

vector<Building*> BuildingManager::findBuildingsByMinCapacity(int minCapacity, const vector<Building*>& buildings) {
    // FIX: Replaced b->data.maxCapacity with b->getMaxCapacity() accessor.
    return filterBuildings(buildings, [minCapacity](Building* b){ return b->getMaxCapacity() >= minCapacity; });
}

vector<Building*> BuildingManager::findBuildingsByMaxCapacity(int maxCapacity, const vector<Building*>& buildings) {
    // FIX: Replaced b->data.maxCapacity with b->getMaxCapacity() accessor.
    return filterBuildings(buildings, [maxCapacity](Building* b){ return b->getMaxCapacity() <= maxCapacity; });
}

bool BuildingManager::deleteBuilding(Building* target) {
    if (!target) return false;
    Building* prev = nullptr;
    for (Building* cur = head; cur; prev = cur, cur = cur->next) {
        if (cur != target) continue;

        // Cascading deletion:
        //   - drop from owning Faculty's list (via back-pointer).
        // FIX: Replaced cur->ownedBy with cur->getFaculty() accessor method.
        if (cur->getFaculty()) {
            FacultyManager::removeBuildingFromFaculty(cur, cur->getFaculty());
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
    // FIX: Modernized display loops to leverage your class getters.
    cout << "Building | " << (b->getName() ? b->getName() : "?")
         << " | "         << (b->getCity() ? b->getCity() : "?")
         << " | rooms="   << b->getRoomCount()
         << " | cap="     << b->getMaxCapacity() << '\n';
}

void BuildingManager::displayBuildingList() const {
    cout << "--- Buildings ---\n";
    for (Building* cur = head; cur; cur = cur->next) displayBuilding(cur);
    cout << "-----------------\n";
}

void BuildingManager::setCourseManager(CourseManager* mgr) {
    courseManager = mgr;
}