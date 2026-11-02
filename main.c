#include "course_system.h"

/* ========================================================================= */
/*                               MAIN MENU                                   */
/* ========================================================================= */

static void printSystemBanner(void) {
    printf("================================================================================\n");
    printf("        COURSE REGISTRATION & SEAT ALLOCATION SYSTEM - ACP PROJECT             \n");
    printf("                 Advanced C Programming (Class 709 A)                          \n");
    printf("================================================================================\n");
}

static void displayProjectExplanation(void) {
    printHeader("PROJECT CORE ARCHITECTURE & WORKFLOW (VIVA EXPLANATION)");
    printf("\n  When asked to 'Explain your project', summarize in one sentence:\n\n");
    printf("  \"Our project models a transactional seat allocation pipeline:\n");
    printf("   Student -> Select Course -> Check Seats -> Register -> Reduce Seats -> Display Slip\"\n\n");
    printf("  +--------------------+      +-----------------------+      +-------------------------+\n");
    printf("  | 1. Student Info    | ---> | 2. Select Course      | ---> | 3. Check Seats          |\n");
    printf("  | (ID & Name Input)  |      | (Live Course Catalog) |      | (Available > 0? No dup) |\n");
    printf("  +--------------------+      +-----------------------+      +-------------------------+\n");
    printf("                                                                          |\n");
    printf("                                                                          v\n");
    printf("  +--------------------+      +-----------------------+      +-------------------------+\n");
    printf("  | 6. Display Slip    | <--- | 5. Reduce Seats       | <--- | 4. Register             |\n");
    printf("  | (Receipt Generated)|      | (AvailableSeats--)    |      | (Linked List Node Added)|\n");
    printf("  +--------------------+      +-----------------------+      +-------------------------+\n\n");
    printf("  Key Advanced C Programming (ACP) Concepts Used:\n");
    printf("   1. Dynamic Array with reallocation (malloc, realloc) for Course Catalog\n");
    printf("   2. Singly Linked List with dynamic nodes for Student Registrations\n");
    printf("   3. Pointer passing and struct manipulation in memory\n");
    printf("   4. Persistent File I/O (File loading, saving, text parsing)\n");
    printf("   5. Robust string processing and input validation\n");
    printDivider();
}

static void handleAddNewCourse(CourseCatalog *catalog) {
    printHeader("ADMIN: ADD NEW COURSE");
    char code[MAX_CODE_LEN];
    char name[MAX_NAME_LEN];
    int credits;
    int seats;

    getStringInput("  Enter Course Code (e.g., CS106): ", code, sizeof(code));
    if (findCourseByCode(catalog, code) != NULL) {
        printf("  [!] Error: Course code '%s' already exists in the catalog!\n", code);
        return;
    }

    getStringInput("  Enter Course Name (e.g., Machine Learning): ", name, sizeof(name));
    credits = getIntegerInput("  Enter Course Credits (1 - 6): ", 1, 6);
    seats = getIntegerInput("  Enter Total Seats (1 - 200): ", 1, 200);

    if (addCourse(catalog, code, name, credits, seats)) {
        saveCatalogToFile(catalog, COURSE_FILE);
        printf("  [OK] Course '%s - %s' successfully added to catalog with %d seats!\n", code, name, seats);
    } else {
        printf("  [X] Failed to add course.\n");
    }
}

static void handleDeleteCourse(CourseCatalog *catalog, RegistrationList *regList) {
    printHeader("ADMIN: DELETE COURSE");
    if (!catalog || catalog->count == 0) {
        printf("  The catalog is empty. Nothing to delete.\n");
        return;
    }

    displayAllCourses(catalog);
    char code[MAX_CODE_LEN];
    getStringInput("\n  Enter Course Code to DELETE (or 0 to abort): ", code, sizeof(code));
    if (strcmp(code, "0") == 0) {
        printf("  Action aborted.\n");
        return;
    }

    Course *course = findCourseByCode(catalog, code);
    if (course == NULL) {
        printf("  [X] Course '%s' not found in the catalog.\n", code);
        return;
    }

    /* Count affected registrations for the confirmation prompt */
    int affected = 0;
    for (RegistrationNode *n = regList->head; n != NULL; n = n->next) {
        if (strcmp(n->courseCode, course->courseCode) == 0) affected++;
    }

    char confirm[8];
    printf("  You are about to delete '%s - %s'.\n", course->courseCode, course->courseName);
    printf("  This will also remove %d student registration(s) for this course.\n", affected);
    getStringInput("  Type YES to confirm: ", confirm, sizeof(confirm));
    if (strcmp(confirm, "YES") != 0 && strcmp(confirm, "yes") != 0) {
        printf("  Action aborted. No changes made.\n");
        return;
    }

    char savedCode[MAX_CODE_LEN];
    strncpy(savedCode, course->courseCode, MAX_CODE_LEN - 1);
    savedCode[MAX_CODE_LEN - 1] = '\0';

    int removed = 0;
    if (deleteCourse(catalog, regList, savedCode, &removed)) {
        saveCatalogToFile(catalog, COURSE_FILE);
        saveRegistrationsToFile(regList, REGISTRATION_FILE);
        printf("  [OK] Course '%s' deleted. %d registration(s) removed. Data files updated.\n", savedCode, removed);
    } else {
        printf("  [X] Failed to delete course.\n");
    }
}

static void handleCancelRegistration(RegistrationList *regList, CourseCatalog *catalog) {
    printHeader("DROP / CANCEL COURSE REGISTRATION");
    if (!regList || regList->count == 0) {
        printf("  No registrations to cancel.\n");
        return;
    }

    displayAllRegistrations(regList, catalog);
    int regId = getIntegerInput("\n  Enter Registration ID to Cancel (0 to abort): ", 0, 999999);
    if (regId == 0) {
        printf("  Action aborted.\n");
        return;
    }

    if (cancelRegistration(regList, catalog, regId)) {
        saveCatalogToFile(catalog, COURSE_FILE);
        saveRegistrationsToFile(regList, REGISTRATION_FILE);
        printf("  [OK] Registration #%d successfully cancelled. 1 seat restored to course!\n", regId);
    } else {
        printf("  [X] Registration ID #%d not found.\n", regId);
    }
}

static void handleSearchStudent(const RegistrationList *regList, const CourseCatalog *catalog) {
    printHeader("SEARCH REGISTRATIONS BY STUDENT ID");
    char studentId[MAX_STUDENT_ID];
    getStringInput("  Enter Student ID to Search (e.g., STU101): ", studentId, sizeof(studentId));
    displayStudentRegistrations(regList, catalog, studentId);
}

int main(void) {
    /* Initialize memory structures */
    CourseCatalog *catalog = createCatalog(DEFAULT_CAPACITY);
    RegistrationList *regList = createRegistrationList();

    /* Load persistent data from disk, or initialize with defaults */
    if (!loadCatalogFromFile(catalog, COURSE_FILE)) {
        seedDefaultCourses(catalog);
        saveCatalogToFile(catalog, COURSE_FILE);
    }
    loadRegistrationsFromFile(regList, REGISTRATION_FILE);

    int choice = 0;
    while (1) {
        printSystemBanner();
        printf("  1. Register for a Course (6-Step Core Workflow)\n");
        printf("  2. View Course Catalog & Seat Availability\n");
        printf("  3. View All Student Registrations\n");
        printf("  4. Search Registrations by Student ID\n");
        printf("  5. Drop / Cancel Course Registration (Restore Seat)\n");
        printf("  6. Add New Course (Admin Feature)\n");
        printf("  7. Delete Course (Admin Feature)\n");
        printf("  8. Explain Project Flow (Teacher Viva Cheat-Sheet)\n");
        printf("  9. Save & Exit System\n");
        printDivider();

        choice = getIntegerInput("  Select an Option [1 - 9]: ", 1, 9);

        switch (choice) {
            case 1:
                if (executeCourseRegistration(catalog, regList)) {
                    /* Automatically commit state to disk after successful transaction */
                    saveCatalogToFile(catalog, COURSE_FILE);
                    saveRegistrationsToFile(regList, REGISTRATION_FILE);
                }
                pauseExecution();
                break;

            case 2:
                displayAllCourses(catalog);
                pauseExecution();
                break;

            case 3:
                displayAllRegistrations(regList, catalog);
                pauseExecution();
                break;

            case 4:
                handleSearchStudent(regList, catalog);
                pauseExecution();
                break;

            case 5:
                handleCancelRegistration(regList, catalog);
                pauseExecution();
                break;

            case 6:
                handleAddNewCourse(catalog);
                pauseExecution();
                break;

            case 8:
                displayProjectExplanation();
                pauseExecution();
                break;

            case 9:
                /* Save everything before exit */
                saveCatalogToFile(catalog, COURSE_FILE);
                saveRegistrationsToFile(regList, REGISTRATION_FILE);
                printf("\n  [OK] All catalog and registration data saved successfully.\n");
                printf("  Thank you for using the Course Registration & Seat Allocation System!\n\n");

                /* Free all allocated heap memory */
                freeCatalog(catalog);
                freeRegistrationList(regList);
                return 0;

            case 7:
                handleDeleteCourse(catalog, regList);
                pauseExecution();
                break;

            default:
                break;
        }
        printf("\n\n");
    }

    return 0;
}
