#include <gtest/gtest.h> //
#include "person/PersonManager.h" //
#include "course/CourseManager.h" //
#include "utils/Country.h" //

// Test fixture for setting up pristine manager environments
class PersonManagerPolymorphicTest : public ::testing::Test { //
protected: //
    CourseManager* cm{nullptr}; //
    PersonManager* pm{nullptr}; //

    void SetUp() override { //
        cm = new CourseManager(); //
        pm = new PersonManager(cm); //
    } //

    void TearDown() override { //
        delete pm; //
        delete cm; //
    } //
}; //

// 1. Verifies that Students are created cleanly as derived objects
TEST_F(PersonManagerPolymorphicTest, AddStudentDerivedVerification) { //
    Student* s = pm->addStudent("Raghav", "Verma", 20, POLAND, BACHELOR); //
    
    ASSERT_NE(s, nullptr); //
    EXPECT_EQ(s->data.age, 20); //
    EXPECT_EQ(s->level, BACHELOR); //
    EXPECT_FLOAT_EQ(s->gpa, -1.0f); //
    
    // Poly check: Ensure it's retrievable via base class pointer arrays
    Person* p = pm->findPersonById(s->getId()); //
    ASSERT_NE(p, nullptr); //
    EXPECT_STREQ(p->data.name, "Raghav"); //
} //

// 2. Verifies that Lecturers handle specialization strings safely
TEST_F(PersonManagerPolymorphicTest, AddLecturerDerivedVerification) { //
    Lecturer* l = pm->addLecturer("David", "Miller", 42, USA, PROFESSOR, "OOP"); //
    
    ASSERT_NE(l, nullptr); //
    EXPECT_EQ(l->title, PROFESSOR); //
    EXPECT_STREQ(l->specialization, "OOP"); //
    
    Person* p = pm->findPersonById(l->getId()); //
    ASSERT_NE(p, nullptr); //
    EXPECT_STREQ(p->data.surname, "Miller"); //
} //

// 3. Verifies that Rule of Three deep copies work polymorphically without slicing
TEST_F(PersonManagerPolymorphicTest, PolymorphicListDeepCopyNoSlicing) { //
    pm->addStudent("Alice", "Nowak", 21, POLAND, BACHELOR); //
    pm->addLecturer("Bob", "Miller", 45, USA, PROFESSOR, "Physics"); //
    
    // Trigger copy constructor
    PersonManager copiedManager(*pm); //
    
    std::vector<Person*> originalList = pm->getAllPeopleCollection(); //
    std::vector<Person*> copiedList = copiedManager.getAllPeopleCollection(); //
    
    ASSERT_EQ(originalList.size(), copiedList.size()); //
    ASSERT_NE(originalList[0]->getId(), copiedList[0]->getId()); // Confirm deep copy addresses
} //

// 4. Verifies that deleting a Person triggers full structural cascades cleanly
TEST_F(PersonManagerPolymorphicTest, DeletePersonCascadingSafety) { //
    Student* s = pm->addStudent("Bob", "Kowalski", 22, POLAND, BACHELOR); //
    uint32_t savedId = s->getId(); //
    
    bool deleteResult = pm->deletePerson(savedId); //
    EXPECT_TRUE(deleteResult); //
    
    Person* lookup = pm->findPersonById(savedId); //
    EXPECT_EQ(lookup, nullptr); // Confirm completely unlinked and freed
} //