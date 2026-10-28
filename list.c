/*
 * list.c  (BACK END)
 * Implementation of the singly linked list.
 */
#include <stdlib.h>
#include "list.h"
#include "utils.h"

Student *head = NULL;                 /* the one and only list head */

/* Search for a roll number. Returns the node or NULL. Time: O(n) */
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

/* Insert a node at the END of the list. Time: O(n) (walk to the tail) */
void appendNode(Student *node)
{
    Student *p;

    node->next = NULL;
    if (head == NULL) {               /* empty list: node becomes head */
        head = node;
        return;
    }
    p = head;
    while (p->next != NULL)           /* walk to the last node */
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

/* Delete the node having this roll number. Time: O(n)
 * Returns 1 if deleted, 0 if the roll number was not found.           */
int deleteByRoll(int roll)
{
    Student *curr = head, *prev = NULL;

    while (curr != NULL && curr->roll != roll) {  /* find the node and  */
        prev = curr;                              /* the node before it */
        curr = curr->next;
    }
    if (curr == NULL)
        return 0;                                 /* not found */

    if (prev == NULL)                 /* deleting the FIRST node */
        head = curr->next;
    else                              /* middle or last node */
        prev->next = curr->next;
    free(curr);                       /* release the memory */
    return 1;
}

/* Free every node (prevents memory leaks). Time: O(n) */
void freeList(void)
{
    Student *p = head, *temp;
    while (p != NULL) {
        temp = p->next;               /* save next BEFORE freeing */
        free(p);
        p = temp;
    }
    head = NULL;
}

/* ---------------------------- SORTING ------------------------------ */

/* Swap the DATA of two nodes but keep their 'next' links unchanged */
static void swapData(Student *a, Student *b)
{
    Student tmp = *a;
    Student *aNext = a->next, *bNext = b->next;

    *a = *b;
    *b = tmp;
    a->next = aNext;                  /* restore the original links */
    b->next = bNext;
}

/* Returns 1 if node a must come AFTER node b for the chosen order */
static int shouldSwap(const Student *a, const Student *b, int field, int order)
{
    int cmp;                          /* >0 means a is greater than b */

    if (field == SORT_BY_ROLL)
        cmp = (a->roll > b->roll) - (a->roll < b->roll);
    else if (field == SORT_BY_NAME)
        cmp = compareIgnoreCase(a->name, b->name);
    else
        cmp = (a->cgpa > b->cgpa) - (a->cgpa < b->cgpa);

    return (order == ASCENDING) ? (cmp > 0) : (cmp < 0);
}

/* Bubble sort on the linked list. Time: O(n^2) */
void sortList(int field, int order)
{
    int swapped;
    Student *p, *last = NULL;

    if (head == NULL || head->next == NULL)
        return;                       /* 0 or 1 node: already sorted */

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
        last = p;                     /* last element is now in place */
    } while (swapped);
}
