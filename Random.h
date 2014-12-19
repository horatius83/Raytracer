//Purpose: Generate Pseudo-Random Numbers (32bit)
//Based on Chapter 11 of "Mathematics for Game Developers" by Christopher Tremblay ISBN: 1-59200-038-X
#ifndef RANDOM_H
#define RANDOM_H

#include <time.h>

namespace Math
{
	//Precompute these values in case floating-point and double functions are called a lot
	const float		FLOAT_UINT_MAX	= float(UINT_MAX);
	const double	DOUBLE_UINT_MAX = double(UINT_MAX);

	/*Generate random numbers using Knuth's proposition 
	(a=1,664,525 and b=1,013,904,223)*/
	class Random
	{
	public:
		//Start off with a random seed
		Random()
		{
			SetRandomSeed();
		};
		//Set the seed to use for random number generation
		inline void SetSeed(unsigned int uiSeed)
		{
			m_uiSeed=uiSeed;
		};
		//Set seed to the system clock
		inline void SetRandomSeed()
		{
			time_t seconds = time(0);
			m_uiSeed = GetRandom(unsigned int(seconds));
		};
		//Generate a random number off of the current seed
		inline unsigned int GetRandom()
		{
			return GetRandom(m_uiSeed);
		}
		//Generate a random number off of this particular seed
		inline unsigned int GetRandom(unsigned int uiSeed)
		{
			return (uiSeed*1664525)+1013904223;
		};
		//Generate a random number between these two values [uiRangeMin,uiRangeMax)
		inline unsigned int GetRandom(unsigned int uiRangeMin,unsigned int uiRangeMax)
		{
			return uiRangeMin+(GetRandom()%uiRangeMax);
		};
		//Generate a new random number and set the current seed equal to it
		inline unsigned int GetNextRandom()
		{
			m_uiSeed = GetRandom(m_uiSeed);
			return m_uiSeed;
		};
		//Generate a new random number, set the seed to that and clamp it to this range
		inline unsigned int GetNextRandom(unsigned int uiRangeMin,unsigned int uiRangeMax)
		{
			return uiRangeMin+(GetNextRandom()%uiRangeMax);
		}
		//Generate a random float between 0 and 1 and don't update the seed
		inline float fGetRandom()
		{
			return float(GetRandom())/FLOAT_UINT_MAX;
		}
		//Generate a random float between 0 and 1
		inline float fGetNextRandom()
		{
			return float(GetNextRandom())/FLOAT_UINT_MAX;
		}
		//Generate a random double between 0 and 1 and don't update the seed
		inline double dGetRandom()
		{
			return double(GetRandom())/DOUBLE_UINT_MAX;
		}
		//Generate a random double between 0 and 1
		inline double dGetNextRandom()
		{
			return double(GetNextRandom())/DOUBLE_UINT_MAX;
		}
	private:
		unsigned int m_uiSeed;
	};
}

#endif