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

MLOrgRuleSet::MLOrgRuleSet(void) {
    logger.Debug("MLOrgRuleSet::MLOrgRuleSet()", "new rule set");
    m_pHead = NULL;
}

///////////////////////////////////////////////////////////////////////////////

MLOrgRuleSet::~MLOrgRuleSet(void) {
    ClearAllRules();
}

///////////////////////////////////////////////////////////////////////////////

MLOrgRule* MLOrgRuleSet::MatchRecord(itemRecord* pRecord) {
    MLOrgRule* pRule;
    int nRules = GetNumberOfRules();
    for (int idx = 0; idx < nRules; idx++) {
        pRule = GetRuleAt(idx);
        if (pRule->MatchesRecord(pRecord)) {
            return pRule;
        }
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::ClearAllRules(void) {
    while (m_pHead) {
        Delete(m_pHead);
    }
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::AddRule(MLOrgRule* pRule) {
    logger.Message("MLOrgRuleSet::AddRule(pRule)", pRule->AsString());

    if (pRule != NULL) {
        MLOrgRuleNode* pNode = new MLOrgRuleNode();
        pNode->pRule = pRule;
        pNode->pNext = NULL;
        pNode->pPrev = NULL;

        // find the end of the list and add this node
        if (m_pHead == NULL) {
            m_pHead = pNode;
        } else {
            Append(Tail(), pNode);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::DeleteRuleAt(int idx) {
    sprintf(tmp, "%d", idx);
    logger.Message("MLOrgRuleSet::DeleteRuleAt(idx)", tmp);
    MLOrgRuleNode* pCur = Cursor(idx);
    if (pCur) {
        Delete(pCur);
    }
}

///////////////////////////////////////////////////////////////////////////////

MLOrgRule* MLOrgRuleSet::GetRuleAt(int idx) {
    sprintf(tmp, "%d", idx);
    logger.Debug("MLOrgRuleSet::GetRuleAt(idx)", tmp);
    MLOrgRuleNode* pCur = Cursor(idx);
    return (pCur ? pCur->pRule : NULL);
}

///////////////////////////////////////////////////////////////////////////////

int MLOrgRuleSet::GetNumberOfRules(void) {
    return CountForward(m_pHead);
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::MoveUp(int idx) {
    BumpPrevious(Cursor(idx));
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::MoveDown(int idx) {
    BumpNext(Cursor(idx));
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRuleSet::DumpListLog(void) {
    sprintf(tmp, "BEGIN LIST DUMP: %d items", CountForward(m_pHead));
    logger.Info("MLOrgRuleSet::DumpListLog()", tmp);

    MLOrgRuleNode* pNode = Head();
    while (pNode) {
        sprintf(tmp, "NODE: %s [0x%x <- 0x%x -> 0x%x]",
                pNode->pRule->AsString(),
                pNode->pPrev, pNode, pNode->pNext);
        logger.Info("MLOrgRuleSet::DumpListLog()", tmp);
    
        pNode = pNode->pNext;
    }
}