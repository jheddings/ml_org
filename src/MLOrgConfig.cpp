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

const char MLOrgConfig::kMLOrgConfigSection[]      = "ml_org";
const char MLOrgConfig::kDefaultLogfile[]          = "C:\\ml_org.log";
const char MLOrgConfig::kDefaultQuery[]            = "type = \"0\"";
const char MLOrgConfig::kDefaultRuleCondition[]    = "type = \"0\"";
const char MLOrgConfig::kDefaultRuleFormatString[] = "<Artist>\\<Album>\\<Track2> - <Title>";
const char MLOrgConfig::kInvalidFileCharacters[]   = "\\/.:*?<>|\"";

///////////////////////////////////////////////////////////////////////////////

MLOrgConfig::MLOrgConfig(void) {
    memset(m_path, 0, sizeof(m_path));
    memset(m_query, 0, sizeof(m_query));
    memset(&m_rules, 0, sizeof(m_rules));
    memset(m_logfile, 0, sizeof(m_logfile));
    memset(m_watchdirs, 0, sizeof(m_watchdirs));
    m_logLevel = Logger::kNumLogLevels;

    m_replaceSpaces = false;
    m_removeEmptyDirs = false;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgConfig::LoadWatchDirs(void) {
    char* confDir = (char*) SendMessage(plugin.hwndWinampParent, WM_WA_IPC, 0, IPC_GETINIDIRECTORY);
    char confFile[1024] = { 0 };
    sprintf(confFile, "%s\\Plugins\\gen_ml.ini", confDir);

    sprintf(tmp, "ML config file: %s", confFile);
    logger.Message("MLOrgConfig::LoadWatchDirs()", tmp);
    GetPrivateProfileString("gen_ml_config", "scandirlist", "",
                            m_watchdirs, sizeof(m_watchdirs), confFile);

    sprintf(tmp, "watch dirs: %s", m_watchdirs);
    logger.Message("MLOrgConfig::LoadWatchDirs()", tmp);
}

///////////////////////////////////////////////////////////////////////////////

bool MLOrgConfig::LoadConfiguration(const char* iniFile) {
    sprintf(tmp, "ini file: %s", iniFile);
    logger.Info("MLOrgConfig::LoadConfiguration()", tmp);

    LoadWatchDirs();

    int tmpInt;
    char key[1024] = { 0 };
    char value[4096] = { 0 };

    // read logging information
    tmpInt = GetPrivateProfileInt(kMLOrgConfigSection, "loglevel",
                                  Logger::kNumLogLevels, iniFile);

    m_logLevel = (Logger::eLogLevels) tmpInt;
    GetPrivateProfileString(kMLOrgConfigSection, "logfile", kDefaultLogfile,
                            m_logfile, sizeof(m_logfile), iniFile);

    sprintf(tmp, "logfile %s, %d", m_logfile, m_logLevel);
    logger.Message("MLOrgConfig::LoadConfiguration()", tmp);

    // load rule set
    mlOrgConfig.m_rules.ClearAllRules();
    int nRules = GetPrivateProfileInt(kMLOrgConfigSection, "num_rules", -1, iniFile);

    sprintf(tmp, "rules: %d", nRules);
    logger.Message("MLOrgConfig::LoadConfiguration()", tmp);

    if (nRules < 0) {
        m_rules.AddRule(new MLOrgRule(kDefaultRuleCondition, kDefaultRuleFormatString));
    } else {
        for (int idx = 0; idx < nRules; idx++) {
            sprintf(key, "rule_%d", idx);
            GetPrivateProfileString(kMLOrgConfigSection, key, "",
                                    value, sizeof(value), iniFile);
            m_rules.AddRule(new MLOrgRule(value));
        }
    }

    // load the organization query
    GetPrivateProfileString(kMLOrgConfigSection, "query", kDefaultQuery,
                            m_query, sizeof(m_query), iniFile);
    sprintf(tmp, "org query: %s", m_query);
    logger.Message("MLOrgConfig::LoadConfiguration()", tmp);

    // load the default path
    GetPrivateProfileString(kMLOrgConfigSection, "path", "",
                            m_path, sizeof(m_path), iniFile);
    sprintf(tmp, "default path: %s", m_path);
    logger.Message("MLOrgConfig::LoadConfiguration()", tmp);

    // load the directory removal preference
    GetPrivateProfileString(kMLOrgConfigSection, "remove_empty_dirs", "",
                            value, sizeof(value), iniFile);
    m_removeEmptyDirs = (strcmp(value, "1") == 0);

    // load the default system character
    GetPrivateProfileString(kMLOrgConfigSection, "replace_spaces", "",
                            value, sizeof(value), iniFile);
    m_replaceSpaces = (strcmp(value, "1") == 0);

    GetPrivateProfileString(kMLOrgConfigSection, "syschar", "",
                            value, sizeof(value), iniFile);
    m_syschar = value[0];

    sprintf(tmp, "system character: %c, replace spaces: %d",
            m_syschar, m_replaceSpaces);
    logger.Message("MLOrgConfig::LoadConfiguration()", tmp);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool MLOrgConfig::SaveConfiguration(const char* iniFile) {
    logger.Info("MLOrgConfig::SaveConfiguration()", "saving...");

    int tmpInt;
    char key[1024] = { 0 };
    char value[4096] = { 0 };

    // save organization query & default path
    WritePrivateProfileString(kMLOrgConfigSection, "path", m_path, iniFile);
    WritePrivateProfileString(kMLOrgConfigSection, "query", m_query, iniFile);

    // save our default system char
    sprintf(value, "%c", m_syschar);
    WritePrivateProfileString(kMLOrgConfigSection, "syschar", value, iniFile);
    WritePrivateProfileString(kMLOrgConfigSection, "replace_spaces",
                              (m_replaceSpaces) ? "1" : "0", iniFile);

    // save the directory removal preference
    WritePrivateProfileString(kMLOrgConfigSection, "remove_empty_dirs",
                              (m_removeEmptyDirs) ? "1" : "0", iniFile);

    // save rule set
    tmpInt = m_rules.GetNumberOfRules();
    sprintf(value, "%d", tmpInt);
    WritePrivateProfileString(kMLOrgConfigSection, "num_rules", value, iniFile);
    for (int idx = 0; idx < tmpInt; idx++) {
        sprintf(key, "rule_%d", idx);
        sprintf(value, "%s", m_rules.GetRuleAt(idx)->AsString());
        WritePrivateProfileString(kMLOrgConfigSection, key, value, iniFile);
    }

    // save log info
    sprintf(value, "%d", m_logLevel);
    WritePrivateProfileString(kMLOrgConfigSection, "loglevel", value, iniFile);
    WritePrivateProfileString(kMLOrgConfigSection, "logfile", m_logfile, iniFile);

    return true;
}
