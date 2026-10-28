/*
 * ui.h  (FRONT END)
 * Menu and screens. Each function talks to the user and then calls the
 * back end (list.h / storage.h) to do the real work.
 */
#ifndef UI_H
#define UI_H

extern int unsaved;              /* 1 = changes not yet saved to file */

void showMenu(void);
void addStudent(void);
void displayAll(void);
void searchStudent(void);
void updateStudent(void);
void deleteStudent(void);
void sortStudents(void);
void showCount(void);
void fileMenu(void);
void loadAtStartup(void);        /* auto-load students.txt if present */
void askSaveBeforeExit(void);    /* ask to save unsaved changes */

#endif
