/**
 * ============================================================================
 * Project: Course Registration & Seat Allocation System
 * Subject: Advanced C Programming (ACP) - Class 709 A
 * Architecture: Modular Single-File Distribution (Self-Contained)
 *
 * Core Workflow (Viva Answer):
 * Student -> Select Course -> Check Seats -> Register -> Reduce Available Seats -> Display Registration
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_CODE_LEN     16
#define MAX_NAME_LEN     64
#define MAX_STUDENT_ID   20
#define MAX_STUDENT_NAME 64
#define DEFAULT_CAPACITY 10

#define COURSE_FILE       "courses.txt"
#define REGISTRATION_FILE "registrations.txt"

/* ========================================================================= */
/*                              DATA STRUCTURES                              */
/* ========================================================================= */

typedef struct {
    char courseCode[MAX_CODE_LEN];     /* e.g., "CS101" */
    char courseName[MAX_NAME_LEN];     /* e.g., "Advanced C Programming" */
    int credits;                        /* Credit weight */
    int totalSeats;                     /* Total course capacity */
    int availableSeats;                 /* Live remaining seats */
} Course;

typedef struct {
    Course *courses;
    int count;
    int capacity;
} CourseCatalog;

typedef struct RegistrationNode {
    int registrationId;
    char studentId[MAX_STUDENT_ID];
    char studentName[MAX_STUDENT_NAME];
    char courseCode[MAX_CODE_LEN];
    char registrationDate[32];
    struct RegistrationNode *next;
} RegistrationNode;

typedef struct {
    RegistrationNode *head;
    int count;
    int nextId;
} RegistrationList;

/* ========================================================================= */
/*                              STRING UTILITIES                             */
/* ========================================================================= */

static void trimNewline(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

static void toUpperCase(char *dest, const char *src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = (char)toupper((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

static void getStringInput(const char *prompt, char *buffer, int maxLen) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, maxLen, stdin) != NULL) {
            trimNewline(buffer);
            if (strlen(buffer) > 0) return;
        }
        printf("  [!] Input cannot be empty. Please try again.\n");
    }
}

static int getIntegerInput(const char *prompt, int minVal, int maxVal) {
    char buffer[64];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            trimNewline(buffer);
            char *endPtr;
            value = (int)strtol(buffer, &endPtr, 10);
            if (endPtr != buffer && *endPtr == '\0') {
                if (value >= minVal && value <= maxVal) {
                    return value;
                }
            }
        }
        printf("  [!] Please enter a valid number between %d and %d.\n", minVal, maxVal);
    }
}

static void getCurrentTimestamp(char *buffer, int maxLen) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) {
        strftime(buffer, maxLen, "%Y-%m-%d %H:%M:%S", t);
    } else {
        snprintf(buffer, maxLen, "Unknown Date");
    }
}

static void printDivider(void) {
    printf("--------------------------------------------------------------------------------\n");
}

static void printHeader(const char *title) {
    printf("\n");
    printDivider();
    printf("   %s\n", title);
    printDivider();
}

static void pauseExecution(void) {
    printf("\nPress [Enter] to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ========================================================================= */
/*                           COURSE CATALOG (DYNAMIC ARRAY)                  */
/* ========================================================================= */

static CourseCatalog* createCatalog(int initialCapacity) {
    if (initialCapacity <= 0) initialCapacity = DEFAULT_CAPACITY;
    CourseCatalog *catalog = (CourseCatalog*)malloc(sizeof(CourseCatalog));
    if (!catalog) exit(EXIT_FAILURE);
    catalog->courses = (Course*)malloc(sizeof(Course) * initialCapacity);
    if (!catalog->courses) {
        free(catalog);
        exit(EXIT_FAILURE);
    }
    catalog->count = 0;
    catalog->capacity = initialCapacity;
    return catalog;
}

static Course* findCourseByCode(CourseCatalog *catalog, const char *code) {
    if (!catalog || !code) return NULL;
    char target[MAX_CODE_LEN];
    toUpperCase(target, code, MAX_CODE_LEN);
    for (int i = 0; i < catalog->count; i++) {
        if (strcmp(catalog->courses[i].courseCode, target) == 0) {
            return &catalog->courses[i];
        }
    }
    return NULL;
}

static bool addCourse(CourseCatalog *catalog, const char *code, const char *name, int credits, int totalSeats) {
    if (!catalog || !code || !name) return false;
    char normalizedCode[MAX_CODE_LEN];
    toUpperCase(normalizedCode, code, MAX_CODE_LEN);

    if (findCourseByCode(catalog, normalizedCode) != NULL) return false;

    if (catalog->count >= catalog->capacity) {
        int newCap = catalog->capacity * 2;
        Course *nb = (Course*)realloc(catalog->courses, sizeof(Course) * newCap);
        if (!nb) return false;
        catalog->courses = nb;
        catalog->capacity = newCap;
    }

    Course *c = &catalog->courses[catalog->count++];
    strncpy(c->courseCode, normalizedCode, MAX_CODE_LEN - 1);
    c->courseCode[MAX_CODE_LEN - 1] = '\0';
    strncpy(c->courseName, name, MAX_NAME_LEN - 1);
    c->courseName[MAX_NAME_LEN - 1] = '\0';
    c->credits = credits;
    c->totalSeats = totalSeats;
    c->availableSeats = totalSeats;
    return true;
}

static void displayAllCourses(const CourseCatalog *catalog) {
    printHeader("COURSE CATALOG & REAL-TIME SEAT AVAILABILITY");
    if (!catalog || catalog->count == 0) {
        printf("  No courses available in the catalog.\n");
        printDivider();
        return;
    }

    printf("%-10s | %-32s | %-7s | %-6s | %-6s | %-8s\n",
           "Code", "Course Title", "Credits", "Total", "Avail", "Status");
    printDivider();

    for (int i = 0; i < catalog->count; i++) {
        const Course *c = &catalog->courses[i];
        const char *status = (c->availableSeats > 0) ? "OPEN" : "FULL";
        printf("%-10s | %-32s | %-7d | %-6d | %-6d | %-8s\n",
               c->courseCode, c->courseName, c->credits, c->totalSeats, c->availableSeats, status);
    }
    printDivider();
    printf("Total Courses Offered: %d\n", catalog->count);
}

static void freeCatalog(CourseCatalog *catalog) {
    if (catalog) {
        if (catalog->courses) free(catalog->courses);
        free(catalog);
    }
}

/* ========================================================================= */
/*                       REGISTRATION LIST (LINKED LIST)                     */
/* ========================================================================= */

static RegistrationList* createRegistrationList(void) {
    RegistrationList *list = (RegistrationList*)malloc(sizeof(RegistrationList));
    if (!list) exit(EXIT_FAILURE);
    list->head = NULL;
    list->count = 0;
    list->nextId = 1001;
    return list;
}

static bool isStudentAlreadyRegistered(const RegistrationList *list, const char *studentId, const char *courseCode) {
    if (!list || !studentId || !courseCode) return false;
    RegistrationNode *curr = list->head;
    while (curr) {
        if (strcmp(curr->studentId, studentId) == 0 && strcmp(curr->courseCode, courseCode) == 0) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

static RegistrationNode* addRegistration(RegistrationList *list, const char *studentId, const char *studentName, const char *courseCode) {
    if (!list || !studentId || !studentName || !courseCode) return NULL;
    RegistrationNode *node = (RegistrationNode*)malloc(sizeof(RegistrationNode));
    if (!node) return NULL;

    node->registrationId = list->nextId++;
    strncpy(node->studentId, studentId, MAX_STUDENT_ID - 1);
    node->studentId[MAX_STUDENT_ID - 1] = '\0';
    strncpy(node->studentName, studentName, MAX_STUDENT_NAME - 1);
    node->studentName[MAX_STUDENT_NAME - 1] = '\0';
    strncpy(node->courseCode, courseCode, MAX_CODE_LEN - 1);
    node->courseCode[MAX_CODE_LEN - 1] = '\0';
    getCurrentTimestamp(node->registrationDate, sizeof(node->registrationDate));
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
    } else {
        RegistrationNode *curr = list->head;
        while (curr->next) curr = curr->next;
        curr->next = node;
    }
    list->count++;
    return node;
}

static bool cancelRegistration(RegistrationList *list, CourseCatalog *catalog, int registrationId) {
    if (!list || !list->head || !catalog) return false;

    RegistrationNode *curr = list->head;
    RegistrationNode *prev = NULL;

    while (curr != NULL && curr->registrationId != registrationId) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return false;

    Course *course = findCourseByCode(catalog, curr->courseCode);
    if (course && course->availableSeats < course->totalSeats) {
        course->availableSeats++;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    free(curr);
    list->count--;
    return true;
}

static void displayAllRegistrations(const RegistrationList *list, const CourseCatalog *catalog) {
    printHeader("ALL REGISTERED STUDENTS & SEAT ALLOCATIONS");
    if (!list || list->count == 0) {
        printf("  No registrations found in the system.\n");
        printDivider();
        return;
    }

    printf("%-7s | %-12s | %-20s | %-8s | %-25s | %-19s\n",
           "Reg ID", "Student ID", "Student Name", "Course", "Course Title", "Registered At");
    printDivider();

    RegistrationNode *curr = list->head;
    while (curr) {
        const char *courseTitle = "N/A";
        Course *c = findCourseByCode((CourseCatalog*)catalog, curr->courseCode);
        if (c) courseTitle = c->courseName;

        printf("%-7d | %-12s | %-20s | %-8s | %-25.25s | %-19s\n",
               curr->registrationId, curr->studentId, curr->studentName,
               curr->courseCode, courseTitle, curr->registrationDate);
        curr = curr->next;
    }
    printDivider();
    printf("Total Registrations: %d\n", list->count);
}

static void displayStudentRegistrations(const RegistrationList *list, const CourseCatalog *catalog, const char *studentId) {
    printHeader("STUDENT REGISTRATION RECORD");
    if (!list || list->count == 0) {
        printf("  No registrations found in the system.\n");
        printDivider();
        return;
    }

    int foundCount = 0;
    int totalCredits = 0;

    printf("%-7s | %-8s | %-30s | %-7s | %-19s\n",
           "Reg ID", "Course", "Course Title", "Credits", "Registered At");
    printDivider();

    RegistrationNode *curr = list->head;
    while (curr) {
        if (strcmp(curr->studentId, studentId) == 0) {
            foundCount++;
            int credits = 0;
            const char *title = "N/A";
            Course *c = findCourseByCode((CourseCatalog*)catalog, curr->courseCode);
            if (c) {
                title = c->courseName;
                credits = c->credits;
                totalCredits += credits;
            }
            printf("%-7d | %-8s | %-30.30s | %-7d | %-19s\n",
                   curr->registrationId, curr->courseCode, title, credits, curr->registrationDate);
        }
        curr = curr->next;
    }

    printDivider();
    if (foundCount == 0) {
        printf("  No registered courses found for Student ID: %s\n", studentId);
    } else {
        printf("Total Courses: %d | Total Enrolled Credits: %d\n", foundCount, totalCredits);
    }
}

/**
 * Removes every registration node that belongs to the given course code.
 * Seats are NOT restored because the course itself is being deleted.
 * Returns the number of registration records removed.
 */
static int removeRegistrationsByCourse(RegistrationList *list, const char *courseCode) {
    if (!list || !courseCode) return 0;

    char target[MAX_CODE_LEN];
    toUpperCase(target, courseCode, MAX_CODE_LEN);

    int removed = 0;
    RegistrationNode *curr = list->head;
    RegistrationNode *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->courseCode, target) == 0) {
            RegistrationNode *dead = curr;
            curr = curr->next;
            if (prev == NULL) {
                list->head = curr;
            } else {
                prev->next = curr;
            }
            free(dead);
            list->count--;
            removed++;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return removed;
}

/**
 * Deletes a course from the catalog (dynamic array) and cascades the delete
 * to all student registrations of that course. Other registrations and
 * courses are left untouched. Returns true if the course existed.
 */
static bool deleteCourse(CourseCatalog *catalog, RegistrationList *regList, const char *courseCode, int *removedRegs) {
    if (removedRegs) *removedRegs = 0;
    if (!catalog || !courseCode) return false;

    char target[MAX_CODE_LEN];
    toUpperCase(target, courseCode, MAX_CODE_LEN);

    int idx = -1;
    for (int i = 0; i < catalog->count; i++) {
        if (strcmp(catalog->courses[i].courseCode, target) == 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0) return false;

    /* 1. Cascade: remove all registrations for this course */
    int removed = removeRegistrationsByCourse(regList, target);
    if (removedRegs) *removedRegs = removed;

    /* 2. Remove from catalog: shift later elements one slot left */
    for (int i = idx; i < catalog->count - 1; i++) {
        catalog->courses[i] = catalog->courses[i + 1];
    }
    catalog->count--;
    return true;
}

static void freeRegistrationList(RegistrationList *list) {
    if (!list) return;
    RegistrationNode *curr = list->head;
    while (curr) {
        RegistrationNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(list);
}

/* ========================================================================= */
/*           CORE 6-STEP TRANSACTION WORKFLOW (EXACT VIVA FLOW)              */
/* ========================================================================= */

static bool executeCourseRegistration(CourseCatalog *catalog, RegistrationList *regList) {
    printHeader("NEW COURSE REGISTRATION WIZARD");

    /* STEP 1: Student */
    char studentId[MAX_STUDENT_ID];
    char studentName[MAX_STUDENT_NAME];
    printf("[STEP 1/6] Student Identification\n");
    getStringInput("  Enter Student ID (e.g., STU101): ", studentId, sizeof(studentId));
    getStringInput("  Enter Student Full Name        : ", studentName, sizeof(studentName));

    /* STEP 2: Select Course */
    printf("\n[STEP 2/6] Select Course\n");
    displayAllCourses(catalog);

    char courseCode[MAX_CODE_LEN];
    char upperCourseCode[MAX_CODE_LEN];
    getStringInput("\n  Enter Course Code to Register (e.g., CS101): ", courseCode, sizeof(courseCode));
    toUpperCase(upperCourseCode, courseCode, sizeof(upperCourseCode));

    /* STEP 3: Check Seats */
    printf("\n[STEP 3/6] Checking Seat Availability & Eligibility...\n");
    Course *selectedCourse = findCourseByCode(catalog, upperCourseCode);
    if (selectedCourse == NULL) {
        printf("  [X] FAILED: Course code '%s' does not exist in catalog!\n", upperCourseCode);
        return false;
    }

    if (isStudentAlreadyRegistered(regList, studentId, upperCourseCode)) {
        printf("  [X] FAILED: Student [%s] is ALREADY registered for course [%s]!\n",
               studentId, upperCourseCode);
        return false;
    }

    if (selectedCourse->availableSeats <= 0) {
        printf("  [X] FAILED: Sorry, course [%s - %s] is FULL! (0 seats remaining)\n",
               selectedCourse->courseCode, selectedCourse->courseName);
        return false;
    }

    printf("  [OK] Seat Check Passed: %d of %d seats currently available.\n",
           selectedCourse->availableSeats, selectedCourse->totalSeats);

    /* STEP 4: Register */
    printf("\n[STEP 4/6] Registering Student into System...\n");
    RegistrationNode *newReg = addRegistration(regList, studentId, studentName, selectedCourse->courseCode);
    if (!newReg) {
        printf("  [X] FAILED: System memory error while recording registration.\n");
        return false;
    }
    printf("  [OK] Registration recorded successfully (Reg ID: #%d).\n", newReg->registrationId);

    /* STEP 5: Reduce Available Seats */
    printf("\n[STEP 5/6] Updating Course Capacity & Seat Allocation...\n");
    int previousSeats = selectedCourse->availableSeats;
    selectedCourse->availableSeats--;
    printf("  [OK] Seat reduced for %s: %d -> %d available.\n",
           selectedCourse->courseCode, previousSeats, selectedCourse->availableSeats);

    /* STEP 6: Display Registration */
    printf("\n[STEP 6/6] Generating Official Registration Slip...\n\n");
    printf("================================================================\n");
    printf("              OFFICIAL COURSE REGISTRATION SLIP                 \n");
    printf("================================================================\n");
    printf("  Registration ID : #%d\n", newReg->registrationId);
    printf("  Date & Time     : %s\n", newReg->registrationDate);
    printf("  Student ID      : %s\n", newReg->studentId);
    printf("  Student Name    : %s\n", newReg->studentName);
    printf("----------------------------------------------------------------\n");
    printf("  Course Code     : %s\n", selectedCourse->courseCode);
    printf("  Course Title    : %s\n", selectedCourse->courseName);
    printf("  Credits         : %d\n", selectedCourse->credits);
    printf("  Remaining Seats : %d / %d\n", selectedCourse->availableSeats, selectedCourse->totalSeats);
    printf("  Status          : CONFIRMED & ALLOCATED\n");
    printf("================================================================\n");

    return true;
}

/* ========================================================================= */
/*                          FILE I/O PERSISTENCE                             */
/* ========================================================================= */

static void saveCatalogToFile(const CourseCatalog *catalog, const char *filename) {
    if (!catalog || !filename) return;
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    for (int i = 0; i < catalog->count; i++) {
        const Course *c = &catalog->courses[i];
        fprintf(fp, "%s|%s|%d|%d|%d\n",
                c->courseCode, c->courseName, c->credits, c->totalSeats, c->availableSeats);
    }
    fclose(fp);
}

static bool loadCatalogFromFile(CourseCatalog *catalog, const char *filename) {
    if (!catalog || !filename) return false;
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;

    char line[256];
    catalog->count = 0;

    while (fgets(line, sizeof(line), fp)) {
        trimNewline(line);
        if (strlen(line) == 0) continue;

        char code[MAX_CODE_LEN] = {0};
        char name[MAX_NAME_LEN] = {0};
        int credits = 0, total = 0, avail = 0;

        char *token = strtok(line, "|");
        if (token) strncpy(code, token, sizeof(code) - 1);
        token = strtok(NULL, "|");
        if (token) strncpy(name, token, sizeof(name) - 1);
        token = strtok(NULL, "|");
        if (token) credits = atoi(token);
        token = strtok(NULL, "|");
        if (token) total = atoi(token);
        token = strtok(NULL, "|");
        if (token) avail = atoi(token);

        if (strlen(code) > 0 && strlen(name) > 0) {
            if (catalog->count >= catalog->capacity) {
                int newCap = catalog->capacity * 2;
                Course *nb = (Course*)realloc(catalog->courses, sizeof(Course) * newCap);
                if (nb) {
                    catalog->courses = nb;
                    catalog->capacity = newCap;
                }
            }
            Course *dest = &catalog->courses[catalog->count++];
            strncpy(dest->courseCode, code, MAX_CODE_LEN - 1);
            dest->courseCode[MAX_CODE_LEN - 1] = '\0';
            strncpy(dest->courseName, name, MAX_NAME_LEN - 1);
            dest->courseName[MAX_NAME_LEN - 1] = '\0';
            dest->credits = credits;
            dest->totalSeats = total;
            dest->availableSeats = avail;
        }
    }
    fclose(fp);
    return true;
}

static void saveRegistrationsToFile(const RegistrationList *list, const char *filename) {
    if (!list || !filename) return;
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    RegistrationNode *curr = list->head;
    while (curr) {
        fprintf(fp, "%d|%s|%s|%s|%s\n",
                curr->registrationId, curr->studentId, curr->studentName,
                curr->courseCode, curr->registrationDate);
        curr = curr->next;
    }
    fclose(fp);
}

static bool loadRegistrationsFromFile(RegistrationList *list, const char *filename) {
    if (!list || !filename) return false;
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;

    char line[256];
    int maxIdFound = 1000;

    while (fgets(line, sizeof(line), fp)) {
        trimNewline(line);
        if (strlen(line) == 0) continue;

        int regId = 0;
        char sId[MAX_STUDENT_ID] = {0};
        char sName[MAX_STUDENT_NAME] = {0};
        char cCode[MAX_CODE_LEN] = {0};
        char date[32] = {0};

        char *token = strtok(line, "|");
        if (token) regId = atoi(token);
        token = strtok(NULL, "|");
        if (token) strncpy(sId, token, sizeof(sId) - 1);
        token = strtok(NULL, "|");
        if (token) strncpy(sName, token, sizeof(sName) - 1);
        token = strtok(NULL, "|");
        if (token) strncpy(cCode, token, sizeof(cCode) - 1);
        token = strtok(NULL, "|");
        if (token) strncpy(date, token, sizeof(date) - 1);

        if (regId > 0 && strlen(sId) > 0 && strlen(cCode) > 0) {
            RegistrationNode *node = (RegistrationNode*)malloc(sizeof(RegistrationNode));
            if (node) {
                node->registrationId = regId;
                strncpy(node->studentId, sId, MAX_STUDENT_ID - 1);
                node->studentId[MAX_STUDENT_ID - 1] = '\0';
                strncpy(node->studentName, sName, MAX_STUDENT_NAME - 1);
                node->studentName[MAX_STUDENT_NAME - 1] = '\0';
                strncpy(node->courseCode, cCode, MAX_CODE_LEN - 1);
                node->courseCode[MAX_CODE_LEN - 1] = '\0';
                strncpy(node->registrationDate, date, sizeof(node->registrationDate) - 1);
                node->registrationDate[sizeof(node->registrationDate) - 1] = '\0';
                node->next = NULL;

                if (list->head == NULL) {
                    list->head = node;
                } else {
                    RegistrationNode *p = list->head;
                    while (p->next) p = p->next;
                    p->next = node;
                }
                list->count++;
                if (regId > maxIdFound) maxIdFound = regId;
            }
        }
    }
    list->nextId = maxIdFound + 1;
    fclose(fp);
    return true;
}

static void seedDefaultCourses(CourseCatalog *catalog) {
    if (!catalog) return;
    addCourse(catalog, "CS101", "Advanced C Programming", 4, 30);
    addCourse(catalog, "CS102", "Data Structures & Algorithms", 4, 25);
    addCourse(catalog, "CS103", "Operating Systems", 3, 20);
    addCourse(catalog, "CS104", "Database Management Systems", 3, 25);
    addCourse(catalog, "CS105", "Computer Networks", 3, 2);
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
    getStringInput("  Enter Course Code (e.g., CS106): ", code, sizeof(code));
    if (findCourseByCode(catalog, code) != NULL) {
        printf("  [!] Error: Course code '%s' already exists in the catalog!\n", code);
        return;
    }
    getStringInput("  Enter Course Name (e.g., Machine Learning): ", name, sizeof(name));
    int credits = getIntegerInput("  Enter Course Credits (1 - 6): ", 1, 6);
    int seats = getIntegerInput("  Enter Total Seats (1 - 200): ", 1, 200);

    if (addCourse(catalog, code, name, credits, seats)) {
        saveCatalogToFile(catalog, COURSE_FILE);
        printf("  [OK] Course '%s - %s' successfully added with %d seats!\n", code, name, seats);
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
    if (regId == 0) return;

    if (cancelRegistration(regList, catalog, regId)) {
        saveCatalogToFile(catalog, COURSE_FILE);
        saveRegistrationsToFile(regList, REGISTRATION_FILE);
        printf("  [OK] Registration #%d successfully cancelled. 1 seat restored to course!\n", regId);
    } else {
        printf("  [X] Registration ID #%d not found.\n", regId);
    }
}

int main(void) {
    CourseCatalog *catalog = createCatalog(DEFAULT_CAPACITY);
    RegistrationList *regList = createRegistrationList();

    if (!loadCatalogFromFile(catalog, COURSE_FILE)) {
        seedDefaultCourses(catalog);
        saveCatalogToFile(catalog, COURSE_FILE);
    }
    loadRegistrationsFromFile(regList, REGISTRATION_FILE);

    int choice = 0;
    while (1) {
        printf("================================================================================\n");
        printf("        COURSE REGISTRATION & SEAT ALLOCATION SYSTEM - ACP PROJECT             \n");
        printf("                 Advanced C Programming (Class 709 A)                          \n");
        printf("================================================================================\n");
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
            case 4: {
                char sId[MAX_STUDENT_ID];
                getStringInput("  Enter Student ID to Search (e.g., STU101): ", sId, sizeof(sId));
                displayStudentRegistrations(regList, catalog, sId);
                pauseExecution();
                break;
            }
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
                saveCatalogToFile(catalog, COURSE_FILE);
                saveRegistrationsToFile(regList, REGISTRATION_FILE);
                printf("\n  [OK] All catalog and registration data saved successfully.\n");
                printf("  Thank you for using the Course Registration & Seat Allocation System!\n\n");
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
