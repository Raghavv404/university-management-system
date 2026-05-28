#include <gtest/gtest.h> //

#include "../../src/course/CourseManager.h" //
#include "../../src/faculty/FacultyManager.h" //
#include "../../src/building/BuildingManager.h" //
#include "../../src/person/PersonManager.h" //

class CourseManagerTest : public testing::Test { //
protected: //
    CourseManager   cm; //
    FacultyManager  fm; //
    BuildingManager bm; //
    PersonManager   pm; //
    
    Faculty* fcs{nullptr}; //
    Building* sw{nullptr}; //
    Building* small{nullptr}; //
    
    // Updated: We removed the standalone legacy Person pointers.
    // Students and Lecturers are now real Persons directly!
    Student* sAlice{nullptr}; //
    Student* sBob{nullptr}; //
    Lecturer* lDavid{nullptr}; //

    void SetUp() override { //
        cm.setCurrentYear(2026); //
        fm.setCourseManager(&cm); //
        bm.setCourseManager(&cm); //
        pm.setCourseManager(&cm); //

        fm.faculty("CS", POLAND, "Warsaw", 1965); //
        fcs = fm.findFacultyByName("CS"); //

        bm.building("Science Wing", POLAND, "Warsaw", 18, 100); //
        bm.building("Tiny Room",    POLAND, "Warsaw",  1,   2); //
        sw    = bm.findBuildingByName("Science Wing"); //
        small = bm.findBuildingByName("Tiny Room"); //

        // Fixed: Instantiating derived objects straight from the central PersonManager factory
        sAlice = pm.addStudent("Alice", "Nowak", 21, POLAND, BACHELOR); //
        sBob   = pm.addStudent("Bob", "Kowalski", 22, POLAND, BACHELOR); //
        lDavid = pm.addLecturer("David", "Miller", 45, USA, PROFESSOR, "Algorithms"); //

        FacultyManager::addBuildingToFaculty(sw,   fcs); //
        FacultyManager::addStudentToFaculty (sAlice, fcs); //
        FacultyManager::addStudentToFaculty (sBob,   fcs); //
        FacultyManager::addLecturerToFaculty(lDavid, fcs); //
    } //
}; //

// ---------- correct scheduling ---------------------------------------------

TEST_F(CourseManagerTest, ScheduleCourseValid) { //
    Course* c = cm.course("Algorithms", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    ASSERT_NE(c, nullptr); //
    EXPECT_EQ(c->data.ects, 6); //
    EXPECT_EQ(c->data.faculty, fcs); //
} //

// ---------- domain-rule violations -----------------------------------------

TEST_F(CourseManagerTest, ScheduleCoursePre1980Rejected) { //
    EXPECT_EQ(cm.course("Time Travel", Semester{WINTER, 1900}, 6, fcs, lDavid, sw), nullptr); //
} //

TEST_F(CourseManagerTest, ScheduleCourseTooFarInFutureRejected) { //
    EXPECT_EQ(cm.course("Future", Semester{WINTER, 2050}, 6, fcs, lDavid, sw), nullptr); //
} //

TEST_F(CourseManagerTest, EctsTooLowRejected) { //
    EXPECT_EQ(cm.course("Tiny", Semester{WINTER, 2026}, 0, fcs, lDavid, sw), nullptr); //
} //

TEST_F(CourseManagerTest, EctsTooHighRejected) { //
    EXPECT_EQ(cm.course("Mega", Semester{WINTER, 2026}, 99, fcs, lDavid, sw), nullptr); //
} //

TEST_F(CourseManagerTest, NullNameRejected) { //
    EXPECT_EQ(cm.course(nullptr, Semester{WINTER, 2026}, 6, fcs, lDavid, sw), nullptr); //
} //

// ---------- enrollments -----------------------------------------------------

TEST_F(CourseManagerTest, EnrollStudent) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    EXPECT_TRUE (CourseManager::enrollStudent(c, sAlice->id)); //
    EXPECT_FALSE(CourseManager::enrollStudent(c, sAlice->id)); //
} //

TEST_F(CourseManagerTest, CapacityLimitEnforced) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, small); //
    EXPECT_TRUE (CourseManager::enrollStudent(c, sAlice->id)); //
    EXPECT_TRUE (CourseManager::enrollStudent(c, sBob->id)); //
    EXPECT_FALSE(CourseManager::enrollStudent(c, lDavid->id)); // Over limit!
} //

TEST_F(CourseManagerTest, GradeStudentValidRange) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    CourseManager::enrollStudent(c, sAlice->id); //
    EXPECT_TRUE(CourseManager::gradeStudent(c, sAlice->id, 4.5f)); //
} //

TEST_F(CourseManagerTest, GradeOutOfRangeRejected) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    CourseManager::enrollStudent(c, sAlice->id); //
    EXPECT_FALSE(CourseManager::gradeStudent(c, sAlice->id, 7.0f)); //
    EXPECT_FALSE(CourseManager::gradeStudent(c, sAlice->id, 1.0f)); //
} //

TEST_F(CourseManagerTest, UnenrollStudent) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    CourseManager::enrollStudent(c, sAlice->id); //
    EXPECT_TRUE (CourseManager::unenrollStudent(c, sAlice->id)); //
    EXPECT_FALSE(CourseManager::unenrollStudent(c, sAlice->id)); //
} //

// ---------- cascading deletion --------------------------------------------

TEST_F(CourseManagerTest, DeletingPersonRemovesFromCourseEnrollments) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    CourseManager::enrollStudent(c, sAlice->id); //

    // Fixed: Removed the deprecated StudentManager method call completely!
    pm.deletePerson(sAlice->id); //

    int n = 0; //
    for (EnrollmentEntry* e = c->data.enrollments; e; e = e->next) ++n; //
    EXPECT_EQ(n, 0); //
} //

TEST_F(CourseManagerTest, DeletingBuildingNullsCourseBuildingPointer) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    bm.deleteBuilding(sw); //
    EXPECT_EQ(c->data.building, nullptr); //
} //

TEST_F(CourseManagerTest, DeletingFacultyNullsCourseFacultyPointer) { //
    Course* c = cm.course("Algo", Semester{WINTER, 2026}, 6, fcs, lDavid, sw); //
    fm.deleteFaculty(fcs); //
    EXPECT_EQ(c->data.faculty, nullptr); //
} //

// ---------- filtering ------------------------------------------------------

TEST_F(CourseManagerTest, FindCoursesBySemester) { //
    cm.course("A", Semester{WINTER, 2026}, 5, fcs, lDavid, sw); //
    cm.course("B", Semester{SUMMER, 2026}, 5, fcs, lDavid, sw); //
    cm.course("C", Semester{WINTER, 2026}, 5, fcs, lDavid, sw); //
    auto winter = CourseManager::findCoursesBySemester({WINTER, 2026}, cm.getAllCoursesCollection()); //
    EXPECT_EQ(winter.size(), 2u); //
} //

TEST_F(CourseManagerTest, FindCoursesByStudent) { //
    Course* c1 = cm.course("A", Semester{WINTER, 2026}, 5, fcs, lDavid, sw); //
    Course* c2 = cm.course("B", Semester{WINTER, 2026}, 5, fcs, lDavid, sw); //
    CourseManager::enrollStudent(c1, sAlice->id); //
    CourseManager::enrollStudent(c2, sAlice->id); //
    auto mine = CourseManager::findCoursesByStudent(sAlice->id, cm.getAllCoursesCollection()); //
    EXPECT_EQ(mine.size(), 2u); //
} //
