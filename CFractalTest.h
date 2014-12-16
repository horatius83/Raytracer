#ifndef CFRACTALTEST_H
#define CFRACTALTEST_H

#include "CPixel.h"

namespace NRayTracer
{
	class CFractalTest
	{
	public:
		CFractalTest() : m_uiWidth(0),m_uiHeight(0) {};
		bool CanInitialize(unsigned int uiWidth,unsigned int uiHeight);
		void Render(NGraphics::CPixel* pData);
	private:
		unsigned int	m_uiWidth;
		unsigned int	m_uiHeight;
	};

	inline bool CFractalTest::CanInitialize(unsigned int uiWidth,unsigned int uiHeight)
	{
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;
		return true;
	};

	inline void CFractalTest::Render(NGraphics::CPixel *pData)
	{
		//memset(m_pDisplay,0,sizeof(int)*iHeight*iWidth);
		memset(pData,0,sizeof(NGraphics::CPixel)*m_uiHeight*m_uiWidth);

		NGraphics::CPixel oColor(1.0f,1.0f,1.0f);
		//Draw Sierpinski Triangle
		float fx=float(rand())/float(RAND_MAX);
		float fy=float(rand())/float(RAND_MAX);
		float fOneThird=1.0f/3.0f;
		float fTwoThird=2.0f/3.0f;
		float fWidth=float(m_uiWidth);
		float fHeight=float(m_uiHeight);

		switch(rand()%7)
		{
		case 0:		oColor.Set(1.0f,0.0f,0.0f);	break;
		case 1:		oColor.Set(0.0f,1.0f,0.0f);	break;
		case 2:		oColor.Set(0.0f,0.0f,1.0f);	break;
		case 3:		oColor.Set(1.0f,1.0f,0.0f);	break;
		case 4:		oColor.Set(1.0f,0.0f,1.0f);	break;
		case 5:		oColor.Set(0.0f,1.0f,1.0f);	break;
		default:	oColor.Set(1.0f,1.0f,1.0f);	break;
		}

		for(int ui=0;ui<10000;++ui)
		{
			float fRand = float(rand())/float(RAND_MAX);
			
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
			int ix=int(fx*fWidth);
			int iy=int(fy*fHeight);
			//m_pDisplay[((iHeight-iy-1)*iWidth)+ix]=0xFFFFFFFF;
			pData[(m_uiWidth*iy)+ix]=oColor;
		}
	}
}

#endif