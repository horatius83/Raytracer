#ifndef TSIERPINSKI_H
#define TSIERPINSKI_H

#include "CRandom.h"

namespace NGraphics
{
	//This class generates a sierpinski triangle on a given 2d buffer
	template <class ColorType>
	class TSierpinski
	{
	public:
		inline bool bGenerate(ColorType* pArray,const ColorType& oColor,unsigned int uiWidth,unsigned int uiLength,
			unsigned int uiIterations)
		{

			if(pArray && uiWidth && uiLength)
			{
				float fx=m_oRandom.fGetNextRandom();
				float fy=m_oRandom.fGetNextRandom();
				float fOneThird=1.0f/3.0f;
				float fTwoThird=fOneThird*2.0f;
				float fWidth = float(uiWidth);
				float fHeight = float(uiLength/uiWidth);
				unsigned int uix,uiy;

				for(int ui=0;ui<uiIterations;++ui)
				{
			
					float fRand = m_oRandom.fGetNextRandom();
			
			 		if(fRand<=fTwoThird)
					{
						if(fRand<=fOneThird)
						{
							fx*=0.5f;
							fy*=0.5f;
						}
						else
						{
							fx=0.5f*(0.5f+fx);
							fy=0.5f*(1.0f+fy);
						}
					}
					else
					{
						fx=0.5f*(1.0f+fx);
						fy*=0.5f;
					}
					uix=unsigned int(fx*fWidth);
					uiy=unsigned int(fy*fHeight);

					pArray[(uiWidth*uiy)+uix]=oColor;
				}
				return true;
			}
			else
				return false;
		}
		NMath::CRandom& oGetRandom(){return m_oRandom;};
	private:
		NMath::CRandom	m_oRandom;
	};
}

#endif