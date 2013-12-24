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

INT_PTR CALLBACK advanced_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
            config_tab_init(hwndDlg);
            logger.Info("advanced_tabproc()", "WM_INITDIALOG");
            SetDlgItemText(hwndDlg, IDC_PATH,  mlOrgConfig.m_path);
            SetDlgItemText(hwndDlg, IDC_SYS_CHAR, &(mlOrgConfig.m_syschar));
            if (mlOrgConfig.m_replaceSpaces) {
                CheckDlgButton(hwndDlg, IDC_REPLACE_SPACES, BST_CHECKED);
            }

            if (mlOrgConfig.m_removeEmptyDirs) {
                CheckDlgButton(hwndDlg, IDC_REMOVE_EMPTY_DIRS, BST_CHECKED);
            }

            GetDlgItemText(hwndDlg, IDC_SYSCHAR_TEXT, tmp, sizeof(tmp) - 1);
            strreplace(tmp, "SYS_CHARS", MLOrgConfig::kInvalidFileCharacters);
            SetDlgItemText(hwndDlg, IDC_SYSCHAR_TEXT, tmp);
            break;

		case WM_COMMAND:
            logger.Message("advanced_tabproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
		
                //////  CONTROL CHANGES  ////////////////////////////////////

                case IDC_PATH:
					if (HIWORD(wParam) == EN_CHANGE) {
                        GetDlgItemText(hwndDlg, IDC_PATH,
                                       mlOrgConfig.m_path,
                                       sizeof(mlOrgConfig.m_path) - 1);
					}
                    break;

                case IDC_SYS_CHAR:
					if (HIWORD(wParam) == EN_CHANGE) {
                        GetDlgItemText(hwndDlg, IDC_SYS_CHAR, tmp, sizeof(tmp) - 1);
                        if (strlen(tmp) > 1) {
                            tmp[1] = NULL;
                            SetDlgItemText(hwndDlg, IDC_SYS_CHAR, tmp);
                        } else {
                            mlOrgConfig.m_syschar = tmp[0];
                        }
					}
                    break;

                case IDC_REPLACE_SPACES:
                    mlOrgConfig.m_replaceSpaces = 
                        IsDlgButtonChecked(hwndDlg, IDC_REPLACE_SPACES);
                    break;

                case IDC_REMOVE_EMPTY_DIRS:
                    mlOrgConfig.m_removeEmptyDirs = 
                        IsDlgButtonChecked(hwndDlg, IDC_REMOVE_EMPTY_DIRS);
                    break;
            }
            break;

        default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("advanced_tabproc()", tmp);
            break;
    }

    return 0;
}
