#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_MANAGER_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_BUILDING_MANAGER_H
#pragma once

#include <vector>
#include <functional>

#include "Building.h"

class CourseManager;     // forward declared - cascading deletion target

// BuildingManager owns every Building in the system.
//
// Invariants:
//   - Building names are unique. Calling building(...) with an existing name
//     triggers an *update* of that record instead of creating a duplicate.
//   - Pointers to a Building handed out by getAllBuildingsCollection() / find...
//     are valid until that Building is deleted via deleteBuilding().
//
// Cascading deletion: deleteBuilding() asks CourseManager to clear every
// Course.data.building that referenced this Building (set to nullptr), and
// asks FacultyManager to remove the building from its owning Faculty's list.
class BuildingManager {
    Building* head;
    CourseManager* courseManager;

    static void clearBuildingMemory(const Building* b);
    static Building* copyBuilding(const Building* other);

public:
    explicit BuildingManager(CourseManager* courseManager = nullptr);
    ~BuildingManager();

    BuildingManager(const BuildingManager& other);
    BuildingManager& operator=(const BuildingManager& other);

    // Overloads that mirror the friend's project: trailing optional arguments.
    // If a building with the given name already exists, the call updates it
    // in-place rather than creating a duplicate.
    void building(const char* name, Country country, const char* city, int roomCount, int maxCapacity);
    void building(const char* name, Country country, const char* city, int roomCount);
    void building(const char* name, Country country, const char* city);

    std::vector<Building*> getAllBuildingsCollection() const;

    Building* findBuildingByName(const char* name) const;

    static std::vector<Building*> filterBuildings(const std::vector<Building*>& buildings,
                                                  const std::function<bool(Building*)>& predicate);
    static std::vector<Building*> findBuildingsByCountry(Country country, const std::vector<Building*>& buildings);
    static std::vector<Building*> findBuildingsByCity(const char* city, const std::vector<Building*>& buildings);
    static std::vector<Building*> findBuildingsByMinCapacity(int minCapacity, const std::vector<Building*>& buildings);
    static std::vector<Building*> findBuildingsByMaxCapacity(int maxCapacity, const std::vector<Building*>& buildings);

    bool deleteBuilding(Building* building);
    void deleteAllBuildings();

    static void displayBuilding(const Building* b);
    void displayBuildingList() const;

    void setCourseManager(CourseManager* mgr);
};

#endif
