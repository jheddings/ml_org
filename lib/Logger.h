/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

class Logger {
	///////////////////////////////////////////////////////////////////////////
	// C O N S T A N T S   &   S T R U C T S
    public:
		typedef enum {
			kError = 0,
			kWarn,
			kInfo,
			kMessage,
			kDebug,
			kNumLogLevels  // MUST BE LAST
		} eLogLevels;

	///////////////////////////////////////////////////////////////////////////
	// M E T H O D S
    public:
		Logger(void);
		~Logger(void);

		void SetLogfile(const char* logfile);

		void GetLogContents(char* pBuffer, int buffLen);
		long GetLogLength(void);
		void ClearLog(void);

		inline eLogLevels GetLogLevel(void) { return m_level; }
		inline void SetLogLevel(eLogLevels lvl) { m_level = lvl; }

		inline void Info(const char* group, const char* message)
		    { Print("INFO", group, message, kInfo); }

		inline void Message(const char* group, const char* message)
		    { Print("MESSAGE", group, message, kMessage); }

		inline void Warn(const char* group, const char* message)
		    { Print("WARNING", group, message, kWarn); }

		inline void Error(const char* group, const char* message)
		    { Print("ERROR", group, message, kError); }

		inline void Debug(const char* group, const char* message)
		    { Print("DEBUG", group, message, kDebug); }

		void Flush(void);
		void Close(void);

	///////////////////////////////////////////////////////////////////////////
	// P R I V A T E
    private:
		FILE* m_fp;
        eLogLevels m_level;
		char m_logfile[1024];

		void Print(const char* type, const char* group,
			       const char* message, eLogLevels level);
};

#endif