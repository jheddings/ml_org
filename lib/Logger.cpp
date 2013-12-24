/* Written by Jason Heddings - http://www.heddway.com/
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software. 
**
** License: http://creativecommons.org/licenses/by-nc-sa/2.0/
** Copyright (c) 2005, Heddway
*/

#include <string.h>
#include "Logger.h"

// TODO :jah: this class should be made thread safe, just because it should

///////////////////////////////////////////////////////////////////////////////

Logger::Logger(void) {
    m_fp = NULL;
	m_level = Logger::kNumLogLevels;
	memset(m_logfile, 0, sizeof(m_logfile));
}

///////////////////////////////////////////////////////////////////////////////

Logger::~Logger(void) {
	Flush();
	Close();
}

///////////////////////////////////////////////////////////////////////////////

void Logger::SetLogfile(const char* logfile) {
#ifdef _DEBUG
	if (m_fp) {
        fclose(m_fp);
	}
	m_fp = fopen(logfile, "w+");
	strcpy(m_logfile, logfile);
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Logger::GetLogContents(char* pBuffer, int buffLen) {
#ifdef _DEBUG
	long size = GetLogLength();
    long start = (buffLen >= size) ? 0 : (size - buffLen);
    long toRead = (buffLen > size) ? size : buffLen;
	long pos = ftell(m_fp);
	fseek(m_fp, start, SEEK_SET);
    fread(pBuffer, 1, toRead, m_fp);
	fseek(m_fp, pos, SEEK_SET);
#else
	memset(pBuffer, NULL, buffLen);
#endif
}

///////////////////////////////////////////////////////////////////////////////

long Logger::GetLogLength(void) {
#ifdef _DEBUG
    long pos = ftell(m_fp);
	fseek(m_fp, 0, SEEK_END);
    long size = ftell(m_fp);
	fseek(m_fp, pos, SEEK_SET);
	return size;
#else
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Logger::ClearLog(void) {
#ifdef _DEBUG
	fclose(m_fp);
	m_fp = fopen(m_logfile, "w+");
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Logger::Flush(void) {
#ifdef _DEBUG
    fflush(m_fp);
#endif
}

///////////////////////////////////////////////////////////////////////////////

 void Logger::Close(void) {
#ifdef _DEBUG
    if (m_fp) {
        fclose(m_fp);
        m_fp = NULL;
        memset(m_logfile, 0, sizeof(m_logfile));
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Logger::Print(const char* type, const char* group,
				   const char* message, eLogLevels level) {
#ifdef _DEBUG
	if (m_fp && (m_level >= level)) {
      fprintf(m_fp, "[%s] %s -- %s\r\n", type, group, message);
	  fflush(m_fp);
	}
#endif
}