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

int m_editingRule;
char m_ruleditCond[MLOrgRule::kMaxRuleLength];
char m_ruleditFrmt[MLOrgRule::kMaxRuleLength];

///////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK ruledit_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int tmpInt;
    MLOrgRule* pRule;
	ml_editquery meq = {hwndDlg, m_ruleditCond};

	switch (uMsg) {
        case WM_INITDIALOG:
            m_editingRule = lParam;
            if (m_editingRule >= 0) {
                sprintf(tmp, "WM_INITDIALOG: edit rule %d", m_editingRule);
                logger.Info("ruledit_dlgproc()", tmp);
                pRule = mlOrgConfig.m_rules.GetRuleAt(m_editingRule);
                SetDlgItemText(hwndDlg, IDC_RULE_FORMAT,    pRule->FormatString());
                SetDlgItemText(hwndDlg, IDC_RULE_CONDITION, pRule->Condition());
            } else {
                logger.Info("ruledit_dlgproc()", "WM_INITDIALOG: new rule");
                SetDlgItemText(hwndDlg, IDC_RULE_FORMAT,    MLOrgConfig::kDefaultRuleFormatString);
                SetDlgItemText(hwndDlg, IDC_RULE_CONDITION, MLOrgConfig::kDefaultRuleCondition);
            }
			break;

		case WM_COMMAND:
            logger.Message("ruledit_dlgproc()", "WM_COMMAND");
            switch (LOWORD(wParam)) {
		        case IDC_QUERY_EDIT:
                    GetDlgItemText(hwndDlg, IDC_RULE_CONDITION,
                                   m_ruleditCond, sizeof(m_ruleditCond) - 1);
		    		tmpInt = (int) SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, 
			    						       (LPARAM) &meq, ML_IPC_EDITQUERY);
				    if (tmpInt) {
                        SetDlgItemText(hwndDlg, IDC_RULE_CONDITION, meq.query);
				    }
                    break;

				case IDC_RULE_FORMAT:
                    switch (HIWORD(wParam)) {
                        case EN_CHANGE:
                            GetDlgItemText(hwndDlg, IDC_RULE_FORMAT,
                                           m_ruleditFrmt, sizeof(m_ruleditFrmt) - 1);
                            ml_org.GetExampleFilename(m_ruleditFrmt);
                            SetDlgItemText(hwndDlg, IDC_RULE_PREVIEW, m_ruleditFrmt);
                            break;
                    }
					break;

				case IDC_FORMAT_HELP:
                    MessageBox(hwndDlg, "NOT IMPLEMENTED YET", "Format String Help", MB_OK|MB_ICONEXCLAMATION);
					break;

                case IDOK:
                    GetDlgItemText(hwndDlg, IDC_RULE_CONDITION,
                                   m_ruleditCond, sizeof(m_ruleditCond) - 1);
                    GetDlgItemText(hwndDlg, IDC_RULE_FORMAT,
                                   m_ruleditFrmt, sizeof(m_ruleditFrmt) - 1);
                    if (m_editingRule >= 0) {
                        pRule = mlOrgConfig.m_rules.GetRuleAt(m_editingRule);
                        pRule->FromString(m_ruleditCond, m_ruleditFrmt);
                    } else {
                        pRule = new MLOrgRule(m_ruleditCond, m_ruleditFrmt);
                        mlOrgConfig.m_rules.AddRule(pRule);
                    }
                case IDCANCEL:
                    EndDialog(hwndDlg, 0);
                    break;
			}
			break;
    }

    return 0;
}