#include "HtmlError.h"

#ifdef WIN32
#include "WindowsSystemSettings.h"
#endif

namespace Utility
{
	wofstream	HtmlError::sm_oOutData;
	long		HtmlError::sm_lBytes=0;

	void HtmlError::OutputSystemSettings()
	{
		std::wstring	sProcessor;
		std::wstring	s3dNow;
		std::wstring	sMmx;
		sm_oOutData<<L"<table>\n";
#ifdef WIN32
		WindowsSystemSettings oSettings;
#endif
		switch(oSettings.eGetProcessor())
		{
		case AMD64:	sProcessor = L"x64";			break;
		case WOW64: sProcessor = L"WOW64";			break;
		case IA64:	sProcessor = L"Intel Itanium";	break;
		case X86:	sProcessor = L"x86";			break;
		default:	sProcessor = L"Unknown";		break;
		}
		
		s3dNow	= oSettings.bHas3dNow() ? L"supported" : L"not supported";
		sMmx	= oSettings.bHasMmx() ? L"supported" : L"not supported";
		sm_oOutData<<L"<table>\n";
		sm_oOutData<<L"<tr>\n\t<th>Processor</th><th>Cores</th><th>3DNow!</th><th>MMX</th>"<<
			L"<th>SSE Version</th></tr>\n";
		sm_oOutData<<L"<tr>\n\t<td>"<<sProcessor<<"</td><td>"<<oSettings.uiGetNumProcessors()<<
			L"</td><td>"<<s3dNow<<L"</td><td>"<<sMmx<<L"</td><td>"<<oSettings.uiSseLevel()<<L"</td>\n</tr>\n";
		sm_oOutData<<L"</table>\n";
	}

	void HtmlError::TimeOut(tm* pTime)
	{
		if(pTime->tm_hour<10 && pTime->tm_hour)
			sm_oOutData<<'0';

		if(pTime->tm_hour%12)
			sm_oOutData<<pTime->tm_hour%12<<':';
		else
			sm_oOutData<<"12:";
		
		if(pTime->tm_min<10)
			sm_oOutData<<'0';
		sm_oOutData<<pTime->tm_min<<":";
		if(pTime->tm_sec<10)
			sm_oOutData<<'0';
		sm_oOutData<<pTime->tm_sec;
		if(pTime->tm_hour > 12)
			sm_oOutData<<"pm";
		else
			sm_oOutData<<"am";
	}

	bool HtmlError::bOpen(const wstring& sFileName,const wstring& sCssFilename,bool bEmbed)
	{
		if(!sm_oOutData.is_open())
		{
			sm_oOutData.clear();
			sm_oOutData.open(sFileName.data());
			
			if(sm_oOutData.good())
			{
				sm_oOutData<<L"<html>\n<head>\n";
				sm_oOutData<<L"<title>"<<sFileName<<L"</title>\n";
				if(bEmbed)
				{
					std::wifstream inDataTemp;
					inDataTemp.open(sCssFilename.data());
					if(inDataTemp.good())
					{
						sm_oOutData<<L"<style type=\"text/css\">\n";
						std::wstring sBuffer;
						while(!inDataTemp.eof())
						{
							inDataTemp>>sBuffer;
							sm_oOutData<<sBuffer;
						}
						sm_oOutData<<L"\n</style>\n";
					}
					inDataTemp.close();
				}
				else
					sm_oOutData<<L"<link rel=\"stylesheet\" title=\"stylesheet\" href=\""<<sCssFilename<<L"\" type=\"text/css\">\n";
				
				sm_oOutData<<L"</head>\n<body>\n";
				OutputSystemSettings();

				time_t stTime;
				tm* pTime=new tm;
				time(&stTime);
				//pTime=localtime(&stTime);
				localtime_s(pTime,&stTime);
				sm_oOutData<<L"<ol>\n<li class=\"call\">Compiled on "<<__DATE__<<L" at "<<__TIME__<<L"</li>\n";
				sm_oOutData<<L"<li class=\"call\">Ran on ";
				DateOut(pTime);
				sm_oOutData<<L" at ";
				TimeOut(pTime);
				sm_oOutData.flush();
				delete pTime;
				return true;
			}
		}
		else
			err(L"HtmlError::Open() : Already open",EL_WARNING);
		return false;
	}

	void HtmlError::Close()
	{

		if(sm_oOutData.good())
		{
			if(sm_lBytes != 0)
				sm_oOutData<<L"<li class=\"error\">Memory Leak : "<<sm_lBytes<<L" Bytes Remain</li>\n";
			
			time_t stTime;
			tm* pTime=new tm;
			time(&stTime);
			//pTime=localtime(&stTime);
			localtime_s(pTime,&stTime);
			sm_oOutData<<L"<li class=\"call\">Ended on ";
			DateOut(pTime);
			sm_oOutData<<L" at ";
			TimeOut(pTime);
			sm_oOutData<<L"</li>\n</ol>\n</body>\n</html>\n";
			sm_oOutData.close();
			delete pTime;
		}
	}

}