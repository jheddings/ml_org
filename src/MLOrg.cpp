/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include "ml_org.h"

///////////////////////////////////////////////////////////////////////////////

MLOrg::MLOrg(void) {
}

///////////////////////////////////////////////////////////////////////////////

MLOrgConfig::eOrganizeResult MLOrg::Organize(itemRecord* pRecord) {
    sprintf(tmp, "organizing %s", pRecord->filename);
    logger.Message("MLOrg::Organize()", tmp);

    MLOrgConfig::eOrganizeResult result = MLOrgConfig::kOrganizeOK;
    MLOrgRule* pRule = mlOrgConfig.m_rules.MatchRecord(pRecord);
    if (pRule) {
        // get the base path
        char basedir[MAX_PATH] = { 0 };
        GetBaseDirForRecord(pRecord, basedir);
        if (strlen(basedir) == 0) {
            logger.Warn("MLOrg::Organize()", "NO BASE PATH: skipping");
            return MLOrgConfig::kOrganizeNoBasePath;
        }

        // figure out the new filename
        char newfile[MAX_PATH] = { 0 };
        strcpy(newfile, pRule->FormatString());
        GetFilename(newfile, pRecord);
        sprintf(tmp, "MATCH: %s | %s", pRule->Condition(), newfile);
        logger.Message("MLOrg::Organize()", tmp);

        // rename the current file
        char* filename = (char*) calloc(1, MAX_PATH);
        strcat(filename, basedir);
        strcat(filename, newfile);
        bool moveRet = move(pRecord->filename, filename);
        if (! moveRet && exists(filename)) {
            // this means that there is a file w/ the same name
            TweakFilename(filename);
            moveRet = move(pRecord->filename, filename);
        }

        // if the move failed by now, there was a problem...
        if (! moveRet) {
            sprintf(error, "Could not move file to:\n%s", filename);
            sprintf(tmp, "Could not move file: %s -> %s", pRecord->filename, filename);
            logger.Error("MLOrg::Organize()", tmp);
            return MLOrgConfig::kOrganizeERROR;
        }

        // set the new filename in the record...
        char oldfile[MAX_PATH] = { 0 };
        strcpy(oldfile, pRecord->filename);
        free(pRecord->filename);
        pRecord->filename = filename;

        // remove the old item from the media library
        int remRes = SendMessage(plugin.hwndLibraryParent, WM_ML_IPC,
                                 (WPARAM) oldfile, ML_IPC_DB_REMOVEITEM);
        if (remRes != 1) {
            sprintf(error, "Could not remove old record: (code: %d)", remRes);
            logger.Error("MLOrg::Organize()", error);
            return MLOrgConfig::kOrganizeERROR;
        }

        // add the record w/ the new filename to the ML DB
        int addRes = SendMessage(plugin.hwndLibraryParent, WM_ML_IPC,
                                 (WPARAM) pRecord, ML_IPC_DB_ADDORUPDATEITEM);
        if (addRes != 1) {
            sprintf(error, "Could not add new record: (code: %d)", addRes);
            logger.Error("MLOrg::Organize()", error);
            return MLOrgConfig::kOrganizeERROR;
        }

        sprintf(tmp, "%s -> %s", oldfile, filename);
        logger.Info("MLOrg::Organize()", tmp);
    } else {
        logger.Warn("MLOrg::Organize()", "NO MATCH: skipping");
        result = MLOrgConfig::kOrganizeNoMatch;
    }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrg::RemoveEmptyDirectories(void) {
    char* watchdirs = strdup(mlOrgConfig.m_watchdirs);
    char* watchdir = strtok(watchdirs, "|");
    for ( ; watchdir != NULL; watchdir = strtok(NULL, "|")) {
        logger.Message("MLOrg::RemoveEmptyDirectories()", watchdir);
        rmempty(watchdir);
    }
    delete watchdirs;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrg::GetBaseDirForRecord(itemRecord* pRecord, char* pBaseDir) {
    char* watchdirs = strdup(mlOrgConfig.m_watchdirs);

    // try to find a watch directory
    char* watchdir = strtok(watchdirs, "|");
    for ( ; watchdir != NULL; watchdir = strtok(NULL, "|")) {
        if (strncmp(watchdir, pRecord->filename, strlen(watchdir)) == 0) {
            break;
        }
    }

    // if we found one, use that otherwise, use the default
    strcpy(pBaseDir, (watchdir) ? watchdir : mlOrgConfig.m_path);
    delete watchdirs;

    // make sure that the specified string is a directory string
    if (pBaseDir[strlen(pBaseDir) - 1] != '\\') {
        strcat(pBaseDir, "\\");
    }

    sprintf(tmp, "basedir(%s) : %s", pRecord->filename, pBaseDir);
    logger.Message("MLOrg::GetBaseDirForRecord()", tmp);
}

///////////////////////////////////////////////////////////////////////////////

itemRecordList* MLOrg::GetItemRecordList(void) {
    sprintf(tmp, "querying: %s", mlOrgConfig.m_query);
    logger.Info("MLOrg::GetItemRecordList()", tmp);

    // set up an empty record list
    itemRecordList* records = new itemRecordList();
    records->Size = 0;
    records->Alloc = 0;
    records->Items = NULL;

    // set up the ipc query
    mlQueryStruct query;
    query.max_results = 0;
    query.query = mlOrgConfig.m_query;
    query.results.Size = 0;
    query.results.Alloc = 0;
    query.results.Items = NULL;

    // send the message & copy the results
    SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &query, ML_IPC_DB_RUNQUERY);
    copyRecordList(records, &(query.results));
    SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &query, ML_IPC_DB_FREEQUERYRESULTS);

    // clean up and return
    return records;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrg::ReplaceInvalidFileCharacters(char* str, char ch) {
    // replace reserved OS characters
    strchreplace(str, MLOrgConfig::kInvalidFileCharacters, ch);

    // check if spaces should be replaced
    if (mlOrgConfig.m_replaceSpaces) {
        strchreplace(str, " ", ch);
    }
}

///////////////////////////////////////////////////////////////////////////////

void MLOrg::ReplaceToken(char* str, const char* token,
                         const char* value, const char* dfault) {

    char* tok = strdup(token);
    char* val = (value) ? strdup(value) : strdup(dfault);
    ReplaceInvalidFileCharacters(val, mlOrgConfig.m_syschar);
    strtrim(val);

    // do the standard replacement
    strreplace(str, tok, val);

    // do the lower-case replacement
    strtolower(tok);
    strtolower(val);
    strreplace(str, tok, val);

    // do the upper-case replacement
    strtoupper(tok);
    strtoupper(val);
    strreplace(str, tok, val);
}

///////////////////////////////////////////////////////////////////////////////

char* MLOrg::GetFilename(char* format, itemRecord* pRecord) {
    sprintf(tmp, "formatting: %s for %s - %s",
            format, pRecord->artist, pRecord->title);
    logger.Message("MLOrg::GetFilename()", tmp);

    // track number is special...
    char track[4] = { 0 };
   if (pRecord->track > 0) {
        sprintf(track, "%d", pRecord->track);
       strreplace(format, "<Track>", track);

      sprintf(track, "%02d", pRecord->track);
       strreplace(format, "<Track2>", track);

      sprintf(track, "%03d", pRecord->track);
       strreplace(format, "<Track3>", track);
   } else {
       strreplace(format, "<Track>", "");
       strreplace(format, "<Track2>", "");
       strreplace(format, "<Track3>", "");
   }

    // so is the year, kinda
    char year[5] = { 0 };
   sprintf(year, "%04d", pRecord->year);
   ReplaceToken(format, "<Year>", (pRecord->year > 0) ? year : NULL, "");

    ReplaceToken(format, "<Album>", pRecord->album, "");
   ReplaceToken(format, "<Genre>", pRecord->genre, "");
   ReplaceToken(format, "<Title>", pRecord->title, "");
   ReplaceToken(format, "<Artist>", pRecord->artist, "");

    // add the original extension to the end of the filename
    strcat(format, extension(pRecord->filename));

    sprintf(tmp, "filename: %s", format);
    logger.Message("MLOrg::GetFilename()", tmp);
   return format;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrg::TweakFilename(char* path) {
    char* ext = extension(path);
    char basename[MAX_PATH] = { 0 };
    char tweakBuf[MAX_PATH] = { 0 };

    strncpy(basename, path, (ext - path));

    for (int tweak = 1; exists(path); tweak++) {
        sprintf(tweakBuf, "%s_%d%s", basename, tweak, ext);
        strcpy(path, tweakBuf);
    }
}

///////////////////////////////////////////////////////////////////////////////

char* MLOrg::GetExampleFilename(char* format) {
   itemRecord record;
   record.filename      = "EXAMPLE FILE.mp3";
   record.album         = "On And On";
   record.artist        = "Jack Johnson";
   record.comment       = "Top 5";
   record.genre         = "Blues";
   record.title         = "Taylor";
   record.track         = 4;
   record.year          = 2003;
   record.length        = 0;
   record.extended_info = NULL;
    return GetFilename(format, &record);
}
