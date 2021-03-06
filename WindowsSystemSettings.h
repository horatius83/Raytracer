#ifndef WINDOWSSYSTEMSETTINGS_H
#define WINDOWSSYSTEMSETTINGS_H

#include "Windows.h"

namespace Utility
{
	enum PROCESSOR_TYPE{ UNKNOWN, AMD64, WOW64, IA64, X86 };

	class WindowsSystemSettings
	{
	public:
		WindowsSystemSettings(){ GetSystemInfo(&m_stSystemInfo); };
		PROCESSOR_TYPE GetProcessor();
		unsigned int GetNumProcessors();
		bool Has3dNow();
		bool HasMmx();
		unsigned int SseLevel();
	private:
		SYSTEM_INFO	m_stSystemInfo;
	};

	inline PROCESSOR_TYPE WindowsSystemSettings::GetProcessor()
	{
		switch (m_stSystemInfo.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_AMD64:			return AMD64;	break;
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:	return WOW64;	break;
		case PROCESSOR_ARCHITECTURE_IA64:			return IA64;	break;
		case PROCESSOR_ARCHITECTURE_INTEL:			return X86;		break;
		default:									return UNKNOWN;	break;
		}
	}

	inline unsigned int WindowsSystemSettings::GetNumProcessors()
	{
		return (unsigned int)m_stSystemInfo.dwNumberOfProcessors;
	}

	inline bool WindowsSystemSettings::Has3dNow()
	{
		return IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE) ? true : false;
	}

	inline bool WindowsSystemSettings::HasMmx()
	{
		return IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE) ? true : false;
	}

	inline unsigned int WindowsSystemSettings::SseLevel()
	{
		unsigned int uiCpuInfo;
		unsigned int uiSse3Info;

		_asm
		{
			mov eax, 01h
				cpuid
				mov uiCpuInfo, edx
				mov uiSse3Info, ecx
		}

		if ((uiCpuInfo >> 25) & 0x1)
		{
			if ((uiCpuInfo >> 26) & 0x1)
			{
				if (uiSse3Info & 0x1)
					return 3;
				else
					return 2;
			}
			else
				return 1;
		}
		else
			return 0;
	}
};

#endif