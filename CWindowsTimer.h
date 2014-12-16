#ifndef CWINDOWSTIMER_H
#define CWINDOWSTIMER_H

#include <windows.h>

namespace NUtility 
{
	class CWindowsTimer
	{
	public:
		CWindowsTimer() : m_i64Frequency(0),m_i64Start(0),m_i64End(0) {};
		bool			bInitialize();
		void			Start();
		void			Stop();
		unsigned long	ulGetMilliSeconds();
		unsigned long	ulGetSeconds();
		inline long		lGetSize(){return sizeof(CWindowsTimer);};
	private:
		__int64 m_i64Frequency;		//frequency of timer
		__int64 m_i64Start;			//start time
		__int64 m_i64End;			//end time
	};

	inline bool CWindowsTimer::bInitialize()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
		m_i64Frequency/=1000;	//Convert to milliseconds
		return (m_i64Frequency) ? true : false;
	}

	inline void CWindowsTimer::Start(){
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64Start);
	}

	inline void CWindowsTimer::Stop(){
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64End);
	}

	inline unsigned long CWindowsTimer::ulGetMilliSeconds(){
		return (unsigned long)((m_i64End-m_i64Start)/m_i64Frequency);
	}

	inline unsigned long CWindowsTimer::ulGetSeconds(){
		return (unsigned long)((m_i64End-m_i64Start)*(1000/m_i64Frequency));
	}
}
/*
class CTimer
{
public:
	CTimer()
	{
		//get the performance timer frequency
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
		//check to see that this computer supports high precision timers
		m_bValid = (m_i64Frequency) ? true : false;
		//set start and end to zero initially
		m_i64Start = m_i64End = 0;
		//calculate the number of ticks per millisecond
		m_i64TicksPerMilli = m_i64Frequency / 1000; 	
	};

	inline bool	bStart()
	//-------------------------------------------------------------------------
	//start timer
	//-------------------------------------------------------------------------
	{
		//get the start time
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64Start); 
		//if this computer doesn't support high precision timers then the 
		//previous function returned zero anyway soo...
		return m_bValid;
	};

	inline void	Stop()
	//-------------------------------------------------------------------------
	//stop timer
	//-------------------------------------------------------------------------
	{
		//get stop time
		QueryPerformanceCounter((LARGE_INTEGER*)&m_i64End);	
	};

	//get the time
	inline __int64 i64GetTime(){return (m_i64End - m_i64Start)/m_i64TicksPerMilli;};
	//get ticks per millisecond
	inline __int64 i64GetTicksPerMilli(){return m_i64TicksPerMilli;};

	inline __int64 i64Stop()
	//-------------------------------------------------------------------------
	//stop timer, return current time (__int64)
	//-------------------------------------------------------------------------
	{
		Stop();
		return i64GetTime();
	};

	inline int iStop()
	//-------------------------------------------------------------------------
	//stop timer and return current time (integer)
	//-------------------------------------------------------------------------
	{
		Stop();
		return int(i64GetTime());
	};

	inline __int64 i64Reset()
	//-------------------------------------------------------------------------
	//Stop timer, get the time, then start clock again
	//-------------------------------------------------------------------------
	{
		Stop();	//stop timer
		__int64 i64Temp = i64GetTime();	//store current time
		bStart();	//start clock
		return i64Temp;	//return the time
	};

	inline int iReset()
	//-------------------------------------------------------------------------
	//same as above, only returns an integer
	//-------------------------------------------------------------------------
	{
		Stop();
		int iTemp = int(i64GetTime());
		bStart();
		return iTemp;
	};

	//get time in seconds
	inline double dGetSeconds(){return double(i64GetTime())/1000.0;};

	inline double dStop()
	//-------------------------------------------------------------------------
	//stops the timer and returns time in seconds
	//-------------------------------------------------------------------------
	{
		Stop();
		return dGetSeconds();
	};
private:
	__int64 m_i64Frequency;		//frequency of timer
	__int64 m_i64Start;			//start time
	__int64 m_i64End;			//end time
	__int64 m_i64TicksPerMilli;	//ticks per millisecond
	bool	m_bValid;			//valid
};
*/
#endif