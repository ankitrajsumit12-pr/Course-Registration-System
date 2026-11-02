# Viva Voice & Project Defense Guide
**Course Registration & Seat Allocation System**
*Advanced C Programming (ACP) — Class 709 A*

---

## 🎯 Question 1: "Explain your project."
*(The Most Important Question)*

### 💬 Your Ideal Answer:
> **"Sir/Ma'am, our project is a Course Registration and Seat Allocation System built in C.**
> **The entire architecture works around a clean 6-step transactional pipeline:**
>
> 1. **Student** — The student enters their Student ID and Name.
> 2. **Select Course** — The system displays live courses and the student selects a Course Code.
> 3. **Check Seats** — The system verifies that the course exists, that the student is not already registered for it, and that seats are available (`availableSeats > 0`).
> 4. **Register** — A dynamic registration record is created with a unique ID and timestamp.
> 5. **Reduce Available Seats** — The course's available seat count is decremented.
> 6. **Display Registration** — An official registration slip with course and student details is printed on screen."

---

## 🎯 Question 2: "What Advanced C concepts did you use?"

### 💬 Your Ideal Answer:
1. **Dynamic Memory Allocation (`malloc`, `realloc`, `free`)**:
   - For the **Course Catalog**, we use a dynamic array that automatically doubles its capacity using `realloc` when more courses are added.
   - For **Registrations**, each new registration node is dynamically allocated on the heap using `malloc`.
2. **Linked Lists**:
   - Registrations are stored in a singly linked list (`RegistrationNode`), allowing unbounded additions and O(1) appending.
3. **Pointers & Pass-by-Reference**:
   - Functions accept pointers to structures (e.g., `CourseCatalog*`, `RegistrationList*`), avoiding costly memory copies and allowing direct in-memory mutation.
4. **File Handling / Persistence (`fopen`, `fgets`, `fprintf`, `fclose`)**:
   - All courses and registrations are automatically read from and saved to disk (`courses.txt` and `registrations.txt`) in delimiter-separated format.
5. **Defensive Input Handling**:
   - We avoid unsafe functions like `gets()` or plain `scanf("%s")`. Instead, we use `fgets()`, custom newline trimming, and range-validated integer parsing with `strtol()`.

---

## 🎯 Question 3: "Why did you use an array for Courses and a Linked List for Registrations?"

### 💬 Your Ideal Answer:
> - **Course Catalog (Dynamic Array):**
>   "Courses are fixed in number during registration and require fast random access or linear indexing. An array provides contiguous memory and cache-friendly fast lookups."
> - **Registrations (Linked List):**
>   "Registrations happen dynamically and unpredictably as students sign up. A linked list lets us insert new registrations dynamically without pre-allocating large unused memory blocks, and makes dropping/cancelling a registration a simple pointer rewiring operation without shifting array elements."

---

## 🎯 Question 4: "What happens if two students try to register for the last remaining seat?"

### 💬 Your Ideal Answer:
> "In Step 3 (`Check Seats`), the system performs an explicit condition check:
> ```c
> if (selectedCourse->availableSeats <= 0) {
>     printf("Course is FULL!");
>     return false;
> }
> ```
> The first student passes this check, registers, and reduces the seat count to `0` in Step 5.
> When the second student tries to register, the check fails, the transaction is rejected, and no registration node is created."

---

## 🎯 Question 5: "How does dropping/cancelling a course work?"

### 💬 Your Ideal Answer:
> "When a registration is cancelled by its Registration ID:
> 1. We traverse the linked list to find the matching node.
> 2. We identify the associated course code and increment `availableSeats++` in the catalog.
> 3. We unlink the node from the linked list and free its heap memory using `free(curr)`."

---

## 🎯 Question 6: "Show me where the 6-step flow is written in code."

### 💬 Where to point the teacher:
Open [`course_system.c`](file:///f:/Class_709_A/ACP/C%20project/course_system.c) (or [`course_system_standalone.c`](file:///f:/Class_709_A/ACP/C%20project/course_system_standalone.c)) and show the function:

```c
bool executeCourseRegistration(CourseCatalog *catalog, RegistrationList *regList) {
    // [STEP 1/6] Student Identification
    // [STEP 2/6] Select Course
    // [STEP 3/6] Check Seats
    // [STEP 4/6] Register
    // [STEP 5/6] Reduce Available Seats
    // [STEP 6/6] Display Registration
}
```

Notice that each step is explicitly labelled with comments and console logs matching the exact viva explanation!
