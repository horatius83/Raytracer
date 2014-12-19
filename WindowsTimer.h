#ifndef WINDOWSTIMER_H
#define WINDOWSTIMER_H

#include <windows.h>

namespace Utility 
{
	class WindowsTimer
	{
	public:
		WindowsTimer() : m_i64Frequency(0),m_i64Start(0),m_i64End(0) {};
		bool			Initialize();
		void			Start();
		void			Stop();
		unsigned long	GetMilliSeconds();
		unsigned long	GetSeconds();
		inline long		GetSize(){return sizeof(WindowsTimer);};
	private:
		__int64 m_i64Frequency;		//frequency of timer
		__int64 m_i64Start;			//start time
		__int64 m_i64End;			//end time
	};

	inline bool WindowsTimer::Initialize()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
		m_i64Frequency/=1000;	//Convert to milliseconds
		return (m_i64Frequency) ? true : false;
	}

	inline void WindowsTimer::Start(){
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64Start);
	}

	inline void WindowsTimer::Stop(){
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64End);
	}

	inline unsigned long WindowsTimer::GetMilliSeconds(){
		return (unsigned long)((m_i64End-m_i64Start)/m_i64Frequency);
	}

	inline unsigned long WindowsTimer::GetSeconds(){
		return (unsigned long)((m_i64End-m_i64Start)*(1000/m_i64Frequency));
	}
}
#endif