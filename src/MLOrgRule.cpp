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

const char MLOrgRule::kRuleDelimiter[] = "|";

///////////////////////////////////////////////////////////////////////////////

MLOrgRule::MLOrgRule(void) {
	m_condition = NULL;
	m_formatString = NULL;
	memset(m_rule, 0, sizeof(m_rule));
	memset(m_ruleString, 0, sizeof(m_ruleString));
}

///////////////////////////////////////////////////////////////////////////////

MLOrgRule::MLOrgRule(const char* rule) {
    FromString(rule);
}

///////////////////////////////////////////////////////////////////////////////

MLOrgRule::MLOrgRule(const char* pCondition, const char* pFormatString) {
    FromString(pCondition, pFormatString);
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRule::FromString(const char* pCondition, const char* pFormatString) {
    char rule[MLOrgRule::kMaxRuleLength] = { 0 };
    sprintf(rule, "%s%s%s", pCondition, kRuleDelimiter, pFormatString);
	logger.Message("MLOrgRule::FromString(pCond, pFormat)", rule);
    FromString(rule);
}

///////////////////////////////////////////////////////////////////////////////

void MLOrgRule::FromString(const char* pRule) {
	logger.Message("MLOrgRule::FromString(pRule)", pRule);
	strcpy(m_rule, pRule);
	m_condition = strtok(m_rule, kRuleDelimiter);
	m_formatString = strtok(NULL, kRuleDelimiter);
}

///////////////////////////////////////////////////////////////////////////////

const char* MLOrgRule::AsString(void) {
    sprintf(m_ruleString, "%s%s%s", m_condition, kRuleDelimiter, m_formatString);
    return m_ruleString;
}

///////////////////////////////////////////////////////////////////////////////

bool MLOrgRule::MatchesRecord(itemRecord* pRecord) {
    char qString[MLOrgRule::kMaxRuleLength] = { 0 };
    sprintf(qString, "%s AND (filename == \"%s\")", m_condition, pRecord->filename);

	// set up the ipc query
	mlQueryStruct query;
	query.max_results = 1;
	query.query = qString;
    query.results.Size = 0;
    query.results.Alloc = 0;
    query.results.Items = NULL;

    // send the message & check the results
    SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &query, ML_IPC_DB_RUNQUERY);
    bool match = (query.results.Size > 0);
    SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &query, ML_IPC_DB_FREEQUERYRESULTS);
    return match;
}