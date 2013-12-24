/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
**
** This code is modified from the ml_ipod plugin: http://mlipod.sf.net/
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include "ml_org.h"

///////////////////////////////////////////////////////////////////////////////

extern void organizeML(void);
extern void config(HWND parent);

///////////////////////////////////////////////////////////////////////////////

#define kMLMenuFirstItem 40028  // "Remove missing files from Media Library..."
#define kMLOrgMenuID     43800
#define kMLOrgMenuText   "Organize Media Library"

///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK winproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	sprintf(tmp, "%d (%d, %d)", uMsg, wParam, lParam);
	//logger.Debug("winproc()", message);

	// Make sure we are dealing with the correct menu.
	// We do this by checking the first menu item in the menu.
	// If it is a well-known one we go further. Otherwise we skip.
	if (uMsg == WM_INITMENUPOPUP) {
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID;
		GetMenuItemInfo((HMENU) wParam, 0, TRUE, &mii);
		if (mii.wID == kMLMenuFirstItem) {
			sprintf(tmp, "adding menu item: %s", kMLOrgMenuText);
		    logger.Debug("ml_org::winproc()", tmp);

    	    // Delete the menu item first (the call is ignored if it does not exist)
	        DeleteMenu((HMENU) wParam, kMLOrgMenuID, MF_BYCOMMAND);

        	// Create a menu item in the Library button popupmenu
	        mii.cbSize = sizeof(MENUITEMINFO);
        	mii.fMask = MIIM_TYPE|MIIM_ID;
	        mii.wID = kMLOrgMenuID;
	        mii.fType = MFT_STRING;
	        mii.dwTypeData = kMLOrgMenuText;

	        // Make the actual call to insert it
	        InsertMenuItem((HMENU) wParam, 0, FALSE, &mii);
		}
	}

	// Catch click on menu item here
	if ((uMsg == WM_COMMAND)
	 && (HIWORD(wParam) == 0)
	 && (LOWORD(wParam) == kMLOrgMenuID)) {
		organizeML();
	}

	// Call the default window proc to handle the message as well.
	return CallWindowProc(mlWndproc, hwnd, uMsg, wParam, lParam);
} 

///////////////////////////////////////////////////////////////////////////////

int msgproc(int message_type, int param1, int param2, int param3) {
	sprintf(tmp, "%d (%d, %d, %d)", message_type, param1, param2, param3);
	logger.Debug("msgproc()", tmp);

	switch (message_type) {
		case ML_MSG_CONFIG:
            logger.Message("ml_org::config()", "open");
			config((HWND) param1);
			return IDOK;
	}

	return 0;
}