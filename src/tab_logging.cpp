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

bool m_dirtyLogfile;

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK logging_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int tmpInt;

    switch (uMsg) {
        case WM_INITDIALOG:
            config_tab_init(hwndDlg);
			m_dirtyLogfile = false;

            tmpInt = logger.GetLogLength();
			sprintf(tmp, "WM_INITDIALOG: log size: %d bytes", tmpInt);
            logger.Info("logging_tabproc()", tmp);

			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_ADDSTRING, 0, (LPARAM) "Error");
			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_ADDSTRING, 0, (LPARAM) "Warn");
			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_ADDSTRING, 0, (LPARAM) "Info");
			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_ADDSTRING, 0, (LPARAM) "Message");
			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_ADDSTRING, 0, (LPARAM) "Debug");
            SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_SETCURSEL, mlOrgConfig.m_logLevel, 0);

			SetDlgItemText(hwndDlg, IDC_LOGFILE, mlOrgConfig.m_logfile);

            // load the log contents into our temp buffer
			memset(tmp, 0, sizeof(tmp));
            logger.GetLogContents(tmp, sizeof(tmp));
			SetDlgItemText(hwndDlg, IDC_LOG_CONTENTS, tmp);

			SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, WM_SETFOCUS, 0, 0);
			break;

		case WM_COMMAND:
            logger.Message("logging_tabproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
				case IDC_URL:
					break;

				case IDC_LOG_LEVEL:
					tmpInt = SendDlgItemMessage(hwndDlg, IDC_LOG_LEVEL, CB_GETCURSEL, 0, 0);
					mlOrgConfig.m_logLevel = (Logger::eLogLevels) tmpInt;
                    logger.SetLogLevel(mlOrgConfig.m_logLevel);
					break;

				case IDC_LOGFILE:
					switch (HIWORD(wParam)) {
                        case EN_CHANGE:
                            GetDlgItemText(hwndDlg, IDC_LOGFILE,
                                           mlOrgConfig.m_logfile,
                                           sizeof(mlOrgConfig.m_logfile) - 1);
				            m_dirtyLogfile = true;
							break;
					}
					break;

				case IDC_CLEAR_LOG:
					logger.ClearLog();
					SetDlgItemText(hwndDlg, IDC_LOG_CONTENTS, "");
					break;

				case IDC_OPEN_LOG:
                    MessageBox(hwndDlg, "NOT IMPLEMENTED YET", "Open Logfile", MB_OK|MB_ICONEXCLAMATION);
					break;
            }
			break;

		default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("logging_tabproc()", tmp);
            break;
    }

    return 0;
}
