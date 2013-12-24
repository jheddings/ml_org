/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include <ctype.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

#define kWhitespaceChars " \b\r\f\n\t"

///////////////////////////////////////////////////////////////////////////////

bool strends(const char* str, const char* what) {
    const char* shouldBeHere = str + (strlen(str) - strlen(what));
    return (strstr(str, what) == shouldBeHere);
}

///////////////////////////////////////////////////////////////////////////////

bool strbegins(const char* str, const char* what) {
    return (strstr(str, what) == str);
}

///////////////////////////////////////////////////////////////////////////////

bool strless(const char* str1, const char* str2) {
    return (strcmp(str1, str2) < 0);
}

///////////////////////////////////////////////////////////////////////////////

bool strequal(const char* str1, const char* str2) {
    return (strcmp(str1, str2) == 0);
}

///////////////////////////////////////////////////////////////////////////////

bool strgreater(const char* str1, const char* str2) {
    return (strcmp(str1, str2) > 0);
}

///////////////////////////////////////////////////////////////////////////////

void strtolower(char* str) { _strlwr(str); }

///////////////////////////////////////////////////////////////////////////////

void strtoupper(char* str) { _strupr(str); }

///////////////////////////////////////////////////////////////////////////////

void strreplace(char* str, const char* replace, const char* with) {
    char* remaining = new char[strlen(str)];

    char* pos = strstr(str, replace);
    while (pos) {
        strcpy(remaining, pos + strlen(replace));
        *pos = NULL;
        strcat(str, with);
        strcat(str, remaining);
        pos = strstr(str, replace);
    }

    delete remaining;
}

///////////////////////////////////////////////////////////////////////////////

void strchreplace(char* str, const char* replace, char with) {
    int len = strlen(replace);
    char ch[2] = { with, NULL };
    char rep[2] = { 0 };
    for (int idx = 0; idx < len; idx++) {
        rep[0] = replace[idx];
        strreplace(str, rep, ch);
    }
}

///////////////////////////////////////////////////////////////////////////////

void strtrim(char* str) {
    while (iswspace(str[strlen(str) - 1])) {
        str[strlen(str) - 1] = NULL;
    }
}