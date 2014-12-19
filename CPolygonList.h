#ifndef CPOLYGONLIST_H
#define CPOLYGONLIST_H

//#include "TArray.h"
#include <assert.h>
#include "CVector.h"
#include "CVector2D.h"
#include "CPlane.h"
#include "CPacketVector.h"
#include "TAlignedArray.h"

namespace NRayTracer
{
	using NUtility::TAlignedArray;

	class CPolygonList
	{
	public:
		bool bCanInitialize(TAlignedArray<unsigned int>& oIndices,TAlignedArray<NMath::CVector>& oVertices,
			TAlignedArray<NMath::CVector>& oNormals,TAlignedArray<NMath::CVector2D>& oTexCoords);
		void GetIntersection(NMath::SVector128& oResults,unsigned int uiIndex,
			NMath::CPacketVector& oOrigin,NMath::CPacketVector& oDirection);
		float fGetIntersection(unsigned int uiIndex,const NMath::CVector& oOrigin,const NMath::CVector& oDirection);
	
		//The index for the polygon plus the vertex index (can be 0,1, or 2)
		NMath::CVector&		oGetVertex(unsigned int uiPolygonIndex,unsigned int uiVertexIndex);
		NMath::CVector&		oGetNormal(unsigned int uiPolygonIndex,unsigned int uiVertexIndex);
		NMath::CVector2D&	oGetTexCoords(unsigned int uiPolygonIndex,unsigned int uiVertexIndex);
		void				GetPlaneNormal(NMath::CVector& oNormal,unsigned int uiPolygonIndex);

		//The number of polygons, not the number of vertices
		inline unsigned		uiGetSize(){return m_oPlanes.uiGetSize();};
	private:
		bool bCrosses(float fPointU,float fPointV,float fVectorU,float fVectorV);

		enum EPolygonType{NOT_INITIALIZED,XDOMINANT,YDOMINANT,ZDOMINANT};
		//1 dominant axis and plane per polygon
		TAlignedArray<EPolygonType>		m_oAxis;
		TAlignedArray<NMath::CPlane>	m_oPlanes;

		//3 Indices, Vertices, Normals, and TexCoords per polygon
		TAlignedArray<unsigned int>		m_oIndices;
		TAlignedArray<NMath::CVector>	m_oVertices;
		TAlignedArray<NMath::CVector>	m_oNormals;
		TAlignedArray<NMath::CVector2D>	m_oTexCoords;
		
	};

	inline void CPolygonList::GetPlaneNormal(NMath::CVector& oNormal,unsigned int uiPolygonIndex)
	{
		assert(uiPolygonIndex<m_oPlanes.uiGetSize());
		m_oPlanes[uiPolygonIndex].GetNormal(oNormal);
	
	}

	inline NMath::CVector& CPolygonList::oGetVertex(unsigned int uiPolygonIndex,unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex<3);
		return m_oVertices[m_oIndices[uiPolygonIndex]+uiVertexIndex];
	}

	inline NMath::CVector& CPolygonList::oGetNormal(unsigned int uiPolygonIndex,unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex<3);
		return m_oNormals[m_oIndices[uiPolygonIndex]+uiVertexIndex];
	}

	inline NMath::CVector2D& CPolygonList::oGetTexCoords(unsigned int uiPolygonIndex,unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex<3);
		return m_oTexCoords[m_oIndices[uiPolygonIndex]+uiVertexIndex];
	}

	inline void CPolygonList::GetIntersection(NMath::SVector128& oResults,unsigned int uiIndex,
		NMath::CPacketVector& oOrigin,NMath::CPacketVector& oDirection)
	{
		using NMath::SVector128;
		using NMath::CVector;
		using NMath::CPacketVector;
		
		m_oPlanes[uiIndex].GetIntersection(oResults,oOrigin,oDirection);
		//For some reason this is faster than the SSE equivalent
		//if(oResults.m_fData[0]>0.0f && oResults.m_fData[1]>0.0f && oResults.m_fData[2]>0.0f && oResults.m_fData[3]>0.0f)
		//int iMask = _mm_movemask_ps(_mm_cmpeq_ps(oResults.m_sseData,_mm_set_ps1(0.0f)));
		int iMask = _mm_movemask_ps(_mm_cmplt_ps(oResults.m_sseData,_mm_set_ps1(FLT_MAX)));
		if(iMask)
		{
			//Determine intersection
			CPacketVector oIntersection;
			oIntersection.Mul(oResults.m_sseData,oDirection);
			//oIntersection.Sub(oOrigin,oIntersection);
			oIntersection.Add(oOrigin,oIntersection);

			SVector128 oVectorU[3];
			SVector128 oVectorV[3];
			SVector128 oPointU[3];
			SVector128 oPointV[3];
			unsigned int uiVertex = uiIndex*3;

			switch(m_oAxis[uiIndex])
			{
			case XDOMINANT:	//z and y
				{
					//Get vectors from A->B B->C and C->A
					//Vector A->B (1-0)
					oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex]].fGetZ());
					oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetY()-
						m_oVertices[m_oIndices[uiVertex]].fGetY());
					
					//Translate point A according to intersection
					oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetZ()),oIntersection.m_sseZ);
					oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetY()),oIntersection.m_sseY);
					
					//Vector B->C (2-1)
					oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetZ());
					oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetY()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetY());

					//Translate point B according to intersection
					oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetZ()),oIntersection.m_sseZ);
					oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetY()),oIntersection.m_sseY);

					//Vector C->A [0-2]
					oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetZ());
					oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetY()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetY());

					//Translate point C according to intersection
					oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetZ()),oIntersection.m_sseZ);
					oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetY()),oIntersection.m_sseY);
				}break;
			case YDOMINANT:	//x and z
				{
					//Get vectors from A->B B->C and C->A
					//Vector A->B (1-0)
					oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex+0]].fGetZ());
					oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+0]].fGetX());
					
					//Translate point A according to intersection
					oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetZ()),oIntersection.m_sseZ);
					oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetX()),oIntersection.m_sseX);
					
					//Vector B->C (2-1)
					oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetZ());
					oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetX());

					//Translate point B according to intersection
					oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetZ()),oIntersection.m_sseZ);
					oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetX()),oIntersection.m_sseX);

					//Vector C->A [0-2]
					oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetZ()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetZ());
					oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetX());

					//Translate point C according to intersection
					oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetZ()),oIntersection.m_sseZ);
					oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetX()),oIntersection.m_sseX);
				}break;
			case ZDOMINANT: //x and y
			default:
				{
					//Get vectors from A->B B->C and C->A
					//Vector A->B (1-0)
					oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+0]].fGetX());
					oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetY()-
						m_oVertices[m_oIndices[uiVertex+0]].fGetY());
					
					//Translate point A according to intersection
					oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetX()),oIntersection.m_sseX);
					oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetY()),oIntersection.m_sseY);

					//Vector B->C (2-1)
					oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetX());
					oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetY()-
						m_oVertices[m_oIndices[uiVertex+1]].fGetY());

					//Translate point B according to intersection
					oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetX()),oIntersection.m_sseX);
					oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+1]].fGetY()),oIntersection.m_sseY);
					
					//Vector C->A [0-2]
					oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetX()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetX());
					oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex+0]].fGetY()-
						m_oVertices[m_oIndices[uiVertex+2]].fGetY());

					//Translate point C according to intersection
					oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetX()),oIntersection.m_sseX);
					oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex+2]].fGetY()),oIntersection.m_sseY);
				}break;
			}

			//Once you get the crossing algorithm done the entire thing is done
			int iCrosses[4]={0,0,0,0};
			
			for(unsigned int ui=0;ui<3;++ui)
			{
				
				if(bCrosses(oPointU[ui].m_fData[0],oPointV[ui].m_fData[0],
					oVectorU[ui].m_fData[0],oVectorV[ui].m_fData[0]))
					++iCrosses[0];

				if(bCrosses(oPointU[ui].m_fData[1],oPointV[ui].m_fData[1],
					oVectorU[ui].m_fData[1],oVectorV[ui].m_fData[1]))
					++iCrosses[1];

				if(bCrosses(oPointU[ui].m_fData[2],oPointV[ui].m_fData[2],
					oVectorU[ui].m_fData[2],oVectorV[ui].m_fData[2]))
					++iCrosses[2];

				if(bCrosses(oPointU[ui].m_fData[3],oPointV[ui].m_fData[3],
					oVectorU[ui].m_fData[3],oVectorV[ui].m_fData[3]))
					++iCrosses[3];
			}

			for(unsigned int ui=0;ui<4;++ui)
			{
				if(!(iCrosses[0]%2))
					oResults.m_fData[ui]=FLT_MAX;
			}	
		}
		else
		{
			oResults.m_fData[0] = fGetIntersection(uiIndex,CVector(oOrigin.m_fX[0],oOrigin.m_fY[0],oOrigin.m_fZ[0]),
				CVector(oDirection.m_fX[0],oDirection.m_fY[0],oDirection.m_fZ[0]));
			oResults.m_fData[1] = fGetIntersection(uiIndex,CVector(oOrigin.m_fX[1],oOrigin.m_fY[1],oOrigin.m_fZ[1]),
				CVector(oDirection.m_fX[1],oDirection.m_fY[1],oDirection.m_fZ[1]));
			oResults.m_fData[2] = fGetIntersection(uiIndex,CVector(oOrigin.m_fX[2],oOrigin.m_fY[2],oOrigin.m_fZ[2]),
				CVector(oDirection.m_fX[2],oDirection.m_fY[2],oDirection.m_fZ[2]));
			oResults.m_fData[3] = fGetIntersection(uiIndex,CVector(oOrigin.m_fX[3],oOrigin.m_fY[3],oOrigin.m_fZ[3]),
				CVector(oDirection.m_fX[3],oDirection.m_fY[3],oDirection.m_fZ[3]));
		}
	}

	inline float CPolygonList::fGetIntersection(unsigned int uiIndex,const NMath::CVector &oOrigin, const NMath::CVector &oDirection)
	{
		using NMath::CVector;
		using NMath::CVector2D;

		float fDistance = m_oPlanes[uiIndex].fGetIntersection(oOrigin,oDirection);

		if(fDistance<FLT_MAX)
		{
			int iCrosses=0;
			//Get intersection point
			CVector oIntersect;
			oIntersect.Mul(fDistance,oDirection);
			oIntersect.Sub(oOrigin,oIntersect);
			//Get vectors for each set of points (so three vectors for three points), and translate points by 
			//intersection point;
			//A->B intersection (vector = b-a)
			CVector oVec1,oPoint1;
			oVec1.Sub(m_oVertices[m_oIndices[uiIndex+1]],m_oVertices[m_oIndices[uiIndex+0]]);
			oPoint1.Sub(m_oVertices[m_oIndices[uiIndex+0]],oIntersect);
			//B->C intersection (vector = c-b)
			CVector	oVec2,oPoint2;
			oVec2.Sub(m_oVertices[m_oIndices[uiIndex+2]],m_oVertices[m_oIndices[uiIndex+1]]);
			oPoint2.Sub(m_oVertices[m_oIndices[uiIndex+1]],oIntersect);
			//C->A intersection (vector = a-c)
			CVector oVec3,oPoint3;
			oVec3.Sub(m_oVertices[m_oIndices[uiIndex+0]],m_oVertices[m_oIndices[uiIndex+2]]);
			oPoint3.Sub(m_oVertices[m_oIndices[uiIndex+2]],oIntersect);
			
			CVector2D oPoint2D,oVector2D;
			switch(m_oAxis[uiIndex])
			{
			case XDOMINANT:	//z and y coords
				{
					//1rst intersection (A->B)
					//oPoint2D.Set(oPoint1.fGetZ(),oPoint1.fGetY());
					//oVector2D.Set(oVec1.fGetZ(),oVec1.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint1.fGetZ(),oPoint1.fGetY(),oVec1.fGetZ(),oVec1.fGetY()))
						++iCrosses;
					//2nd intersection (B->C)
					//oPoint2D.Set(oPoint2.fGetZ(),oPoint2.fGetY());
					//oVector2D.Set(oVec2.fGetZ(),oVec2.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint2.fGetZ(),oPoint2.fGetY(),oVec2.fGetZ(),oVec2.fGetY()))
						++iCrosses;
					//3rd intersection (C->A)
					//oPoint2D.Set(oPoint3.fGetZ(),oPoint3.fGetY());
					//oVector2D.Set(oVec3.fGetZ(),oVec3.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint3.fGetZ(),oPoint3.fGetY(),oVec3.fGetZ(),oVec3.fGetY()))
						++iCrosses;
				}break;
			case YDOMINANT: //x and z coords
				{
					//1rst intersection (A->B)
					//oPoint2D.Set(oPoint1.fGetX(),oPoint1.fGetZ());
					//oVector2D.Set(oVec1.fGetX(),oVec1.fGetZ());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint1.fGetX(),oPoint1.fGetZ(),oVec1.fGetX(),oVec1.fGetZ()))
						++iCrosses;
					//2nd intersection (B->C)
					//oPoint2D.Set(oPoint2.fGetX(),oPoint2.fGetZ());
					//oVector2D.Set(oVec2.fGetX(),oVec2.fGetZ());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint2.fGetX(),oPoint2.fGetZ(),oVec2.fGetX(),oVec2.fGetZ()))
						++iCrosses;
					//3rd intersection (C->A)
					//oPoint2D.Set(oPoint3.fGetX(),oPoint3.fGetZ());
					//oVector2D.Set(oVec3.fGetX(),oVec3.fGetZ());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint3.fGetX(),oPoint3.fGetZ(),oVec3.fGetX(),oVec3.fGetZ()))
						++iCrosses;
				}break;
			case ZDOMINANT:	//x and y coords
			default:		
				{
					//1rst intersection (A->B)
					//oPoint2D.Set(oPoint1.fGetX(),oPoint1.fGetY());
					//oVector2D.Set(oVec1.fGetX(),oVec1.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint1.fGetX(),oPoint1.fGetY(),oVec1.fGetX(),oVec1.fGetY()))
						++iCrosses;
					//2nd intersection (B->C)
					//oPoint2D.Set(oPoint2.fGetX(),oPoint2.fGetY());
					//oVector2D.Set(oVec2.fGetX(),oVec2.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint2.fGetX(),oPoint2.fGetY(),oVec2.fGetX(),oVec2.fGetY()))
						++iCrosses;
					//3rd intersection (C->A)
					//oPoint2D.Set(oPoint3.fGetX(),oPoint3.fGetY());
					//oVector2D.Set(oVec3.fGetX(),oVec3.fGetY());
					//if(bCrosses(oPoint2D,oVector2D))
					if(bCrosses(oPoint3.fGetX(),oPoint3.fGetY(),oVec3.fGetX(),oVec3.fGetY()))
						++iCrosses;
				}break;
			}

			if(iCrosses%2)
				return fDistance;
		}
		return FLT_MAX;
	}

	inline bool CPolygonList::bCrosses(float fPointU,float fPointV,float fVectorU,float fVectorV)
	{
		//if(oVector.fGetY())
		if(fVectorV)
		{
			float t=-fPointV/fVectorV;
			if(t>=0.0f && t<=1.0f)
			{
				if(fPointU+t*fVectorU>=0.0f)
					return true;
			}
		}
		return false;
	}

	bool CPolygonList::bCanInitialize(TAlignedArray<unsigned int>& oIndices,TAlignedArray<NMath::CVector>& oVertices,
		TAlignedArray<NMath::CVector>& oNormals,TAlignedArray<NMath::CVector2D>& oTexCoords)
	{
		using NMath::CVector;

		if(m_oVertices.uiGetSize()==m_oNormals.uiGetSize() && m_oVertices.uiGetSize()==m_oTexCoords.uiGetSize())
		{
			m_oIndices.Set(oIndices);
			m_oVertices.Set(oVertices);
			m_oNormals.Set(oNormals);
			m_oTexCoords.Set(oTexCoords);

			CVector a,b,planeNormal,avgNormal;
			m_oPlanes.Allocate(m_oIndices.uiGetSize()/3);
			m_oAxis.Allocate(m_oPlanes.uiGetSize());

			for(unsigned int ui=0;ui<m_oPlanes.uiGetSize();++ui)
			{
				//Create two vectors off of the 3 points 
				unsigned int uiIndex=ui*3;
				a.Sub(m_oVertices[m_oIndices[uiIndex]],m_oVertices[m_oIndices[uiIndex+1]]);
				a.Normalize();
				b.Sub(m_oVertices[m_oIndices[uiIndex]],m_oVertices[m_oIndices[uiIndex+2]]);
				b.Normalize();
				//Calculate the dot product to get the plane-normal (although technically this produces 2 normals)
				planeNormal.Cross(a,b);
				//Get the average of all three point-normals
				avgNormal.Add(m_oNormals[m_oIndices[uiIndex]],m_oNormals[m_oIndices[uiIndex+1]]);
				avgNormal.Add(avgNormal,m_oNormals[m_oIndices[uiIndex+2]]);
				avgNormal.Normalize();
				//Determine which side the plane normal is supposed to be on
				if(avgNormal.fDot(planeNormal)<0.0f)
					planeNormal.Mul(-1.0f,planeNormal);
				//Create plane
				planeNormal.Normalize();
				//m_oPlanes[ui].Set(m_oVertices[m_oIndices[uiIndex]],planeNormal);
				m_oPlanes[ui].Set(m_oVertices[m_oIndices[uiIndex]],planeNormal);

				planeNormal.Mul(planeNormal,planeNormal);
				m_oAxis[ui]=ZDOMINANT;
				if(planeNormal.fGetX()>planeNormal.fGetY() && planeNormal.fGetX()>planeNormal.fGetZ())
					m_oAxis[ui]=XDOMINANT;
				if(planeNormal.fGetY()>planeNormal.fGetX() && planeNormal.fGetY()>planeNormal.fGetZ())
					m_oAxis[ui]=YDOMINANT;
				/*
				if(planeNormal.fGetX()>planeNormal.fGetY())
				{
					if(planeNormal.fGetY()>planeNormal.fGetZ())
						m_oAxis[ui]=XDOMINANT;		//x dominant axis
					else
					{
						if(planeNormal.fGetX()>planeNormal.fGetZ())
							m_oAxis[ui]=XDOMINANT;	//x dominant axis
						else
							m_oAxis[ui]=ZDOMINANT;	//z dominant axis
					}
				}
				else	//y>x
				{
					if(planeNormal.fGetX()>planeNormal.fGetZ())
						m_oAxis[ui]=YDOMINANT;		//y dominant axis
					else
					{
						if(planeNormal.fGetY()>planeNormal.fGetZ())
							m_oAxis[ui]=YDOMINANT;	//y dominant axis
						else
							m_oAxis[ui]=ZDOMINANT;	//z dominant axis
					}
				}*/
			}
			return true;
		}
		return false;
	}
}

#endif