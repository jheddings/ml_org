/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __MLORG_CONFIG_H__
#define __MLORG_CONFIG_H__

#include "Logger.h"
#include "MLOrgRuleSet.h"

class MLOrgConfig {
    ///////////////////////////////////////////////////////////////////////////
    // C O N S T A N T S   &   S T R U C T S
    public:
        typedef enum {
            kOrganizeOK = 0,
            kOrganizeNoMatch,
            kOrganizeNoBasePath,
            kOrganizeERROR
        } eOrganizeResult;

    ///////////////////////////////////////////////////////////////////////////
    // M E M B E R S
    public:
        MLOrgRuleSet m_rules;

        char m_syschar;
        bool m_replaceSpaces;
        bool m_removeEmptyDirs;

        char m_query[4096];
        char m_path[MAX_PATH];
        char m_watchdirs[10 * MAX_PATH];

        // some stuff used for debugging
        char m_logfile[MAX_PATH];
        Logger::eLogLevels m_logLevel;

        static const char kMLOrgConfigSection[];
        static const char kDefaultQuery[];
        static const char kDefaultLogfile[];
        static const char kDefaultRuleCondition[];
        static const char kDefaultRuleFormatString[];
        static const char kInvalidFileCharacters[];

    ///////////////////////////////////////////////////////////////////////////
    // M E T H O D S
    public:
        MLOrgConfig(void);

        bool LoadConfiguration(const char* iniFile);
        bool SaveConfiguration(const char* iniFile);

    ///////////////////////////////////////////////////////////////////////////
    // P R I V A T E
    private:
        void LoadWatchDirs(void);
};

#endif
