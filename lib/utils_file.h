/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

// Returns the parent directory of 'path' in 'dirname'
void dirname(const char* path, char* dirname);

// Returns a pointer to the filename portion of 'path'
char* basename(const char* path);

// Returns a pointer to the extension portion of 'path'
char* extension(const char* path);

// Returns true 'path' exists
bool exists(const char* path);

// Makes 'path' along with all necessary intermediate directories
bool mkpath(const char* path);

// Returns true if the path references a '.' or '..' directory
bool isdotdir(const char* path);

// Returns the number of entries in a directory
int dirlength(const char* path);

// Removes 'path' recursively
bool rmpath(const char* path);

// Recursively removes all empty directories in 'path'
// Only makes one pass, so if a directory becomes empty
// during this function, it will not be removed
// To ensure all empty directories are removed, enclose this
// function in a while, i.e.  while (rmepmty(path)) ;
// Returns true if empty directories were removed,
// false it not or an error occurred
bool rmempty(const char* path);

// Moves file 'from' to 'to'
bool move(const char* from, const char* to);

#endif