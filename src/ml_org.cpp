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

int init(void);
void quit(void);
void config(HWND parent);

///////////////////////////////////////////////////////////////////////////////

char tmp[4096];
char error[1024];
char* iniFile;

WNDPROC mlWndproc;
prefsDlgRec prefsPage;

MLOrg ml_org;
Logger logger;
MLOrgConfig mlOrgConfig;

///////////////////////////////////////////////////////////////////////////////

extern INT_PTR CALLBACK organize_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK config_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern LRESULT CALLBACK winproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern int msgproc(int message_type, int param1, int param2, int param3);

///////////////////////////////////////////////////////////////////////////////

winampMediaLibraryPlugin plugin = {
    MLHDR_VER,
    PLUGIN_DESC,
    init, quit, msgproc
};

///////////////////////////////////////////////////////////////////////////////

int init(void) {
	iniFile = (char*) SendMessage(plugin.hwndWinampParent, WM_WA_IPC, 0, IPC_GETINIFILE);
	if (! mlOrgConfig.LoadConfiguration(iniFile)) {
        logger.Error("ml_org::init()", "could not load configuration");
		return 1;
	}

	logger.SetLogLevel(mlOrgConfig.m_logLevel);
	logger.SetLogfile(mlOrgConfig.m_logfile);

	// Grab ahold of the window function pointer so we can do our own thang
	// when someone presses the "Organize Media Library" menu item
	mlWndproc = (WNDPROC) SetWindowLongPtr(plugin.hwndLibraryParent,
		                                   GWLP_WNDPROC, (LONG) winproc);
	if (mlWndproc == 0) {
		int error = GetLastError();
		sprintf(tmp, "could not get window func ptr (%d)", error);
        logger.Error("ml_org::init()", tmp);
		return error;
	}

	logger.Info("ml_org::init()", "plugin initialized");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void quit(void) { 
	if (! mlOrgConfig.SaveConfiguration(iniFile)) {
        logger.Warn("ml_org::quit()", "could not save configuration");
	}
	logger.Info("ml_org::quit()", "plugin unloaded");
} 

///////////////////////////////////////////////////////////////////////////////

void config(HWND parent) {
	logger.Info("ml_org::config()", "open");
    DialogBox(plugin.hDllInstance, MAKEINTRESOURCE(IDD_CONFIG), parent, config_dlgproc);
	logger.Info("ml_org::config()", "close");
}

///////////////////////////////////////////////////////////////////////////////

void organizeML(void) {
	int startTime = GetTickCount();
	sprintf(tmp, "started at: %d", startTime);
	logger.Info("ml_org::organizeML()", tmp);

	int result = DialogBox(plugin.hDllInstance, MAKEINTRESOURCE(IDD_PROGRESS),
                           plugin.hwndLibraryParent, organize_dlgproc);

	int stopTime = GetTickCount();
	//int totalMS = (stopTime - startTime);
	//sprintf(tmp, "complete (%d), total time: %dms, avg: %.4fms / record",
	//	    result, totalMS, ((float) totalMS / (float) ml_org_records->Size));
	//logger.Info("ml_org::organizeML()", tmp);

    // make sure to re-sync the ML DB after all this...
    SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, 0, ML_IPC_DB_SYNCDB);
}

///////////////////////////////////////////////////////////////////////////////

extern "C" {
__declspec( dllexport ) winampMediaLibraryPlugin* winampGetMediaLibraryPlugin(void) {
	return &plugin;
}
};
