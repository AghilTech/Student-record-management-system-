/*
 * student.h  (COMMON)
 * The Student structure (one linked-list node) and shared constants.
 * Included by both the front end and the back end.
 */
#ifndef STUDENT_H
#define STUDENT_H

#define FILE_NAME "students.txt"   /* file used for Save / Load */
#define NAME_LEN  50
#define DEPT_LEN  30
#define PHONE_LEN 11               /* 10 digits + '\0' */
#define EMAIL_LEN 50

/* One node of the singly linked list = one student + pointer to next */
typedef struct Student {
    int   roll;
    char  name[NAME_LEN];
    int   age;
    char  gender;                  /* 'M', 'F' or 'O' */
    char  dept[DEPT_LEN];
    int   year;                    /* 1 to 4 */
    float cgpa;                    /* 0.0 to 10.0 */
    char  phone[PHONE_LEN];
    char  email[EMAIL_LEN];
    struct Student *next;          /* link to the next node */
} Student;

#endif
