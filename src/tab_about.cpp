/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
**
** The tab control is from the ml_ipod plugin: http://mlipod.sf.net/
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include "ml_org.h"

///////////////////////////////////////////////////////////////////////////////

extern void config_tab_init(HWND tab);

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK about_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
            config_tab_init(hwndDlg);
            logger.Info("about_tabproc()", "WM_INITDIALOG");
            SetDlgItemText(hwndDlg, IDC_VERSION, PLUGIN_DESC);
            break;

		case WM_COMMAND:
            logger.Message("about_dialog()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
				case IDC_URL:
					break;
            }
			break;

        default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("about_tabproc()", tmp);
            break;
    }

    return 0;
}
