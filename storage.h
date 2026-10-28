/*
 * storage.h  (BACK END)
 * File handling: save the list to a text file and load it back.
 * File format (one line per student):
 *   roll|name|age|gender|dept|year|cgpa|phone|email
 */
#ifndef STORAGE_H
#define STORAGE_H

int fileExists(void);                 /* 1 if students.txt can be opened */
int saveToFile(void);                 /* records saved, or -1 on error   */
int loadFromFile(int *skipped);       /* records added, or -1 if no file;
                                         *skipped = duplicate/invalid    */

#endif
