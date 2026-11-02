#ifndef COURSE_SYSTEM_H
#define COURSE_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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

/**
 * Structure representing an Academic Course.
 * Used inside a dynamic array for fast O(1) indexed access and search.
 */
typedef struct {
    char courseCode[MAX_CODE_LEN];     /* e.g., "CS101" */
    char courseName[MAX_NAME_LEN];     /* e.g., "Advanced C Programming" */
    int credits;                        /* Course credit points (e.g., 4) */
    int totalSeats;                     /* Maximum capacity */
    int availableSeats;                 /* Remaining open seats */
} Course;

/**
 * Dynamic Array wrapper for the Course catalog.
 * Demonstrates Advanced C dynamic memory allocation (malloc, realloc, free).
 */
typedef struct {
    Course *courses;
    int count;
    int capacity;
} CourseCatalog;

/**
 * Singly Linked List Node for Student Registrations.
 * Demonstrates Advanced C dynamic data structures and pointer manipulation.
 */
typedef struct RegistrationNode {
    int registrationId;
    char studentId[MAX_STUDENT_ID];
    char studentName[MAX_STUDENT_NAME];
    char courseCode[MAX_CODE_LEN];
    char registrationDate[32];
    struct RegistrationNode *next;
} RegistrationNode;

/**
 * Linked list container keeping head and count.
 */
typedef struct {
    RegistrationNode *head;
    int count;
    int nextId;
} RegistrationList;

/* ========================================================================= */
/*                           FUNCTION PROTOTYPES                             */
/* ========================================================================= */

/* Catalog Management (Dynamic Array) */
CourseCatalog* createCatalog(int initialCapacity);
bool addCourse(CourseCatalog *catalog, const char *code, const char *name, int credits, int totalSeats);
Course* findCourseByCode(CourseCatalog *catalog, const char *code);
void displayAllCourses(const CourseCatalog *catalog);
bool deleteCourse(CourseCatalog *catalog, RegistrationList *regList, const char *courseCode, int *removedRegs);
void freeCatalog(CourseCatalog *catalog);

/* Registration Management (Linked List) */
RegistrationList* createRegistrationList(void);
bool isStudentAlreadyRegistered(const RegistrationList *list, const char *studentId, const char *courseCode);
RegistrationNode* addRegistration(RegistrationList *list, const char *studentId, const char *studentName, const char *courseCode);
bool cancelRegistration(RegistrationList *list, CourseCatalog *catalog, int registrationId);
void displayAllRegistrations(const RegistrationList *list, const CourseCatalog *catalog);
void displayStudentRegistrations(const RegistrationList *list, const CourseCatalog *catalog, const char *studentId);
int removeRegistrationsByCourse(RegistrationList *list, const char *courseCode);
void freeRegistrationList(RegistrationList *list);

/* Core 6-Step Transaction Workflow (The Teacher's Question) */
/* Flow: Student -> Select Course -> Check Seats -> Register -> Reduce Available Seats -> Display Registration */
bool executeCourseRegistration(CourseCatalog *catalog, RegistrationList *regList);

/* File I/O Persistence */
void saveCatalogToFile(const CourseCatalog *catalog, const char *filename);
bool loadCatalogFromFile(CourseCatalog *catalog, const char *filename);
void saveRegistrationsToFile(const RegistrationList *list, const char *filename);
bool loadRegistrationsFromFile(RegistrationList *list, const char *filename);
void seedDefaultCourses(CourseCatalog *catalog);

/* Utility & Input Helpers */
void getStringInput(const char *prompt, char *buffer, int maxLen);
int getIntegerInput(const char *prompt, int minVal, int maxVal);
void getCurrentTimestamp(char *buffer, int maxLen);
void printHeader(const char *title);
void printDivider(void);
void clearScreen(void);
void pauseExecution(void);

#endif /* COURSE_SYSTEM_H */
