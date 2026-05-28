#include <gtest/gtest.h> //

#include "../../src/faculty/FacultyManager.h" //
#include "../../src/building/BuildingManager.h" //
#include "../../src/person/PersonManager.h" //

class FacultyManagerTest : public testing::Test { //
protected: //
    FacultyManager  fm; //
    BuildingManager bm; //
    PersonManager   pm; //

    Faculty* fcs{nullptr}; //
    Faculty* fphys{nullptr}; //
    Building* mh{nullptr}; //
    Building* sw{nullptr}; //
    
    // Updated: Students and Lecturers are now first-class objects 
    // managed directly via the central PersonManager list
    Student* sAlice{nullptr}; //
    Student* sBob{nullptr}; //
    Lecturer* lDavid{nullptr}; //

    void SetUp() override { //
        fm.faculty("CS",      POLAND, "Warsaw", 1965); //
        fm.faculty("Physics", POLAND, "Warsaw", 1920); //
        fcs   = fm.findFacultyByName("CS"); //
        fphys = fm.findFacultyByName("Physics"); //

        bm.building("Main Hall",    POLAND, "Warsaw", 30, 800); //
        bm.building("Science Wing", POLAND, "Warsaw", 18, 350); //
        mh = bm.findBuildingByName("Main Hall"); //
        sw = bm.findBuildingByName("Science Wing"); //

        // Fixed: Create the human instances directly via polymorphic factories
        sAlice = pm.addStudent("Alice", "Nowak",  21, POLAND, BACHELOR); //
        sBob   = pm.addStudent("Bob", "Kowalski", 22, POLAND, BACHELOR); //
        lDavid = pm.addLecturer("David", "Miller", 45, USA, PROFESSOR, "Algorithms"); //
    } //
}; //

TEST_F(FacultyManagerTest, AllFacultiesPresent) { //
    EXPECT_EQ(fm.getAllFacultiesCollection().size(), 2u); //
} //

TEST_F(FacultyManagerTest, FindByCountry) { //
    auto poles = FacultyManager::findFacultiesByCountry(POLAND, fm.getAllFacultiesCollection()); //
    EXPECT_EQ(poles.size(), 2u); //
} //

TEST_F(FacultyManagerTest, FindOlderThan) { //
    auto old = FacultyManager::findFacultiesOlderThan(1950, fm.getAllFacultiesCollection()); //
    EXPECT_EQ(old.size(), 1u);   // Physics(1920) //
} //

TEST_F(FacultyManagerTest, AddBuildingToFacultySetsBackPointer) { //
    FacultyManager::addBuildingToFaculty(sw, fcs); //
    EXPECT_EQ(sw->ownedBy, fcs); //
    EXPECT_EQ(FacultyManager::getFacultyBuildingsCount(fcs), 1); //
} //

TEST_F(FacultyManagerTest, AddingBuildingToSecondFacultyTransfersIt) { //
    FacultyManager::addBuildingToFaculty(sw, fcs); //
    FacultyManager::addBuildingToFaculty(sw, fphys);   // transfer //
    EXPECT_EQ(sw->ownedBy, fphys); //
    EXPECT_EQ(FacultyManager::getFacultyBuildingsCount(fcs),   0); //
    EXPECT_EQ(FacultyManager::getFacultyBuildingsCount(fphys), 1); //
} //

TEST_F(FacultyManagerTest, AddStudentSetsEmployedBy) { //
    FacultyManager::addStudentToFaculty(sAlice, fcs); //
    EXPECT_EQ(sAlice->employedBy, fcs); // Directly verifies base Person field via inheritance!
    EXPECT_EQ(FacultyManager::getFacultyStudentsCount(fcs), 1); //
} //

TEST_F(FacultyManagerTest, AddLecturerSetsEmployedBy) { //
    FacultyManager::addLecturerToFaculty(lDavid, fcs); //
    EXPECT_EQ(lDavid->employedBy, fcs); // Directly verifies base Person field via inheritance!
    EXPECT_EQ(FacultyManager::getFacultyLecturersCount(fcs), 1); //
} //

TEST_F(FacultyManagerTest, AddSameBuildingTwiceIsIdempotent) { //
    FacultyManager::addBuildingToFaculty(sw, fcs); //
    FacultyManager::addBuildingToFaculty(sw, fcs); //
    EXPECT_EQ(FacultyManager::getFacultyBuildingsCount(fcs), 1); //
} //

TEST_F(FacultyManagerTest, RemovePersonClearsBackPointer) { //
    FacultyManager::addStudentToFaculty(sAlice, fcs); //
    EXPECT_TRUE(FacultyManager::removePersonFromFaculty(sAlice, fcs)); // Upcasting handles this safely!
    EXPECT_EQ(sAlice->employedBy, nullptr); //
} //

TEST_F(FacultyManagerTest, DuplicateNameUpdatesInPlace) { //
    fm.faculty("CS", GERMANY, "Munich", 2000); //
    EXPECT_EQ(fm.getAllFacultiesCollection().size(), 2u); //
    Faculty* f = fm.findFacultyByName("CS"); //
    EXPECT_EQ(f->data.country, GERMANY); //
    EXPECT_STREQ(f->data.city, "Munich"); //
} //

TEST_F(FacultyManagerTest, DeleteFacultyClearsEntityBackPointers) { //
    FacultyManager::addBuildingToFaculty(sw, fcs); //
    FacultyManager::addStudentToFaculty (sAlice, fcs); //
    FacultyManager::addLecturerToFaculty(lDavid, fcs); //
    EXPECT_TRUE(fm.deleteFaculty(fcs)); //

    EXPECT_EQ(sw->ownedBy,        nullptr); //
    EXPECT_EQ(sAlice->employedBy, nullptr); //
    EXPECT_EQ(lDavid->employedBy, nullptr); //
} //