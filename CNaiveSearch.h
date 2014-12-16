#ifndef CNAIVESEARCH_H
#define CNAIVESEARCH_H

//#include "CPolygon.h"
#include "CVector.h"
#include "TAlignedArray.h"
#include "CPacketVector.h"
#include "CVector2D.h"
#include "CPolygonList.h"

namespace NRayTracer
{
	using NMath::SVector128;
	using NMath::CPacketVector;
	//using NMath::CPolygon;
	using NMath::CVector;
	using NMath::CVector2D;
	using NUtility::TAlignedArray;
/*
	struct SPolyPointers
	{
		SPolyPointers() : m_pPolyA(0),m_pPolyB(0),m_pPolyC(0),m_pPolyD(0) {};
		CPolygon* m_pPolyA;
		CPolygon* m_pPolyB;
		CPolygon* m_pPolyC;
		CPolygon* m_pPolyD;
	};
*/	
	struct SPolyIndices
	{
		SPolyIndices() : m_uiPolyA(0),m_uiPolyB(0),m_uiPolyC(0),m_uiPolyD(0) {};
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

	class CNaiveSearch
	{
	public:
		void GetPrimaryIntersection(SVector128& oResults,SPolyIndices& oPolyIndices,
			CPacketVector& oOrigin,CPacketVector& oDirection);
		void GetShadowIntersection(SVector128& oResults,CPacketVector& oOrigin,CPacketVector& oDirection);
		//bool bCanSetPolygonList(TAlignedArray<unsigned int>& oIndices,TAlignedArray<CVector>& oVertices,
		//	TAlignedArray<CVector>& oNormals,TAlignedArray<CVector2D>& oTexCoords);

		CPolygonList	m_oPolyList;
	};

	inline void CNaiveSearch::GetPrimaryIntersection(SVector128& oResults,
		SPolyIndices& oPolyIndices,
		CPacketVector& oOrigin,
		CPacketVector& oDirection)
	{
		NMath::SVector128 oTempResults;
		oResults.m_sseData = _mm_set_ps1(FLT_MAX);

		for(unsigned int ui=0;ui<m_oPolyList.uiGetSize();++ui)
		{
			m_oPolyList.GetIntersection(oTempResults,ui,oOrigin,oDirection);
			//oTempResults.m_sseData = _mm_cmplt_ps(oTempResults.m_sseData,oResults.m_sseData);
			
			/*switch(_mm_movemask_ps(_mm_cmplt_ps(oTempResults.m_sseData,oResults.m_sseData)))
			{
			case 0x1:
				{
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0x2:
				{
					oPolyIndices.m_uiPolyB = ui;
				}break;
			case 0x3:
				{
					oPolyIndices.m_uiPolyA = ui;
					oPolyIndices.m_uiPolyB = ui;
				}break;
			case 0x4:
				{
					oPolyIndices.m_uiPolyC = ui;
				}break;
			case 0x5:
				{
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0x6:
				{
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyB = ui;
				}break;
			case 0x7:
				{
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyB = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0x8:
				{
					oPolyIndices.m_uiPolyD = ui;
				}break;
			case 0x9:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0xA:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyB = ui;
				}break;
			case 0xB:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyB = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0xC:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyC = ui;
				}break;
			case 0xD:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			case 0xE:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyB = ui;
				}break;
			case 0xF:
				{
					oPolyIndices.m_uiPolyD = ui;
					oPolyIndices.m_uiPolyC = ui;
					oPolyIndices.m_uiPolyB = ui;
					oPolyIndices.m_uiPolyA = ui;
				}break;
			default: break;
			}*/
			for(unsigned int ui=0;ui<4;++ui)
				oPolyIndices.m_uiPoly[ui] = oTempResults.m_fData[ui] < oResults.m_fData[ui] ? ui : oPolyIndices.m_uiPoly[ui];
			oResults.m_sseData = _mm_min_ps(oTempResults.m_sseData,oResults.m_sseData);
			

			/*
			if(oResults.m_fData[0]>0.0f)
			{
				if(oResults.m_fData[0]<oTempResults.m_fData[0])
				{
					oTempResults.m_fData[0]=oResults.m_fData[0];
					oPolyIndices.m_uiPolyA = ui;
				}
			}

			if(oResults.m_fData[1]>0.0f)
			{
				if(oResults.m_fData[1]<oTempResults.m_fData[1])
				{
					oTempResults.m_fData[1]=oResults.m_fData[1];
					oPolyIndices.m_uiPolyB = ui;
				}
			}
			
			if(oResults.m_fData[2]>0.0f)
			{
				if(oResults.m_fData[2]<oTempResults.m_fData[2])
				{
					oTempResults.m_fData[2]=oResults.m_fData[2];
					oPolyIndices.m_uiPolyC = ui;
				}
			}

			if(oResults.m_fData[3]>0.0f)
			{
				if(oResults.m_fData[3]<oTempResults.m_fData[3])
				{
					oTempResults.m_fData[3]=oResults.m_fData[3];
					oPolyIndices.m_uiPolyD = ui;
				}
			}*/
		}
		/*
		for(unsigned int ui=0;ui<4;++ui)
		{
			if(oTempResults.m_fData[ui]<NMath::g_fMaxRay)
				oResults.m_fData[ui]=oTempResults.m_fData[ui];
		}*/
	}

	inline void CNaiveSearch::GetShadowIntersection(NMath::SVector128& oResults,NMath::CPacketVector& oOrigin,
		NMath::CPacketVector& oDirection)
	{
		
	}
/*
	inline bool CNaiveSearch::bCanSetPolygonList(
		NUtility::TAlignedArray<unsigned int>& oIndices,
		NUtility::TAlignedArray<NMath::CVector>& oVertices,
		NUtility::TAlignedArray<NMath::CVector>& oNormals,
		NUtility::TAlignedArray<NMath::CVector2D>& oTexCoords)
	{
		return m_oPolyList.bCanInitialize(oIndices,oVertices,oNormals,oTexCoords);
	}*/
}

#endif