/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

// Returns true if 'str' ends with 'what'
bool strends(const char* str, const char* what);

// Returns true if 'str' begins with 'what'
bool strbegins(const char* str, const char* what);

// Returns true if 'str1' is lexiographically less than 'str2'
bool strless(const char* str1, const char* str2);

// Returns true if 'str1' is lexiographically equal to 'str2'
bool strequal(const char* str1, const char* str2);

// Returns true if 'str1' is lexiographically greater than 'str2'
bool strgreater(const char* str1, const char* str2);

// Replaces all occurences of 'what' in 'str' with 'with'
void strreplace(char* str, const char* what, const char* with);

// Replaces all of the characters in 'what' with 'with' in place
void strchreplace(char* str, const char* what, char with);

// Converts 'str' to lower case in place
void strtolower(char* str);

// Converts 'str' to upper case in place
void strtoupper(char* str);

// Trims all whitespace at the end of the string
void strtrim(char* str);

// Used as hash_compare class for hash_map's with const char* keys
struct cchar_hashcmp {
    enum {
        bucket_size = 4,
        min_buckets = 8
    };

    size_t operator()(const char *s1) const {
        const unsigned char *p = (const unsigned char *) s1;
        size_t hashval = 0;

        while (*p != '\0') {
            hashval += *p++; // or whatever
        }
        return (hashval);
    }

    bool operator()(const char *s1, const char *s2) const {
        return strless(s1, s2);
    }
};

#endif