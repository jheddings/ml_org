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

HWND m_hwndDlg,m_hwndTab,m_hwndTabDisplay;

///////////////////////////////////////////////////////////////////////////////

extern INT_PTR CALLBACK about_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK rules_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK logging_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern INT_PTR CALLBACK advanced_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////

struct pref_tab {
  char* title;
  int res_id;
  DLGPROC dlg_proc;
} tabs[] = {
  {"Organization Rules", IDD_CONFIG_RULES, rules_tabproc},
  {"Advanced Settings", IDD_CONFIG_ADVANCED, advanced_tabproc},
#ifdef _DEBUG
  {"Logging", IDD_CONFIG_LOGGING, logging_tabproc},
#endif
  {"About", IDD_CONFIG_ABOUT, about_tabproc},
};

///////////////////////////////////////////////////////////////////////////////

#define ETDT_DISABLE 0x01
#define ETDT_ENABLE 0x02
#define ETDT_ENABLETAB 0x06
#define ETDT_USETABTEXTURE 0x04
typedef HRESULT (WINAPI * ENABLETHEMEDIALOGTEXTURE)(HWND, DWORD);

void config_tab_init(HWND tab)
{
  RECT r;
  GetWindowRect(m_hwndTab,&r);
  TabCtrl_AdjustRect(m_hwndTab,FALSE,&r);
  MapWindowPoints(NULL,m_hwndDlg,(LPPOINT)&r,2);
  SetWindowPos(tab,HWND_TOP,r.left,r.top,r.right-r.left,r.bottom-r.top,SWP_NOACTIVATE);

  ENABLETHEMEDIALOGTEXTURE pfnETDT;
  HINSTANCE hDll;
  if(NULL != (hDll = LoadLibrary(TEXT("uxtheme.dll")))) {
    pfnETDT = (ENABLETHEMEDIALOGTEXTURE)GetProcAddress(hDll,"EnableThemeDialogTexture");
    if(NULL != pfnETDT) {
      pfnETDT(tab,ETDT_ENABLETAB);
    }
    FreeLibrary(hDll);
  }
}

///////////////////////////////////////////////////////////////////////////////

VOID WINAPI OnSelChanged(HWND hwndDlg) 
{
  int sel=TabCtrl_GetCurSel(m_hwndTab);
  if(m_hwndTabDisplay!=NULL)
    DestroyWindow(m_hwndTabDisplay);
  m_hwndTabDisplay=CreateDialog(plugin.hDllInstance,MAKEINTRESOURCE(tabs[sel].res_id),hwndDlg,tabs[sel].dlg_proc);
}

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK config_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LPNMHDR lpn = (LPNMHDR) lParam; 
    switch (uMsg) {
        case WM_INITDIALOG:
            logger.Info("config_dlgproc()", "WM_INITDIALOG");
            TCITEM tie;
            m_hwndDlg = hwndDlg;
            m_hwndTab = GetDlgItem(hwndDlg, IDC_CONFIG_TABS);
            tie.mask = TCIF_TEXT;
            for (int i = 0; i < sizeof(tabs) / sizeof(pref_tab); i++) {
                tie.pszText = tabs[i].title;
                TabCtrl_InsertItem(m_hwndTab, i, &tie);
            }

            TabCtrl_SetCurSel(m_hwndTab, 0);
            OnSelChanged(hwndDlg);
            break;

        case WM_NOTIFY:
            if(lpn) if(lpn->code==TCN_SELCHANGE) OnSelChanged(hwndDlg);
            break;

		case WM_COMMAND:
            logger.Message("config_dlgproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
				case IDOK:
                    mlOrgConfig.SaveConfiguration(iniFile);
				case IDCANCEL:
                    mlOrgConfig.LoadConfiguration(iniFile);
                    EndDialog(hwndDlg, 0);
					break;
            }
            break;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
INT_PTR CALLBACK config_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd;
	int tmpInt;
	LVITEM* pItem;
    MLOrgRule* pRule;
    itemRecordList* pRecords;
    ml_editquery meq = {hwndDlg, m_configCond};

	switch (uMsg) {
        case WM_INITDIALOG:
            logger.Info("config_dlgproc()", "WM_INITDIALOG");

            // set the main query
            SetDlgItemText(hwndDlg, IDC_QUERY, mlOrgConfig.m_query);

            // load the rules into the list
            hwnd = GetDlgItem(hwndDlg, IDC_RULES);
			ListView_SetExtendedListViewStyle(hwnd, LVS_EX_FULLROWSELECT);
            addColumn(hwnd, "Condition", 0, 195);
            addColumn(hwnd, "Format",    1, 200);
			updateRulesList(hwnd);
            checkRuleButtons(hwndDlg);

#ifdef _DEBUG
            ShowWindow(GetDlgItem(hwndDlg, IDC_LOGGING), SW_SHOW);
#endif

			break;

        case WM_NOTIFY:
            logger.Debug("config_dlgproc()", "WM_NOTIFY");
            switch (((LPNMHDR) lParam)->code) {

                //////  HANDLE RULE LIST DISPLAY  /////////////////////////////

                case LVN_GETDISPINFO:
                    pItem = &(((NMLVDISPINFO*) lParam)->item);
                    pRule = mlOrgConfig.m_rules.GetRuleAt(pItem->iItem);
                    switch (pItem->iSubItem) {
                        case 0:
                            pItem->pszText = pRule->Condition();
                            break;

                        case 1:
                            pItem->pszText = pRule->FormatString();
                            break;
                    }
                    break;

                case NM_CLICK:
                    checkRuleButtons(hwndDlg);
                    break;
            }
            break;

		case WM_COMMAND:
            logger.Message("config_dlgproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {

				//////  CONTROL CHANGES  ////////////////////////////////////

                case IDC_QUERY:
					switch (HIWORD(wParam)) {
                        case EN_CHANGE:
                            GetDlgItemText(hwndDlg, IDC_QUERY,
                                           mlOrgConfig.m_query,
                                           sizeof(mlOrgConfig.m_query) - 1);
							break;
					}
                    break;

				//////  RULE BUTTONS  ///////////////////////////////////////

				case IDC_RULE_ADD:
                    DialogBoxParam(plugin.hDllInstance,
					               MAKEINTRESOURCE(IDD_RULE_EDIT),
    				               hwndDlg, ruledit_dlgproc, -1);
                    updateRulesList(GetDlgItem(hwndDlg, IDC_RULES));
                    checkRuleButtons(hwndDlg);
					break;

				case IDC_RULE_EDIT:
                    hwnd = GetDlgItem(hwndDlg, IDC_RULES);
                    tmpInt = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                    DialogBoxParam(plugin.hDllInstance,
					               MAKEINTRESOURCE(IDD_RULE_EDIT),
    				               hwndDlg, ruledit_dlgproc, tmpInt);
                    updateRulesList(hwnd, tmpInt);
                    checkRuleButtons(hwndDlg);
					break;

				case IDC_RULE_DELETE:
                    hwnd = GetDlgItem(hwndDlg, IDC_RULES);
                    tmpInt = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                    if (tmpInt >= 0) {
                        mlOrgConfig.m_rules.DeleteRuleAt(tmpInt);
                        updateRulesList(hwnd);
                        checkRuleButtons(hwndDlg);
                    }
					break;

				case IDC_RULE_UP:
                    hwnd = GetDlgItem(hwndDlg, IDC_RULES);
                    tmpInt = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                    if (tmpInt >= 0) {
                        mlOrgConfig.m_rules.MoveUp(tmpInt);
                        updateRulesList(hwnd, tmpInt - 1);
                        checkRuleButtons(hwndDlg);
                    }
					break;

				case IDC_RULE_DOWN:
                    hwnd = GetDlgItem(hwndDlg, IDC_RULES);
                    tmpInt = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                    if (tmpInt >= 0) {
                        mlOrgConfig.m_rules.MoveDown(tmpInt);
                        updateRulesList(hwnd, tmpInt + 1);
                        checkRuleButtons(hwndDlg);
                    }
					break;

				//////  QUERY BUTTONS  ////////////////////////////////////////

		        case IDC_QUERY_EDIT:
                    GetDlgItemText(hwndDlg, IDC_QUERY,
                                   m_configCond, sizeof(m_configCond) - 1);
		    		tmpInt = (int) SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, 
			    						       (LPARAM) &meq, ML_IPC_EDITQUERY);
				    if (tmpInt) {
                        SetDlgItemText(hwndDlg, IDC_QUERY, meq.query);
				    }
                    break;

		        case IDC_QUERY_RESET:
                    SetDlgItemText(hwndDlg, IDC_QUERY, MLOrgConfig::kDefaultQuery);
                    break;

                case IDC_QUERY_TEST:
                    pRecords = ml_org.GetItemRecordList();
                    sprintf(tmp, "Query: %s\n\nThis query matches %d records in your library.",
                            mlOrgConfig.m_query, pRecords->Size);
                    freeRecordList(pRecords);
                    MessageBox(hwndDlg, tmp, "Test Query Results",
                               MB_OK | MB_ICONINFORMATION);
                    break;

				//////  DIALOG BUTTONS  ///////////////////////////////////////

                case IDC_ABOUT:
                    DialogBox(plugin.hDllInstance,
						      MAKEINTRESOURCE(IDD_ABOUTBOX),
						      hwndDlg, about_dlgproc);
					break;

				case IDC_LOGGING:
                    DialogBox(plugin.hDllInstance,
						      MAKEINTRESOURCE(IDD_LOGGING),
						      hwndDlg, logging_dlgproc);
					break;

				case IDOK:
                    mlOrgConfig.SaveConfiguration(iniFile);
				case IDCANCEL:
                    mlOrgConfig.LoadConfiguration(iniFile);
                    EndDialog(hwndDlg, 0);
					break;
            }
			break;

		default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("config_dlgproc_debug()", tmp);
            break;
	}

    return 0;
}
*/