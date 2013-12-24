/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <shlobj.h>

#include "utils_str.h"

///////////////////////////////////////////////////////////////////////////////

void dirname(const char* path, char* dirname) {
    strncpy(dirname, path, (strrchr(path, '\\') - path));
}

///////////////////////////////////////////////////////////////////////////////

char* basename(const char* path) { return strrchr(path, '\\') + 1; }

///////////////////////////////////////////////////////////////////////////////

char* extension(const char* path) { return strrchr(path, '.'); }

///////////////////////////////////////////////////////////////////////////////

bool exists(const char* path) {
    HANDLE handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    CloseHandle(handle);
    return (handle != INVALID_HANDLE_VALUE);
}

///////////////////////////////////////////////////////////////////////////////

bool isdotdir(const char* path) {
    return (strends(path, ".") || strends(path, ".."));
}

///////////////////////////////////////////////////////////////////////////////

int dirlength(const char* path) {
    char pathSpec[MAX_PATH];
    sprintf(pathSpec, "%s\\*", path);

    WIN32_FIND_DATA findData;
    HANDLE hDir = FindFirstFile(pathSpec, &findData);
    if (hDir == INVALID_HANDLE_VALUE) {
        return 0;
    } 

    DWORD count = (isdotdir(findData.cFileName) ? 0 : 1);
    while (FindNextFile(hDir, &findData)) {
        if (! isdotdir(findData.cFileName)) {
            count++;
        }
    }

    DWORD error = GetLastError();
    FindClose(hDir);
    return (error == ERROR_NO_MORE_FILES) ? count : 0;
}

///////////////////////////////////////////////////////////////////////////////

bool rmempty(const char* path) {
    bool removed = false;
    char pathSpec[MAX_PATH] = { 0 };
    sprintf(pathSpec, "%s\\*", path);

    WIN32_FIND_DATA findData;
    HANDLE hDir = FindFirstFile(pathSpec, &findData);
    if (hDir == INVALID_HANDLE_VALUE) {
        return false;
    }

    char subdirs[MAX_PATH];
    while (FindNextFile(hDir, &findData)) {
        if (isdotdir(findData.cFileName)) {
            continue;
        } else if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            sprintf(subdirs, "%s\\%s", path, findData.cFileName);
            if (rmempty(subdirs)) {
                removed = true;
            }
        }
    }

    DWORD error = GetLastError();
    FindClose(hDir);
    if (error != ERROR_NO_MORE_FILES) {
        return false;
    }

    if (RemoveDirectory(path) == 0) {
        removed = true;
    }

    return removed;
}

///////////////////////////////////////////////////////////////////////////////

bool rmpath(const char* path) {
    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = path;
    fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
    SHFileOperation(&fileOp);
    return (! exists(path));
}

///////////////////////////////////////////////////////////////////////////////

bool mkpath(const char* path) {
    // the easy case...
    if (exists(path)) {
        return true;
    }

    // make sure the parent path is made, then recurse
    char parent[MAX_PATH] = { 0 };
    dirname(path, parent);
    if (mkpath(parent)) {
        mkdir(path);
    }

    // and just make sure that the creation was successful
    return (exists(path));
}

///////////////////////////////////////////////////////////////////////////////

bool move(const char* from, const char* to) {
/* Should this (or something similar) work?
    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.wFunc  = FO_MOVE;
    fileOp.pFrom  = from;
    fileOp.pTo    = to;
    fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
    return (SHFileOperation(&fileOp) == 0);
*/

    // the easy case...
    if (stricmp(from, to) == 0) {
        return true;
    }

    // try to get the parent name of the directory
    char dir[MAX_PATH] = { 0 };
    dirname(to, dir);
    if (strcmp(dir, "") == 0) {
        return false;
    }

    // try to make the path and move the file
    return (mkpath(dir)) ? (MoveFile(from, to) != 0) : false;
}
