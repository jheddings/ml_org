/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __MLORG_RULE_SET_H__
#define __MLORG_RULE_SET_H__

#include "MLOrgRule.h"

class itemListRecord;

class MLOrgRuleSet {
    ///////////////////////////////////////////////////////////////////////////
    // C O N S T A N T S   &   S T R U C T S
    public:
        typedef struct _MLOrgRuleNode {
            MLOrgRule* pRule;
            _MLOrgRuleNode* pPrev;
            _MLOrgRuleNode* pNext;

            _MLOrgRuleNode(void) {
                pRule = NULL;
                pPrev = NULL;
                pNext = NULL;
            }
        } MLOrgRuleNode;

    ///////////////////////////////////////////////////////////////////////////
    // M E T H O D S
    public:
        MLOrgRuleSet(void);
        ~MLOrgRuleSet(void);

        MLOrgRule* MatchRecord(itemRecord* pRecord);

        void ClearAllRules(void);
        int GetNumberOfRules(void);
        MLOrgRule* GetRuleAt(int idx);
        void AddRule(MLOrgRule* pRule);
        void DeleteRuleAt(int idx);

        void MoveUp(int idx);
        void MoveDown(int idx);

        void DumpListLog(void);

    ///////////////////////////////////////////////////////////////////////////
    // P R I V A T E
    private:
        MLOrgRuleNode* m_pHead;

        ////  fast list operations - the faster the better  ////

        inline MLOrgRuleNode* Head(void) { return m_pHead; }

        inline MLOrgRuleNode* Tail(void) {
            MLOrgRuleNode* pCur = m_pHead;
            while ((pCur != NULL) && (pCur->pNext != NULL)) {
                pCur = pCur->pNext;
            }
            return pCur;
        }

        inline MLOrgRuleNode* Cursor(int idx) {
            MLOrgRuleNode* pCur = m_pHead;
            for (int count = 0; (count < idx) && (pCur != NULL); count++) {
                pCur = pCur->pNext;
            }
            return pCur;
        }

        inline void Append(MLOrgRuleNode* pCur, MLOrgRuleNode* pNode) {
            pNode->pNext = pCur->pNext;
            pNode->pPrev = pCur;
            pCur->pNext  = pNode;
            if (pNode->pNext) {
                pNode->pNext->pPrev = pNode;
            }
        }

        inline void Insert(MLOrgRuleNode* pCur, MLOrgRuleNode* pNode) {
            pNode->pPrev = pCur->pPrev;
            pNode->pNext = pCur;
            pCur->pPrev  = pNode;
            if (pNode->pPrev) {
                pNode->pPrev->pNext = pNode;
            } else {
                m_pHead = pNode;
            }
        }

        inline void BumpNext(MLOrgRuleNode* pNode) {
            MLOrgRuleNode* pNext = pNode->pNext;
            if (pNext) {
                Unlink(pNode);
                Append(pNext, pNode);
            }
        }

        inline void BumpPrevious(MLOrgRuleNode* pNode) {
            MLOrgRuleNode* pPrev = pNode->pPrev;
            if (pPrev) {
                Unlink(pNode);
                Insert(pPrev, pNode);
            }
        }

        inline void Delete(MLOrgRuleNode* pNode) {
            Unlink(pNode);
            delete pNode->pRule;
            delete pNode;
        }

        inline void Unlink(MLOrgRuleNode* pNode) {
            // set up the previous node, or update the head pointer
            if (pNode->pPrev) {
                pNode->pPrev->pNext = pNode->pNext;
            } else {
                m_pHead = pNode->pNext;
            }

            // set up the next node
            if (pNode->pNext) {
                pNode->pNext->pPrev = pNode->pPrev;
            }

            pNode->pPrev = NULL;
            pNode->pNext = NULL;
        }

        inline int CountForward(MLOrgRuleNode* pStart) {
            int count = 0;
            for ( ; pStart != NULL; pStart = pStart->pNext) {
                count++;
            }
            return count;
        }

        inline int CountBackward(MLOrgRuleNode* pStart) {
            int count = 0;
            for ( ; pStart != NULL; pStart = pStart->pPrev) {
                count++;
            }
            return count;
        }
};

#endif
