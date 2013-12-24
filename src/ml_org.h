/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __ML_ORG_H__
#define __ML_ORG_H__

// plugin information
#define VERSION     "0.1"
#define PLUGIN_DESC "Media Library Organizer,v0.1"

#include "resource.h"

// My Libraries
#include "Logger.h"
#include "utils_str.h"
#include "utils_file.h"

// Windows Stuff
#include <windows.h>
#include <commctrl.h>

// Standard Libraries
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Winamp SDK
#include "ml.h"
#include "wa_ipc.h"

// ml_org Classes
#include "MLOrg.h"
#include "MLOrgRule.h"
#include "MLOrgRuleSet.h"
#include "MLOrgConfig.h"


// make all the global ml_org variables accessible
extern char tmp[4096];
extern char error[1024];
extern char* iniFile;

extern MLOrg ml_org;
extern Logger logger;
extern MLOrgConfig mlOrgConfig;

extern WNDPROC mlWndproc;
extern prefsDlgRec prefsPage;
extern winampMediaLibraryPlugin plugin;

#endif
