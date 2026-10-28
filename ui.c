/*
 * ui.c  (FRONT END)
 * All printing and user interaction. No pointer manipulation of the list
 * links happens here: that is the job of the back end.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "input.h"
#include "list.h"
#include "storage.h"
#include "utils.h"
#include "student.h"

int unsaved = 0;

/* ------------------------- table printing -------------------------- */
static void printLine(void)
{
    printf("+--------+------------------------+-----+-----+----------------------+----+------+------------+------------------------------+\n");
}

static void printHeader(void)
{
    printLine();
    printf("| %-6s | %-22s | %-3s | %-3s | %-20s | %-2s | %-4s | %-10s | %-28s |\n",
           "Roll", "Name", "Age", "Sex", "Department", "Yr", "CGPA",
           "Phone", "Email");
    printLine();
}

static void printRow(const Student *s)
{
    printf("| %-6d | %-22s | %-3d | %-3c | %-20s | %-2d | %-4.2f | %-10s | %-28s |\n",
           s->roll, s->name, s->age, s->gender, s->dept, s->year,
           s->cgpa, s->phone, s->email);
}

/* ---------------------------- main menu ---------------------------- */
void showMenu(void)
{
    printf("\n");
    printf("+==================================================+\n");
    printf("|     STUDENT RECORD MANAGEMENT SYSTEM             |\n");
    printf("|          (Singly Linked List in C)               |\n");
    printf("+==================================================+\n");
    printf("|   1. Add Student                                 |\n");
    printf("|   2. Display All Students                        |\n");
    printf("|   3. Search Student (Roll No / Name)             |\n");
    printf("|   4. Update Student Details                      |\n");
    printf("|   5. Delete Student (by Roll No)                 |\n");
    printf("|   6. Sort Students (Roll No / Name / CGPA)       |\n");
    printf("|   7. Count Students                              |\n");
    printf("|   8. Save / Load Records (File)                  |\n");
    printf("|   9. Exit                                        |\n");
    printf("+==================================================+\n");
}

/* ------------------------- 1. ADD STUDENT -------------------------- */
void addStudent(void)
{
    Student temp, *node;

    printf("\n--- ADD NEW STUDENT ---\n");

    /* Roll number must be unique */
    while (1) {
        temp.roll = readInt("Roll Number (1-999999): ", 1, 999999);
        if (findByRoll(temp.roll) != NULL)
            printf("  [!] Roll number %d already exists. Try another.\n",
                   temp.roll);
        else
            break;
    }
    readName("Name                  : ", temp.name);
    temp.age    = readInt("Age (15-60)           : ", 15, 60);
    temp.gender = readGender("Gender (M/F/O)        : ");
    readDept("Department            : ", temp.dept);
    temp.year   = readInt("Year (1-4)            : ", 1, 4);
    temp.cgpa   = readCGPA("CGPA (0.0-10.0)       : ");
    readPhone("Phone (10 digits)      : ", temp.phone);
    readEmail("Email                 : ", temp.email);

    /* Allocate memory only after all input is valid */
    node = (Student *)malloc(sizeof(Student));
    if (node == NULL) {
        printf("  [!] Memory allocation failed. Student not added.\n");
        return;
    }
    *node = temp;                       /* copy data into the new node */
    appendNode(node);                   /* back end links it into list */
    unsaved = 1;
    printf("\n  [OK] Student added successfully.\n");
}

/* ----------------------- 2. DISPLAY ALL ---------------------------- */
void displayAll(void)
{
    Student *p = head;

    printf("\n--- ALL STUDENT RECORDS ---\n");
    if (head == NULL) {
        printf("  No records to display.\n");
        return;
    }
    printHeader();
    while (p != NULL) {
        printRow(p);
        p = p->next;
    }
    printLine();
    printf("Total students: %d\n", countStudents());
}

/* ------------------------- 3. SEARCH ------------------------------- */
void searchStudent(void)
{
    int choice, found = 0;

    printf("\n--- SEARCH STUDENT ---\n");
    printf("  1. Search by Roll Number\n");
    printf("  2. Search by Name (partial match allowed)\n");
    choice = readInt("Enter choice: ", 1, 2);

    if (choice == 1) {
        int roll = readInt("Enter Roll Number: ", 1, 999999);
        Student *s = findByRoll(roll);
        if (s == NULL) {
            printf("  [!] No student found with roll number %d.\n", roll);
        } else {
            printHeader();
            printRow(s);
            printLine();
        }
    } else {
        char key[NAME_LEN];
        Student *p = head;
        readLine("Enter name (or part of it): ", key, sizeof(key));
        if (key[0] == '\0') {
            printf("  [!] Search text cannot be empty.\n");
            return;
        }
        while (p != NULL) {
            if (containsIgnoreCase(p->name, key)) {
                if (!found)
                    printHeader();
                printRow(p);
                found++;
            }
            p = p->next;
        }
        if (found) {
            printLine();
            printf("%d record(s) found.\n", found);
        } else {
            printf("  [!] No student found with name containing \"%s\".\n", key);
        }
    }
}

/* ------------------------- 4. UPDATE ------------------------------- */
void updateStudent(void)
{
    int roll, choice;
    Student *s;

    printf("\n--- UPDATE STUDENT ---\n");
    if (head == NULL) {
        printf("  No records available.\n");
        return;
    }
    roll = readInt("Enter Roll Number to update: ", 1, 999999);
    s = findByRoll(roll);
    if (s == NULL) {
        printf("  [!] No student found with roll number %d.\n", roll);
        return;
    }
    printf("\nCurrent record:\n");
    printHeader();
    printRow(s);
    printLine();

    printf("\nWhich detail do you want to update? (Roll Number cannot be changed)\n");
    printf("  1. Name        2. Age        3. Gender\n");
    printf("  4. Department  5. Year       6. CGPA\n");
    printf("  7. Phone       8. Email      9. All details\n");
    choice = readInt("Enter choice: ", 1, 9);

    if (choice == 1 || choice == 9) readName("New Name                  : ", s->name);
    if (choice == 2 || choice == 9) s->age    = readInt("New Age (15-60)           : ", 15, 60);
    if (choice == 3 || choice == 9) s->gender = readGender("New Gender (M/F/O)        : ");
    if (choice == 4 || choice == 9) readDept("New Department            : ", s->dept);
    if (choice == 5 || choice == 9) s->year   = readInt("New Year (1-4)            : ", 1, 4);
    if (choice == 6 || choice == 9) s->cgpa   = readCGPA("New CGPA (0.0-10.0)       : ");
    if (choice == 7 || choice == 9) readPhone("New Phone (10 digits)      : ", s->phone);
    if (choice == 8 || choice == 9) readEmail("New Email                 : ", s->email);

    unsaved = 1;
    printf("\n  [OK] Record updated successfully.\n");
}

/* ------------------------- 5. DELETE ------------------------------- */
void deleteStudent(void)
{
    int roll;
    Student *s;

    printf("\n--- DELETE STUDENT ---\n");
    if (head == NULL) {
        printf("  No records available.\n");
        return;
    }
    roll = readInt("Enter Roll Number to delete: ", 1, 999999);
    s = findByRoll(roll);
    if (s == NULL) {
        printf("  [!] No student found with roll number %d.\n", roll);
        return;
    }

    printHeader();
    printRow(s);
    printLine();
    if (!readYesNo("Are you sure you want to delete this record? (Y/N): ")) {
        printf("  Deletion cancelled.\n");
        return;
    }

    if (deleteByRoll(roll)) {           /* back end unlinks + frees */
        unsaved = 1;
        printf("  [OK] Record deleted successfully.\n");
    }
}

/* --------------------------- 6. SORT ------------------------------- */
void sortStudents(void)
{
    int field, order;

    printf("\n--- SORT STUDENTS ---\n");
    if (countStudents() < 2) {
        printf("  Need at least 2 records to sort.\n");
        return;
    }
    printf("  1. Sort by Roll Number\n");
    printf("  2. Sort by Name\n");
    printf("  3. Sort by CGPA\n");
    field = readInt("Enter choice: ", 1, 3);
    printf("  1. Ascending\n");
    printf("  2. Descending\n");
    order = readInt("Enter order: ", 1, 2);

    sortList(field, order);             /* back end does the sorting */
    unsaved = 1;
    printf("  [OK] Records sorted successfully.\n");
    displayAll();
}

/* -------------------------- 7. COUNT ------------------------------- */
void showCount(void)
{
    printf("\n  Total number of students: %d\n", countStudents());
}

/* ------------------------ 8. SAVE / LOAD --------------------------- */
static void saveRecords(void)
{
    int n = saveToFile();
    if (n < 0) {
        printf("  [!] Error: could not write to \"%s\".\n", FILE_NAME);
    } else {
        unsaved = 0;
        printf("  [OK] %d record(s) saved to \"%s\".\n", n, FILE_NAME);
    }
}

static void loadRecords(void)
{
    int skipped, n = loadFromFile(&skipped);
    if (n < 0)
        printf("  [!] No saved file (\"%s\") found.\n", FILE_NAME);
    else
        printf("  [OK] %d record(s) loaded, %d skipped (duplicate/invalid).\n",
               n, skipped);
}

void fileMenu(void)
{
    int choice;

    printf("\n--- SAVE / LOAD RECORDS ---\n");
    printf("  1. Save records to file\n");
    printf("  2. Load records from file\n");
    choice = readInt("Enter choice: ", 1, 2);
    if (choice == 1)
        saveRecords();
    else
        loadRecords();
}

/* Load automatically when the program starts */
void loadAtStartup(void)
{
    if (fileExists()) {
        printf("  Found saved file. Loading records...\n");
        loadRecords();
        unsaved = 0;
    }
}

/* Called when the user chooses Exit */
void askSaveBeforeExit(void)
{
    if (unsaved &&
        readYesNo("  You have unsaved changes. Save now? (Y/N): "))
        saveRecords();
}
