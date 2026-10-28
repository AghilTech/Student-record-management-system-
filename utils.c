/*
 * utils.c  (COMMON)
 * Implementation of string and validation helpers.
 */
#include <string.h>
#include <ctype.h>
#include "utils.h"

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

/* Case-insensitive comparison (<0, 0, >0 like strcmp) */
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

/* Valid if: not empty, starts with a letter, and contains only letters,
 * spaces and the extra characters given in 'extra'.                    */
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
                return 0;               /* more than one '@' */
            at = i;
        } else if (s[i] == '.') {
            if (at != -1)
                dot = i;                /* last '.' after the '@' */
        } else if (!isalnum((unsigned char)s[i]) &&
                   s[i] != '_' && s[i] != '-' && s[i] != '+') {
            return 0;                   /* space, '|' etc. not allowed */
        }
    }
    if (at < 1 || dot == -1) return 0;
    if (dot == at + 1 || dot == len - 1) return 0;
    return 1;
}
