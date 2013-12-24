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

enum {
	kMLOrgStatusStart = 0,
	kMLOrgStatusOrganizing,
    kMLOrgStatusRemoveEmptyDirs,
	kMLOrgStatusCancelled,
	kMLOrgStatusComplete,
	kMLOrgStatusERROR,

	kProgDlgTimer = 0x6a6168
};

///////////////////////////////////////////////////////////////////////////////

itemRecordList* m_records;
int m_orgStatus;
int m_recordIdx;
bool m_cancel;

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK organize_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	itemRecord* record;
	switch (uMsg) {
        case WM_INITDIALOG:
            logger.Info("organize_dlgproc()", "WM_INITDIALOG");
			m_orgStatus = kMLOrgStatusStart;
			SetTimer(hwndDlg, kProgDlgTimer, 25, NULL);
			break;

		case WM_COMMAND:
			logger.Message("organize_dlgproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
		        case IDCANCEL:
                    logger.Info("organize_dlgproc()", "CANCELLED BY USER");
					m_cancel = true;
					break;
            }
			break;

	    case WM_TIMER:
			if (wParam == kProgDlgTimer) {
				switch (m_orgStatus) {
		            case kMLOrgStatusStart:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusStart");
             			m_cancel = false;
						m_recordIdx = 0;

						SetDlgItemText(hwndDlg, IDC_STATUS, "Loading records...");
                        m_records = ml_org.GetItemRecordList();
                        SendDlgItemMessage(hwndDlg, IDC_PROGBAR, PBM_SETRANGE,
                                           0, MAKELPARAM(0, m_records->Size));

						sprintf(tmp, "organizing %d records", m_records->Size);
                        logger.Info("organize_dlgproc()", tmp);
						m_orgStatus = kMLOrgStatusOrganizing;
						break;

					case kMLOrgStatusOrganizing:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusOrganizing");
                        if (m_recordIdx == m_records->Size) {
                            if (mlOrgConfig.m_removeEmptyDirs) {
    							m_orgStatus = kMLOrgStatusRemoveEmptyDirs;
                            } else {
    							m_orgStatus = kMLOrgStatusComplete;
                            }
                        } else {
			    			record = &(m_records->Items[m_recordIdx]);
		    				sprintf(tmp, "%d / %d records", m_recordIdx, m_records->Size);
	    					SetDlgItemText(hwndDlg, IDC_STATUS, tmp);
    						SetDlgItemText(hwndDlg, IDC_TITLE, record->title);
                            if (ml_org.Organize(record) == MLOrgConfig::kOrganizeERROR) {
				    			m_orgStatus = kMLOrgStatusERROR;
			    			} else if (m_cancel) {
		    					m_orgStatus = kMLOrgStatusCancelled;
                            } else {
                                m_recordIdx++;
                            }
    						SendDlgItemMessage(hwndDlg, IDC_PROGBAR, PBM_SETPOS,
                                               m_recordIdx, 0);
                        }
						break;

					case kMLOrgStatusRemoveEmptyDirs:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusRemoveEmptyDirs");
						SetDlgItemText(hwndDlg, IDC_STATUS, "Removing empty directories...");
                        ml_org.RemoveEmptyDirectories();
						m_orgStatus = kMLOrgStatusComplete;
						break;

					case kMLOrgStatusCancelled:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusCancelled");
						KillTimer(hwndDlg, kProgDlgTimer);
						EndDialog(hwndDlg, IDCANCEL);
                        MessageBox(hwndDlg, "Operation cancelled by user",
                                   "Organizing Files", MB_OK | MB_ICONEXCLAMATION);
                        freeRecordList(m_records);
						break;

					case kMLOrgStatusERROR:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusERROR");
						KillTimer(hwndDlg, kProgDlgTimer);
						EndDialog(hwndDlg, IDCANCEL);
						record = &(m_records->Items[m_recordIdx]);
                        sprintf(tmp, "There was an error while processing the following file:\n%s\n\nERROR: %s",
                                record->filename, error);
                        MessageBox(hwndDlg, tmp, "Organizing Files", MB_OK | MB_ICONERROR);
                        freeRecordList(m_records);
						break;

					case kMLOrgStatusComplete:
                        logger.Debug("organize_dlgproc()", "WM_TIMER: kMLOrgStatusComplete");
						KillTimer(hwndDlg, kProgDlgTimer);
						EndDialog(hwndDlg, IDOK);
                        sprintf(tmp, "Operation completed succesfully.\n\n%d files processed",
                                m_records->Size);
                        MessageBox(hwndDlg, tmp, "Organizing Files", MB_OK | MB_ICONINFORMATION);
                        freeRecordList(m_records);
						break;

		            default:
                        logger.Error("organize_dlgproc()", "WM_TIMER: <UNKNOWN STATUS>");
						break;
				}
			}
			break;

		default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("organize_dlgproc()", tmp);
            break;
	}

    return 0;
}