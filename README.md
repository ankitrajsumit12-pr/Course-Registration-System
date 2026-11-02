# Course Registration & Seat Allocation System
**Advanced C Programming (ACP) Project — Class 709 A**

A clean, robust, and modular C application for academic course registration and dynamic seat allocation with file persistence.

---

## 📌 One-Sentence Project Explanation (Teacher's Question)

If your teacher or external examiner asks:
> **"Explain your project."**

You can simply say:
> **"Our system operates on a 6-step transactional pipeline:**
> **Student → Select Course → Check Seats → Register → Reduce Available Seats → Display Registration."**

---

## 🔄 Core 6-Step Workflow Diagram

```
+---------------------+
| 1. Student          |  Enter Student ID & Full Name
+----------+----------+
           |
           v
+---------------------+
| 2. Select Course    |  Browse live course catalog & select Course Code (e.g. CS101)
+----------+----------+
           |
           v
+---------------------+
| 3. Check Seats      |  Verify course exists, availableSeats > 0, & no duplicate registration
+----------+----------+
           |
           v
+---------------------+
| 4. Register         |  Dynamically allocate and insert registration node into Linked List
+----------+----------+
           |
           v
+---------------------+
| 5. Reduce Seats     |  Atomically decrement available seats for the course (e.g. 30 -> 29)
+----------+----------+
           |
           v
+---------------------+
| 6. Display Slip     |  Generate and print official registration slip / receipt on screen
+---------------------+
```

---

## 🛠️ Advanced C Programming (ACP) Concepts Used

| Concept | Implementation in Code |
| :--- | :--- |
| **Structures (`struct`) & Typedefs** | `Course` and `RegistrationNode` structures representing entities |
| **Dynamic Array (`malloc`, `realloc`)** | `CourseCatalog` dynamically grows when new courses are added |
| **Singly Linked List** | `RegistrationList` dynamically allocates and links student records |
| **Pointers & Pass-by-Reference** | All mutation functions operate via pointers to structs |
| **File I/O Persistence** | Text-based storage in `courses.txt` and `registrations.txt` |
| **Safe Input & String Handling** | Safe buffer reads (`fgets`), newline stripping, case normalization |
| **Modular C Architecture** | Clear separation between header (`.h`), logic (`.c`), and runner (`main.c`) |

---

## 📁 Project Structure

```text
Class_709_A/ACP/C project/
│
├── course_system.h             <- Header file (struct definitions, function prototypes)
├── course_system.c             <- Core business logic, dynamic memory & file I/O
├── main.c                      <- CLI user interface & interactive menu loop
├── course_system_standalone.c  <- Self-contained single-file version (for single-file submission)
├── build.bat                   <- One-click Windows build & run script
├── Makefile                    <- Standard GNU Makefile
├── courses.txt                 <- Auto-saved course catalog on disk
├── registrations.txt           <- Auto-saved student registrations on disk
├── VIVA_GUIDE.md               <- Dedicated Viva questions & answers cheat-sheet
└── README.md                   <- Full project documentation
```

---

## 🚀 How to Compile and Run

### Method 1: Using the Batch Script (Easiest on Windows)
Double-click `build.bat` or run:
```cmd
build.bat
```

### Method 2: Using GCC in Terminal
```bash
gcc -Wall -Wextra -std=c99 -o course_system.exe main.c course_system.c
./course_system.exe
```

### Method 3: Using the Standalone Single-File Version
If your teacher requires submitting only a single `.c` file:
```bash
gcc -Wall -Wextra -std=c99 -o course_system.exe course_system_standalone.c
./course_system.exe
```

---

## 💻 Sample Terminal Output

### Step 1 to 6 in Action:
```text
--------------------------------------------------------------------------------
   NEW COURSE REGISTRATION WIZARD
--------------------------------------------------------------------------------
[STEP 1/6] Student Identification
  Enter Student ID (e.g., STU101): STU2026
  Enter Student Full Name        : Sarah Connor

[STEP 2/6] Select Course
--------------------------------------------------------------------------------
   COURSE CATALOG & REAL-TIME SEAT AVAILABILITY
--------------------------------------------------------------------------------
Code       | Course Title                     | Credits | Total  | Avail  | Status  
--------------------------------------------------------------------------------
CS101      | Advanced C Programming           | 4       | 30     | 30     | OPEN    
CS102      | Data Structures & Algorithms     | 4       | 25     | 25     | OPEN    
CS103      | Operating Systems                | 3       | 20     | 20     | OPEN    
CS104      | Database Management Systems      | 3       | 25     | 25     | OPEN    
CS105      | Computer Networks                | 3       | 2      | 2      | OPEN    
--------------------------------------------------------------------------------
Total Courses Offered: 5

  Enter Course Code to Register (e.g., CS101): CS101

[STEP 3/6] Checking Seat Availability & Eligibility...
  [OK] Seat Check Passed: 30 of 30 seats currently available.

[STEP 4/6] Registering Student into System...
  [OK] Registration recorded successfully (Reg ID: #1001).

[STEP 5/6] Updating Course Capacity & Seat Allocation...
  [OK] Seat reduced for CS101: 30 -> 29 available.

[STEP 6/6] Generating Official Registration Slip...

================================================================
              OFFICIAL COURSE REGISTRATION SLIP                 
================================================================
  Registration ID : #1001
  Date & Time     : 2026-09-29 13:55:18
  Student ID      : STU2026
  Student Name    : Sarah Connor
----------------------------------------------------------------
  Course Code     : CS101
  Course Title    : Advanced C Programming
  Credits         : 4
  Remaining Seats : 29 / 30
  Status          : CONFIRMED & ALLOCATED
================================================================
```

---

## 🗑️ Admin Feature: Delete Course (Menu Option 7)

Option **7** lets the admin delete any course from the catalog. The system asks for the course code, shows how many registrations will be affected, and requires `YES` to confirm.

On deletion:
- The course is removed from the catalog (dynamic array) and `courses.txt`.
- Every registration for that course is removed from the linked list and `registrations.txt`.
- All other courses and registrations stay unchanged.
