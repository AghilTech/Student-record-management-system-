/*
 * utils.h  (COMMON)
 * Pure helper functions for strings and validation.
 * These functions do NO input/output (no printf / scanf).
 */
#ifndef UTILS_H
#define UTILS_H

void trim(char *s);                                   /* remove spaces at both ends   */
int  compareIgnoreCase(const char *a, const char *b); /* like strcmp, ignores case    */
int  containsIgnoreCase(const char *text, const char *part);
int  isValidText(const char *s, const char *extra);   /* letters/spaces + extra chars */
int  isValidEmail(const char *s);

#endif
