/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __MLORG_RULE_H__
#define __MLORG_RULE_H__

class MLOrgRule {
	///////////////////////////////////////////////////////////////////////////
	// C O N S T A N T S   &   S T R U C T S
    public:
        enum { kMaxRuleLength = 1024 };
		static const char kRuleDelimiter[];

	///////////////////////////////////////////////////////////////////////////
	// M E T H O D S
    public:
		MLOrgRule(void);
		MLOrgRule(const char* pRule);
		MLOrgRule(const char* pCondition, const char* pFormatString);

		inline char* Condition(void)    { return m_condition; }
		inline char* FormatString(void) { return m_formatString; }

		const char* AsString(void);
		void FromString(const char* rule);
		void FromString(const char* pCondition, const char* pFormatString);

        bool MatchesRecord(itemRecord* pRecord);

	///////////////////////////////////////////////////////////////////////////
	// P R I V A T E
    private:
		char* m_condition;
		char* m_formatString;
		char m_rule[kMaxRuleLength];
		char m_ruleString[kMaxRuleLength];
};

#endif
