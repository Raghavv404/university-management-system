#include <iostream> //
#include "person/PersonManager.h" //
#include "faculty/FacultyManager.h" //
#include "building/BuildingManager.h" //
#include "course/CourseManager.h" //
#include "utils/Country.h" //
#include "utils/Semester.h" //

using namespace std; //

int main() { //
    cout << "==================================================\n"; //
    cout << "  POLYMORPHIC UNIVERSITY MANAGEMENT SYSTEM SYSTEM \n"; //
    cout << "==================================================\n\n"; //

    // 1. Initialize core infrastructure managers & wire them together
    CourseManager   courseManager; //
    BuildingManager buildingManager(&courseManager); //
    FacultyManager  facultyManager(&courseManager); //
    PersonManager   personManager(&courseManager); //

    // 2. Create Faculties [cite: 12, 670]
    cout << "[System] Creating Faculties...\n"; //
    facultyManager.faculty("Faculty of CS", POLAND, "Warsaw", 1965); //
    facultyManager.faculty("Faculty of Physics", POLAND, "Warsaw", 1920); //
    
    Faculty* fcs = facultyManager.findFacultyByName("Faculty of CS"); //
    Faculty* fphys = facultyManager.findFacultyByName("Faculty of Physics"); //

    // 3. Create Campus Buildings [cite: 11, 672-673]
    cout << "[System] Erecting Campus Buildings...\n"; //
    buildingManager.building("Science Wing", POLAND, "Warsaw", 18, 350); //
    buildingManager.building("Main Hall", POLAND, "Warsaw", 30, 800); //
    
    Building* scienceWing = buildingManager.findBuildingByName("Science Wing"); //
    Building* mainHall = buildingManager.findBuildingByName("Main Hall"); //

    // Assign physical infrastructure to respective departments [cite: 682]
    FacultyManager::addBuildingToFaculty(scienceWing, fcs); //
    FacultyManager::addBuildingToFaculty(mainHall, fphys); //

    // 4. Create Human Entities Polymorphically via PersonManager! [cite: 319-320]
    cout << "[System] Populating Human Database (Inheritance Factories)...\n"; //
    
    Student* alice = personManager.addStudent("Alice", "Nowak", 21, POLAND, BACHELOR); //
    Student* bob = personManager.addStudent("Bob", "Kowalski", 22, POLAND, BACHELOR); //
    Lecturer* david = personManager.addLecturer("David", "Miller", 45, USA, PROFESSOR, "Algorithms"); //

    // Assign people straight to their Faculty rosters [cite: 683-685]
    FacultyManager::addStudentToFaculty(alice, fcs); //
    FacultyManager::addStudentToFaculty(bob, fcs); //
    FacultyManager::addLecturerToFaculty(david, fcs); //

    // 5. Schedule a Course [cite: 13-14, 690]
    cout << "[System] Scheduling Autumn Curriculum...\n"; //
    Course* algo = courseManager.course("Algorithms", Semester{WINTER, 2026}, 6, fcs, david, scienceWing); //

    // 6. Polymorphic Registration & Grading Controls [cite: 18, 714]
    cout << "[System] Executing Student Enrollment Ledger...\n"; //
    CourseManager::enrollStudent(algo, alice->getId()); //
    CourseManager::enrollStudent(algo, bob->getId()); //

    // Input some initial grading metrics [cite: 726]
    CourseManager::gradeStudent(algo, alice->getId(), 4.5f); //
    alice->gpa = 4.5f; // 
    bob->gpa = 3.8f;   // 

    // 7. Output System Roster Status Reports via Runtime Polymorphism [cite: 355, 540, 615]
    cout << "\n==================================================\n"; //
    cout << "              UNIVERSITY STATUS DASHBOARD         \n"; //
    cout << "==================================================\n"; //
    facultyManager.displayFacultyList(); //
    courseManager.displayCoursesList(); //
    personManager.displayPeopleList(); // 

    // 8. Test the Cascading Deletion Safety Loops [cite: 20-21, 312-315]
    cout << "\n==================================================\n"; //
    cout << "       EXECUTING CASCADING DELETION SAFETY TEST    \n"; //
    cout << "==================================================\n"; //
    cout << "[Action] Deleting Alice (ID: " << alice->getId() << ") from the master database...\n"; //
    
    personManager.deletePerson(alice->getId()); // 

    cout << "\n[Post-Cascade Check] Updated System Roster:\n"; //
    personManager.displayPeopleList(); //
    facultyManager.displayFacultyList(); // 

    cout << "\n==================================================\n"; //
    cout << "  SYSTEM SHUTDOWN: CLEANING UP ALL MEMORY TREES   \n"; //
    cout << "==================================================\n"; //
    return 0; //
} //