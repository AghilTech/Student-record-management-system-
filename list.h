/*
 * list.h  (BACK END)
 * Singly linked list operations. No printf/scanf here: the back end only
 * works with data and returns results. The front end shows the messages.
 */
#ifndef LIST_H
#define LIST_H

#include "student.h"

/* Sorting options */
#define SORT_BY_ROLL 1
#define SORT_BY_NAME 2
#define SORT_BY_CGPA 3
#define ASCENDING    1
#define DESCENDING   2

extern Student *head;                 /* first node (NULL = empty list) */

Student *findByRoll(int roll);        /* node pointer or NULL      O(n)  */
void     appendNode(Student *node);   /* insert at end             O(n)  */
int      countStudents(void);         /* number of nodes           O(n)  */
int      deleteByRoll(int roll);      /* 1 = deleted, 0 = missing  O(n)  */
void     sortList(int field, int order); /* bubble sort            O(n^2) */
void     freeList(void);              /* free all nodes            O(n)  */

#endif
