#ifndef NAIVESEARCH_H
#define NAIVESEARCH_H

//#include "CPolygon.h"
#include "Vector.h"
#include "AlignedArray.h"
#include "PacketVector.h"
#include "Vector2D.h"
#include "PolygonList.h"

namespace RayTracer
{
	using Math::Vector128;
	using Math::PacketVector;
	using Math::Vector;
	using Math::Vector2D;
	using Utility::AlignedArray;
	
	struct PolyIndices
	{
		PolyIndices() : m_uiPolyA(0),m_uiPolyB(0),m_uiPolyC(0),m_uiPolyD(0) {};
		union
		{
			struct
			{
				unsigned int m_uiPolyA;
				unsigned int m_uiPolyB;
				unsigned int m_uiPolyC;
				unsigned int m_uiPolyD;
			};
			unsigned int m_uiPoly[4];
		};
	};

	class NaiveSearch
	{
	public:
		void GetPrimaryIntersection(Vector128& oResults,PolyIndices& oPolyIndices,
			PacketVector& oOrigin,PacketVector& Direction);
		void GetShadowIntersection(Vector128& oResults,PacketVector& oOrigin,PacketVector& Direction);

		PolygonList	m_oPolyList;
	};

	inline void NaiveSearch::GetPrimaryIntersection(Vector128& oResults,
		PolyIndices& oPolyIndices,
		PacketVector& oOrigin,
		PacketVector& Direction)
	{
		Math::Vector128 oTempResults;
		oResults.m_sseData = _mm_set_ps1(FLT_MAX);

		for(unsigned int ui=0;ui<m_oPolyList.uiGetSize();++ui)
		{
			m_oPolyList.GetIntersection(oTempResults,ui,oOrigin,Direction);
			
			for(unsigned int ui=0;ui<4;++ui)
				oPolyIndices.m_uiPoly[ui] = oTempResults.m_fData[ui] < oResults.m_fData[ui] ? ui : oPolyIndices.m_uiPoly[ui];
			oResults.m_sseData = _mm_min_ps(oTempResults.m_sseData,oResults.m_sseData);
		}
	}

	inline void NaiveSearch::GetShadowIntersection(Math::Vector128& oResults,Math::PacketVector& oOrigin,
		Math::PacketVector& Direction)
	{
		
	}
}

#endif