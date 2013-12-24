/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __MLORG_H__
#define __MLORG_H__

#include "MLOrgConfig.h"

class MLOrg {
    ///////////////////////////////////////////////////////////////////////////
    // M E T H O D S
    public:
        MLOrg(void);

        // Organizes the specified record.
        MLOrgConfig::eOrganizeResult Organize(itemRecord* pRecord);

        // Removes empty folders from the watch directories.
        void RemoveEmptyDirectories(void);

        // Returns all records in the DB that match the internal filters.
        itemRecordList* GetItemRecordList(void);

        // Renames the specified file into the new directory.
        bool RenameFile(const char* from, const char* to);

        // Uses the format to determine the new filename
        static char* GetFilename(char* format, itemRecord* pRecord);

        // Uses the format to determine a sample filename
        static char* GetExampleFilename(char* format);

    ///////////////////////////////////////////////////////////////////////////
    // P R I V A T E
    private:
        // Returns an estimate of what the query string size will be.
        size_t EstimateQueryStringSize(void);

        // Determines the watch directory the record resides in
        void GetBaseDirForRecord(itemRecord* pRecord, char* pBaseDir);

        // Adds numbers to the end of the filename until it is unique.
        void TweakFilename(char* path);

        // replaces all invalid system characters in the string
        static void ReplaceInvalidFileCharacters(char* str, char ch);

        // Replaces a specific token within the string
        static void ReplaceToken(char* str, const char* token,
                                 const char* value, const char* dfault);
};

#endif
