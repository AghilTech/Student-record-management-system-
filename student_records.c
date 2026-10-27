/*
 * ======================================================================
 *  PROJECT : Student Record Management System Using Linked List
 *  LANGUAGE: C (ANSI C / C99 compatible)
 *  DATA STRUCTURE : Singly Linked List
 *
 *  Features: Add, Display, Search (Roll/Name), Update, Delete,
 *            Sort (Roll/Name/CGPA), Count, Save/Load (file), Exit
 *
 *  Compile : gcc student_records.c -o student_records
 *  Run     : ./student_records        (Windows: student_records.exe)
 * ======================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "students.txt"   /* file used for Save / Load        */
#define NAME_LEN  50
#define DEPT_LEN  30
#define PHONE_LEN 11               /* 10 digits + '\0'                 */
#define EMAIL_LEN 50

/* ---------------------------- STRUCTURE ----------------------------- */
/* One node of the linked list = one student record + pointer to next   */
typedef struct Student {
    int   roll;
    char  name[NAME_LEN];
    int   age;
    char  gender;                  /* 'M', 'F' or 'O'                  */
    char  dept[DEPT_LEN];
    int   year;                    /* 1 to 4                           */
    float cgpa;                    /* 0.0 to 10.0                      */
    char  phone[PHONE_LEN];
    char  email[EMAIL_LEN];
    struct Student *next;          /* link to the next node            */
} Student;

/* ------------------------- GLOBAL VARIABLES ------------------------- */
Student *head = NULL;              /* head of the list (NULL = empty)  */
int unsaved = 0;                   /* 1 if changes are not saved yet   */

/* --------------------- FUNCTION PROTOTYPES -------------------------- */
void freeList(void);

/* ======================================================================
 *                       INPUT HELPER FUNCTIONS
 * ====================================================================== */

/* Remove spaces from both ends of a string */
void trim(char *s)
{
    int start = 0, end = (int)strlen(s) - 1, i;

    while (s[start] != '\0' && isspace((unsigned char)s[start]))
        start++;
    while (end >= start && isspace((unsigned char)s[end]))
        end--;
    for (i = 0; start + i <= end; i++)
        s[i] = s[start + i];
    s[i] = '\0';
}

/* Safely read one line (fgets is safer than gets/scanf).
 * If input ends (Ctrl+D / Ctrl+Z), free memory and quit cleanly.      */
void readLine(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        printf("\nInput ended. Exiting...\n");
        freeList();
        exit(0);
    }
    if (strchr(buf, '\n') == NULL) {           /* line was too long:   */
        int c;                                  /* discard the extra    */
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    buf[strcspn(buf, "\n")] = '\0';
    trim(buf);
}

/* Read an integer between min and max (repeats until valid) */
int readInt(const char *prompt, int min, int max)
{
    char buf[32], *end;
    long v;

    while (1) {
        readLine(prompt, buf, sizeof(buf));
        v = strtol(buf, &end, 10);
        if (end == buf || *end != '\0') {
            printf("  [!] Invalid input. Enter a whole number.\n");
            continue;
        }
        if (v < min || v > max) {
            printf("  [!] Value must be between %d and %d.\n", min, max);
            continue;
        }
        return (int)v;
    }
}

/* Read a CGPA (0.0 - 10.0) */
float readCGPA(const char *prompt)
{
    char buf[32], *end;
    double v;

    while (1) {
        readLine(prompt, buf, sizeof(buf));
        v = strtod(buf, &end);
        if (end == buf || *end != '\0') {
            printf("  [!] Invalid input. Enter a number like 8.5\n");
            continue;
        }
        if (v < 0.0 || v > 10.0) {
            printf("  [!] CGPA must be between 0.0 and 10.0.\n");
            continue;
        }
        return (float)v;
    }
}

/* Text is valid if: not empty, starts with a letter, and contains only
 * letters, spaces and the extra characters allowed in 'extra'.         */
int isValidText(const char *s, const char *extra)
{
    int i;
    if (s[0] == '\0' || !isalpha((unsigned char)s[0]))
        return 0;
    for (i = 0; s[i] != '\0'; i++) {
        if (!isalpha((unsigned char)s[i]) && s[i] != ' ' &&
            strchr(extra, s[i]) == NULL)
            return 0;
    }
    return 1;
}

/* Read a name (letters, spaces, dot) */
void readName(const char *prompt, char *dest)
{
    char buf[NAME_LEN];
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (isValidText(buf, ".")) {
            strcpy(dest, buf);
            return;
        }
        printf("  [!] Name must start with a letter and contain only letters/spaces.\n");
    }
}

/* Read a department (letters, spaces, & - .) */
void readDept(const char *prompt, char *dest)
{
    char buf[DEPT_LEN];
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (isValidText(buf, "&-.")) {
            strcpy(dest, buf);
            return;
        }
        printf("  [!] Department must contain only letters, spaces, '&' or '-'.\n");
    }
}

/* Read gender: M / F / O */
char readGender(const char *prompt)
{
    char buf[8];
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (strlen(buf) == 1 && strchr("MFOmfo", buf[0]) != NULL)
            return (char)toupper((unsigned char)buf[0]);
        printf("  [!] Enter M (Male), F (Female) or O (Other).\n");
    }
}

/* Read phone: exactly 10 digits, first digit 6-9 (Indian mobile) */
void readPhone(const char *prompt, char *dest)
{
    char buf[32];
    int i, ok;
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        ok = (strlen(buf) == 10 && buf[0] >= '6' && buf[0] <= '9');
        for (i = 0; ok && buf[i] != '\0'; i++)
            if (!isdigit((unsigned char)buf[i]))
                ok = 0;
        if (ok) {
            strcpy(dest, buf);
            return;
        }
        printf("  [!] Phone must be 10 digits and start with 6, 7, 8 or 9.\n");
    }
}

/* Simple email check: one '@', a '.' after it, allowed characters only */
int isValidEmail(const char *s)
{
    int i, at = -1, dot = -1;
    int len = (int)strlen(s);

    if (len < 5)
        return 0;
    for (i = 0; i < len; i++) {
        if (s[i] == '@') {
            if (at != -1)
                return 0;               /* more than one '@'          */
            at = i;
        } else if (s[i] == '.') {
            if (at != -1)
                dot = i;                /* last '.' after the '@'     */
        } else if (!isalnum((unsigned char)s[i]) &&
                   s[i] != '_' && s[i] != '-' && s[i] != '+') {
            return 0;                   /* space, '|' etc. not allowed */
        }
    }
    if (at < 1 || dot == -1) return 0;
    if (dot == at + 1 || dot == len - 1) return 0;
    return 1;
}

void readEmail(const char *prompt, char *dest)
{
    char buf[EMAIL_LEN];
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (isValidEmail(buf)) {
            strcpy(dest, buf);
            return;
        }
        printf("  [!] Invalid email. Example: name@college.edu\n");
    }
}

/* Ask a Yes/No question; returns 1 for yes, 0 for no */
int readYesNo(const char *prompt)
{
    char buf[8];
    while (1) {
        readLine(prompt, buf, sizeof(buf));
        if (strlen(buf) == 1 && (buf[0] == 'y' || buf[0] == 'Y')) return 1;
        if (strlen(buf) == 1 && (buf[0] == 'n' || buf[0] == 'N')) return 0;
        printf("  [!] Please enter Y or N.\n");
    }
}

void pauseScreen(void)
{
    char dummy[8];
    readLine("\nPress Enter to continue...", dummy, sizeof(dummy));
}

/* ======================================================================
 *                   STRING / DISPLAY HELPER FUNCTIONS
 * ====================================================================== */

/* Case-insensitive comparison (like strcmp, ignores upper/lower case) */
int compareIgnoreCase(const char *a, const char *b)
{
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb)
            return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Does 'text' contain 'part' (ignoring case)? Used for name search */
int containsIgnoreCase(const char *text, const char *part)
{
    int i, j, n = (int)strlen(text), m = (int)strlen(part);

    for (i = 0; i + m <= n; i++) {
        for (j = 0; j < m; j++)
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)part[j]))
                break;
        if (j == m)
            return 1;
    }
    return 0;
}

void printLine(void)
{
    printf("+--------+------------------------+-----+-----+----------------------+----+------+------------+------------------------------+\n");
}

void printHeader(void)
{
    printLine();
    printf("| %-6s | %-22s | %-3s | %-3s | %-20s | %-2s | %-4s | %-10s | %-28s |\n",
           "Roll", "Name", "Age", "Sex", "Department", "Yr", "CGPA",
           "Phone", "Email");
    printLine();
}

void printRow(const Student *s)
{
    printf("| %-6d | %-22s | %-3d | %-3c | %-20s | %-2d | %-4.2f | %-10s | %-28s |\n",
           s->roll, s->name, s->age, s->gender, s->dept, s->year,
           s->cgpa, s->phone, s->email);
}

/* ======================================================================
 *                        LINKED LIST OPERATIONS
 * ====================================================================== */

/* Search the list for a roll number. Returns node pointer or NULL.
 * Time: O(n)                                                          */
Student *findByRoll(int roll)
{
    Student *p = head;
    while (p != NULL) {
        if (p->roll == roll)
            return p;
        p = p->next;
    }
    return NULL;
}

/* Insert a node at the END of the list. Time: O(n) (must reach tail)  */
void appendNode(Student *node)
{
    Student *p;

    node->next = NULL;
    if (head == NULL) {                 /* list is empty              */
        head = node;
        return;
    }
    p = head;
    while (p->next != NULL)             /* walk to the last node      */
        p = p->next;
    p->next = node;
}

/* Count nodes by traversal. Time: O(n) */
int countStudents(void)
{
    int count = 0;
    Student *p = head;
    while (p != NULL) {
        count++;
        p = p->next;
    }
    return count;
}

/* Free every node (prevents memory leak). Time: O(n) */
void freeList(void)
{
    Student *p = head, *temp;
    while (p != NULL) {
        temp = p->next;                 /* save next BEFORE freeing   */
        free(p);
        p = temp;
    }
    head = NULL;
}

/* ------------------------- 1. ADD STUDENT --------------------------- */
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
    appendNode(node);
    unsaved = 1;
    printf("\n  [OK] Student added successfully.\n");
}

/* ----------------------- 2. DISPLAY ALL ----------------------------- */
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

/* ------------------------- 3. SEARCH -------------------------------- */
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

/* ------------------------- 4. UPDATE -------------------------------- */
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

/* ------------------------- 5. DELETE -------------------------------- */
void deleteStudent(void)
{
    int roll;
    Student *curr, *prev = NULL;

    printf("\n--- DELETE STUDENT ---\n");
    if (head == NULL) {
        printf("  No records available.\n");
        return;
    }
    roll = readInt("Enter Roll Number to delete: ", 1, 999999);

    curr = head;
    while (curr != NULL && curr->roll != roll) {   /* find the node    */
        prev = curr;                               /* keep the node    */
        curr = curr->next;                         /* before it        */
    }
    if (curr == NULL) {
        printf("  [!] No student found with roll number %d.\n", roll);
        return;
    }

    printHeader();
    printRow(curr);
    printLine();
    if (!readYesNo("Are you sure you want to delete this record? (Y/N): ")) {
        printf("  Deletion cancelled.\n");
        return;
    }

    if (prev == NULL)                   /* deleting the FIRST node      */
        head = curr->next;
    else                                /* middle or last node          */
        prev->next = curr->next;
    free(curr);                         /* release the memory           */
    unsaved = 1;
    printf("  [OK] Record deleted successfully.\n");
}

/* --------------------------- 6. SORT -------------------------------- */

/* Swap the DATA of two nodes but keep their 'next' links unchanged.
 * (Easier for beginners than re-linking the nodes.)                   */
void swapData(Student *a, Student *b)
{
    Student tmp = *a;
    Student *aNext = a->next, *bNext = b->next;

    *a = *b;
    *b = tmp;
    a->next = aNext;                    /* restore original links      */
    b->next = bNext;
}

/* Returns 1 if node a must come AFTER node b for the chosen order */
int shouldSwap(const Student *a, const Student *b, int field, int order)
{
    int cmp;                            /* >0 means a > b              */

    if (field == 1)
        cmp = (a->roll > b->roll) - (a->roll < b->roll);
    else if (field == 2)
        cmp = compareIgnoreCase(a->name, b->name);
    else
        cmp = (a->cgpa > b->cgpa) - (a->cgpa < b->cgpa);

    return (order == 1) ? (cmp > 0) : (cmp < 0);
}

/* Bubble sort on the linked list. Time: O(n^2) */
void sortStudents(void)
{
    int field, order, swapped;
    Student *p, *last = NULL;

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

    do {
        swapped = 0;
        p = head;
        while (p->next != last) {
            if (shouldSwap(p, p->next, field, order)) {
                swapData(p, p->next);
                swapped = 1;
            }
            p = p->next;
        }
        last = p;                       /* last element is now fixed   */
    } while (swapped);

    unsaved = 1;
    printf("  [OK] Records sorted successfully.\n");
    displayAll();
}

/* -------------------------- 7. COUNT -------------------------------- */
void showCount(void)
{
    printf("\n  Total number of students: %d\n", countStudents());
}

/* ======================================================================
 *                           FILE HANDLING
 * ====================================================================== */

/* Save all records to a text file, one record per line, fields
 * separated by the '|' character.                                     */
void saveToFile(void)
{
    FILE *fp;
    Student *p = head;
    int count = 0;

    fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("  [!] Error: could not open \"%s\" for writing.\n", FILE_NAME);
        return;
    }
    while (p != NULL) {
        if (fprintf(fp, "%d|%s|%d|%c|%s|%d|%.2f|%s|%s\n",
                    p->roll, p->name, p->age, p->gender, p->dept,
                    p->year, p->cgpa, p->phone, p->email) < 0) {
            printf("  [!] Error while writing to file.\n");
            fclose(fp);
            return;
        }
        count++;
        p = p->next;
    }
    if (fclose(fp) != 0) {
        printf("  [!] Error: file could not be closed properly.\n");
        return;
    }
    unsaved = 0;
    printf("  [OK] %d record(s) saved to \"%s\".\n", count, FILE_NAME);
}

/* Check that a record read from file has sensible values */
int isRecordValid(const Student *s)
{
    return s->roll >= 1 && s->roll <= 999999 &&
           isValidText(s->name, ".") &&
           s->age >= 15 && s->age <= 60 &&
           strchr("MFO", s->gender) != NULL &&
           isValidText(s->dept, "&-.") &&
           s->year >= 1 && s->year <= 4 &&
           s->cgpa >= 0.0f && s->cgpa <= 10.0f &&
           strlen(s->phone) == 10 &&
           isValidEmail(s->email);
}

/* Load records from the file. Records whose roll number already exists
 * in memory are skipped, so duplicates are never created.             */
void loadFromFile(void)
{
    FILE *fp;
    char line[256];
    Student temp, *node;
    int added = 0, skipped = 0;

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("  [!] No saved file (\"%s\") found.\n", FILE_NAME);
        return;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;                               /* ignore blank line */

        /* %49[^|] reads text up to the next '|' (max 49 characters)   */
        if (sscanf(line, "%d|%49[^|]|%d|%c|%29[^|]|%d|%f|%10[^|]|%49[^|]",
                   &temp.roll, temp.name, &temp.age, &temp.gender,
                   temp.dept, &temp.year, &temp.cgpa, temp.phone,
                   temp.email) != 9 || !isRecordValid(&temp)) {
            skipped++;                              /* corrupted line   */
            continue;
        }
        if (findByRoll(temp.roll) != NULL) {        /* duplicate roll   */
            skipped++;
            continue;
        }
        node = (Student *)malloc(sizeof(Student));
        if (node == NULL) {
            printf("  [!] Memory allocation failed while loading.\n");
            break;
        }
        *node = temp;
        appendNode(node);
        added++;
    }
    fclose(fp);
    printf("  [OK] %d record(s) loaded, %d skipped (duplicate/invalid).\n",
           added, skipped);
}

void fileMenu(void)
{
    int choice;

    printf("\n--- SAVE / LOAD RECORDS ---\n");
    printf("  1. Save records to file\n");
    printf("  2. Load records from file\n");
    choice = readInt("Enter choice: ", 1, 2);
    if (choice == 1)
        saveToFile();
    else
        loadFromFile();
}

/* ======================================================================
 *                              MAIN MENU
 * ====================================================================== */
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

int main(void)
{
    int choice;

    printf("\n  Welcome to the Student Record Management System!\n");

    /* Load previously saved records (if the file exists) */
    {
        FILE *fp = fopen(FILE_NAME, "r");
        if (fp != NULL) {
            fclose(fp);
            printf("  Found saved file. Loading records...\n");
            loadFromFile();
            unsaved = 0;
        }
    }

    do {
        showMenu();
        choice = readInt("  Enter your choice (1-9): ", 1, 9);

        switch (choice) {
            case 1: addStudent();    break;
            case 2: displayAll();    break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: sortStudents();  break;
            case 7: showCount();     break;
            case 8: fileMenu();      break;
            case 9:
                if (unsaved &&
                    readYesNo("  You have unsaved changes. Save now? (Y/N): "))
                    saveToFile();
                break;
        }
        if (choice != 9)
            pauseScreen();
    } while (choice != 9);

    freeList();                         /* release all memory          */
    printf("\n  All memory released. Thank you! Goodbye.\n\n");
    return 0;
}
