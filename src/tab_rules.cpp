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
extern INT_PTR CALLBACK ruledit_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////

void checkRuleButtons(HWND hwnd);
void updateRulesList(HWND hwnd, int selectedIdx = -1);
void addItem(HWND hwndList, MLOrgRule* pRule, int idx);
void addColumn(HWND hwnd, char* pText, int col, int width);

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK rules_tabproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd;
	int tmpInt;
	LVITEM* pItem;
    MLOrgRule* pRule;
    itemRecordList* pRecords;
    ml_editquery meq = {hwndDlg, mlOrgConfig.m_query};

    switch (uMsg) {
        case WM_INITDIALOG:
            config_tab_init(hwndDlg);
            logger.Info("rules_tabproc()", "WM_INITDIALOG");
            SetDlgItemText(hwndDlg, IDC_QUERY, mlOrgConfig.m_query);

            // load the rules into the list
            hwnd = GetDlgItem(hwndDlg, IDC_RULES);
			ListView_SetExtendedListViewStyle(hwnd, LVS_EX_FULLROWSELECT);
            addColumn(hwnd, "Condition", 0, 190);
            addColumn(hwnd, "Format",    1, 200);
			updateRulesList(hwnd);
            checkRuleButtons(hwndDlg);
            break;

        case WM_NOTIFY:
            logger.Debug("rules_tabproc()", "WM_NOTIFY");
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
            logger.Message("rules_tabproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {

                //////  CONTROL CHANGES  ////////////////////////////////////

                case IDC_QUERY:
					if (HIWORD(wParam) == EN_CHANGE) {
                        GetDlgItemText(hwndDlg, IDC_QUERY,
                                       mlOrgConfig.m_query,
                                       sizeof(mlOrgConfig.m_query) - 1);
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
            }
            break;

        default:
			sprintf(tmp, "<UNHANDLED MESSAGE> 0x%x", uMsg);
			logger.Debug("rules_tabproc()", tmp);
            break;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void checkRuleButtons(HWND hwnd) {
    bool enable;
    HWND hwndRules    = GetDlgItem(hwnd, IDC_RULES);
    int itemCount     = ListView_GetItemCount(hwndRules);
    int selectedCount = ListView_GetSelectedCount(hwndRules);
    int selectedItem  = ListView_GetNextItem(hwndRules, -1, LVNI_SELECTED);

    sprintf(tmp, "selectedCount:%d, itemCount:%d, selectedItem:%d",
            selectedCount, itemCount, selectedItem);
    logger.Debug("checkRuleButtons()", tmp);

    enable = (selectedCount > 0);
    EnableWindow(GetDlgItem(hwnd, IDC_RULE_EDIT), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_RULE_DELETE), enable);

    enable = (selectedItem > 0);
    EnableWindow(GetDlgItem(hwnd, IDC_RULE_UP), enable);

    enable = ((selectedCount > 0) && (selectedItem < itemCount - 1));
    EnableWindow(GetDlgItem(hwnd, IDC_RULE_DOWN), enable);
}

///////////////////////////////////////////////////////////////////////////////

void addColumn(HWND hwndList, char* pText, int col, int width) {
    sprintf(tmp, "addColumn(%s, %d, %d)", pText, col, width);
    logger.Message("dlg_config", tmp);

    LVCOLUMN lvc = { 0 };
	lvc.mask     = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx       = width;
    lvc.pszText  = pText;
    ListView_InsertColumn(hwndList, col, &lvc);
}

///////////////////////////////////////////////////////////////////////////////

void addItem(HWND hwndList, MLOrgRule* pRule, int idx) {
    sprintf(tmp, "addItem(%s, %s)", pRule->Condition(), pRule->FormatString());
    logger.Message("dlg_config", tmp);

    LVITEM item  = { 0 };
    item.iItem   = idx;
    item.mask    = LVIF_TEXT | LVIF_PARAM;
	item.pszText = LPSTR_TEXTCALLBACK;
    item.lParam  = (LPARAM) pRule;
	ListView_InsertItem(hwndList, &item);
}

///////////////////////////////////////////////////////////////////////////////

void updateRulesList(HWND hwndList, int selectedIdx) {
    sprintf(tmp, "updateRulesList(%d)", selectedIdx);
    logger.Message("dlg_config", tmp);

    ListView_DeleteAllItems(hwndList);
    int nRules = mlOrgConfig.m_rules.GetNumberOfRules();
	for (int idx = 0; idx < nRules; idx++) {
		addItem(hwndList, mlOrgConfig.m_rules.GetRuleAt(idx), idx);
	}
    ListView_SetItemState(hwndList, selectedIdx, LVIS_SELECTED, LVIS_SELECTED);
}