#ifndef CHTMLERROR_H
#define CHTMLERROR_H

#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <wchar.h>

#ifdef WIN32
#include "windows.h"
#endif

#define EL_ERROR	0
#define EL_WARNING	1
#define EL_DEBUG	2
#define EL_CALL		3
#define EL_EVENT	4

#ifdef _DEBUG
#define EL_LEVEL 4;
#else
#define SUPPRESS_ALLOCATION_EVENTS
#define SUPPRESS_ALIGNMENT_EVENTS
#endif

//#define SUPPRESS_ALLOCATION_EVENTS

#define err(ws_error,i_errorLevel)	NUtility::CHtmlError::ErrorMessage(__FUNCTION__,ws_error,i_errorLevel)
#define alloc(ws_name,l_bytes)		NUtility::CHtmlError::Allocated(__FUNCTION__,ws_name,l_bytes)
#define dealloc(l_bytes)			NUtility::CHtmlError::Deallocated(__FUNCTION__,l_bytes)

namespace NUtility
{
	using std::wofstream;
	using std::wstring;

	class CHtmlError
	{
	public:
		static bool bOpen(const wstring& sFileName,const wstring& sCssFile,bool bEmbed);
		static void ErrorMessage(const char* sFunction,const wstring& sError,int iErrorLevel);
		static void LogEvent(const char* sFunction);
		static void OutputVariable(const wstring& sName,int iVariable);
		static void OutputVariable(const wstring& sName,float fVariable);
		static void OutputVariable(const wstring& sName,unsigned int uiVariable);

		static void ErrorCode(const wstring& sError,int iCode);
		static void Close();
		static void Allocated(const char* szFunction,const wstring& sName,long lBytes);
		static void Deallocated(const char* szFunction,long lBytes);
	private:
		static void OutputSystemSettings();
		static void OutputErrorCode(int iError_Level);
		static void DateOut(tm* pTime);
		static void TimeOut(tm* pTime);

		static wofstream sm_oOutData;	
		static long sm_lBytes;
	};
	
	inline void CHtmlError::Allocated(const char* szFunction,const wstring& sName,long lBytes)
	{
#ifndef SUPPRESS_ALLOCATION_EVENTS
		sm_oOutData<<L"<li class=\"event\">"<<szFunction<<L"(): <b>"<<sName<<L"</b> allocated: <b>"<<lBytes<<L"</b> bytes</li>\n";
#endif
		sm_lBytes+=lBytes;
#ifndef SUPPRESS_ALIGNMENT_EVENTS
		if(lBytes%16)
			err(L"Data not aligned to address boundaries (16 byte)",EL_WARNING);
#endif
	}

	inline void CHtmlError::Deallocated(const char* szFunction,long lBytes)
	{
#ifndef SUPPRESS_ALLOCATION_EVENTS
		sm_oOutData<<L"<li class=\"event\">"<<szFunction<<L"(): deallocated: <b>"<<lBytes<<L"</b> bytes</li>\n";
#endif
		sm_lBytes-=lBytes;
	}

	inline void CHtmlError::DateOut(tm* pTime)
	{
		sm_oOutData<<pTime->tm_mon+1<<'/'<<pTime->tm_mday<<'/'<<pTime->tm_year+1900;	
	}

	inline void CHtmlError::LogEvent(const char* sFunction)
	{
		sm_oOutData<<L"<li class=\"call\">"<<sFunction<<L"()</li>\n";
	}

	inline void CHtmlError::OutputVariable(const wstring& sName,unsigned int uiVariable)
	{
		sm_oOutData<<L"<li class=\"debug\">"<<sName<<L": <b>"<<uiVariable<<L"</b></li>\n";
	}

	inline void CHtmlError::OutputVariable(const wstring& sName,int iVariable)
	{
		sm_oOutData<<L"<li class=\"debug\">"<<sName<<L": <b>"<<iVariable<<L"</b></li>\n";
	}

	inline void CHtmlError::OutputVariable(const wstring& sName,float fVariable)
	{
		sm_oOutData<<L"<li class=\"debug\">"<<sName<<L": <b>"<<fVariable<<L"</b></li>\n";
	}

	inline void CHtmlError::ErrorCode(const std::wstring& sError,int iCode)
	{
		sm_oOutData<<L"<li class=\"error\">"<<sError<<L" : "<<iCode<<L"</li>\n";
	}

	inline void CHtmlError::ErrorMessage(const char* sFunction,const std::wstring &sError,int iErrorLevel)
	{
#ifdef EL_LEVEL
		switch(iErrorLevel)
		{
		case EL_ERROR:
			{
				sm_oOutData<<L"<li class=\"error\">"<<sFunction<<L"() :"<<sError<<L"</li>\n";	
			}break;
		case EL_WARNING:	sm_oOutData<<L"<li class=\"warning\">"<<sFunction<<L"() :"<<sError<<L"</li>\n";	break;
		case EL_DEBUG:		sm_oOutData<<L"<li class=\"debug\">"<<sFunction<<L"() :"<<sError<<L"</li>\n";	break;
		case EL_CALL:		sm_oOutData<<L"<li class=\"call\">"<<sFunction<<L"() :"<<sError<<L"</li>\n";	break;
		case EL_EVENT:		sm_oOutData<<L"<li class=\"event\">"<<sFunction<<L"() :"<<sError<<L"</li>\n";	break;
		}
		sm_oOutData.flush();

#ifdef WIN32
		OutputDebugStringA(sFunction);
		OutputDebugStringA("() :");
		OutputDebugStringW(sError.data());
		OutputDebugStringW(L"\n");
#endif
#endif
	}
}

#endif