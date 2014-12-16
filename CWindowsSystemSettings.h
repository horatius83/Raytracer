#ifndef CWINDOWSSYSTEMSETTINGS_H
#define CWINDOWSSYSTEMSETTINGS_H

#include "Windows.h"

namespace NUtility
{
	enum PROCESSOR_TYPE{UNKNOWN,AMD64,WOW64,IA64,X86};

	class CWindowsSystemSettings
	{
	public:
		CWindowsSystemSettings(){GetSystemInfo(&m_stSystemInfo);};
		PROCESSOR_TYPE eGetProcessor();
		unsigned int uiGetNumProcessors();
		bool bHas3dNow();
		bool bHasMmx();
		unsigned int uiSseLevel();
	private:
		SYSTEM_INFO	m_stSystemInfo;
	};

	inline PROCESSOR_TYPE CWindowsSystemSettings::eGetProcessor()
	{
		switch(m_stSystemInfo.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_AMD64:			return AMD64;	break;
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:	return WOW64;	break;
		case PROCESSOR_ARCHITECTURE_IA64:			return IA64;	break;
		case PROCESSOR_ARCHITECTURE_INTEL:			return X86;		break;
		default:									return UNKNOWN;	break;
		}
	}

	inline unsigned int CWindowsSystemSettings::uiGetNumProcessors()
	{
		return (unsigned int)m_stSystemInfo.dwNumberOfProcessors;
	}

	inline bool CWindowsSystemSettings::bHas3dNow()
	{
		return IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE) ? true : false;
	}

	inline bool CWindowsSystemSettings::bHasMmx()
	{
		return IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE) ? true : false;
	}

	inline unsigned int CWindowsSystemSettings::uiSseLevel()
	{
		unsigned int uiCpuInfo;
		unsigned int uiSse3Info;

		_asm
		{
			mov eax,01h
			cpuid
			mov uiCpuInfo, edx
			mov uiSse3Info,ecx
		}

		if((uiCpuInfo>>25)&0x1)
		{
			if((uiCpuInfo>>26)&0x1)
			{
				if(uiSse3Info&0x1)
					return 3;
				else
					return 2;
			}
			else
				return 1;
		}
		else
			return 0;
		/*
		if(IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
		{
			if(IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE))
			{
#ifndef PF_SSE3_INSTRUCTIONS_AVAILABLE
#define PF_SSE3_INSTRUCTIONS_AVAILABLE 13
#endif
				if(IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE))
					return 3;
				else
					return 2;
			}
			else
				return 1;
		}
		else
			return 0;*/
	}
};

#endif