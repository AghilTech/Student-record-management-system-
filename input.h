/*
 * input.h  (FRONT END)
 * Safe keyboard input with validation. Every function keeps asking
 * until the user types a valid value.
 */
#ifndef INPUT_H
#define INPUT_H

void  readLine(const char *prompt, char *buf, int size);
int   readInt(const char *prompt, int min, int max);
float readCGPA(const char *prompt);
void  readName(const char *prompt, char *dest);
void  readDept(const char *prompt, char *dest);
char  readGender(const char *prompt);
void  readPhone(const char *prompt, char *dest);
void  readEmail(const char *prompt, char *dest);
int   readYesNo(const char *prompt);     /* 1 = yes, 0 = no */
void  pauseScreen(void);

#endif
