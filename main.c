/*
 * ======================================================================
 *  PROJECT : Student Record Management System Using Linked List
 *  FILE    : main.c  (entry point)
 *
 *  Project structure
 *    common/   student.h  utils.h  utils.c     shared struct + helpers
 *    backend/  list.h  list.c                  linked list operations
 *              storage.h  storage.c            file handling
 *    frontend/ input.h  input.c                validated keyboard input
 *              ui.h  ui.c                      menu and screens
 *    main.c                                    program start / main loop
 *
 *  Build : make            (or see the gcc command in README.md)
 *  Run   : ./student_records
 * ======================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "input.h"
#include "list.h"

int main(void)
{
    int choice;

    /* If the program ends unexpectedly (e.g. input closed), the list is
     * still freed automatically.                                        */
    atexit(freeList);

    printf("\n  Welcome to the Student Record Management System!\n");
    loadAtStartup();

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
            case 9: askSaveBeforeExit(); break;
        }
        if (choice != 9)
            pauseScreen();
    } while (choice != 9);

    freeList();                         /* release all memory */
    printf("\n  All memory released. Thank you! Goodbye.\n\n");
    return 0;
}
