#ifndef POLYGONLIST_H
#define POLYGONLIST_H

//#include "TArray.h"
#include <assert.h>
#include "Vector.h"
#include "Vector2D.h"
#include "Plane.h"
#include "PacketVector.h"
#include "AlignedArray.h"

namespace RayTracer
{
	using Utility::AlignedArray;

	class PolygonList
	{
	public:
		bool CanInitialize(AlignedArray<unsigned int>& oIndices, AlignedArray<Math::Vector>& oVertices,
			AlignedArray<Math::Vector>& oNormals, AlignedArray<Math::Vector2D>& oTexCoords);
		void GetIntersection(Math::Vector128& oResults, unsigned int uiIndex,
			Math::PacketVector& oOrigin, Math::PacketVector& Direction);
		float GetIntersection(unsigned int uiIndex, const Math::Vector& oOrigin, const Math::Vector& Direction);

		//The index for the polygon plus the vertex index (can be 0,1, or 2)
		Math::Vector&	GetVertex(unsigned int uiPolygonIndex, unsigned int uiVertexIndex);
		Math::Vector&	GetNormal(unsigned int uiPolygonIndex, unsigned int uiVertexIndex);
		Math::Vector2D&	GetTexCoords(unsigned int uiPolygonIndex, unsigned int uiVertexIndex);
		void	GetPlaneNormal(Math::Vector& oNormal, unsigned int uiPolygonIndex);

		//The number of polygons, not the number of vertices
		inline unsigned		GetSize(){ return m_oPlanes.GetSize(); };
	private:
		bool DoesCross(float fPointU, float fPointV, float fVectorU, float fVectorV);

		enum EPolygonType{ NOT_INITIALIZED, XDOMINANT, YDOMINANT, ZDOMINANT };
		//1 dominant axis and plane per polygon
		AlignedArray<EPolygonType>		m_oAxis;
		AlignedArray<Math::Plane>	m_oPlanes;

		//3 Indices, Vertices, Normals, and TexCoords per polygon
		AlignedArray<unsigned int>		m_oIndices;
		AlignedArray<Math::Vector>	m_oVertices;
		AlignedArray<Math::Vector>	m_oNormals;
		AlignedArray<Math::Vector2D>	m_oTexCoords;

	};

	inline void PolygonList::GetPlaneNormal(Math::Vector& oNormal, unsigned int uiPolygonIndex)
	{
		assert(uiPolygonIndex < m_oPlanes.GetSize());
		m_oPlanes[uiPolygonIndex].GetNormal(oNormal);

	}

	inline Math::Vector& PolygonList::GetVertex(unsigned int uiPolygonIndex, unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex < 3);
		return m_oVertices[m_oIndices[uiPolygonIndex] + uiVertexIndex];
	}

	inline Math::Vector& PolygonList::GetNormal(unsigned int uiPolygonIndex, unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex < 3);
		return m_oNormals[m_oIndices[uiPolygonIndex] + uiVertexIndex];
	}

	inline Math::Vector2D& PolygonList::GetTexCoords(unsigned int uiPolygonIndex, unsigned int uiVertexIndex)
	{
		assert(uiVertexIndex < 3);
		return m_oTexCoords[m_oIndices[uiPolygonIndex] + uiVertexIndex];
	}

	inline void PolygonList::GetIntersection(Math::Vector128& oResults, unsigned int uiIndex,
		Math::PacketVector& oOrigin, Math::PacketVector& Direction)
	{
		using Math::Vector128;
		using Math::Vector;
		using Math::PacketVector;

		m_oPlanes[uiIndex].GetIntersection(oResults, oOrigin, Direction);
		//For some reason this is faster than the SSE equivalent
		int iMask = _mm_movemask_ps(_mm_cmplt_ps(oResults.m_sseData, _mm_set_ps1(FLT_MAX)));
		if (iMask)
		{
			//Determine intersection
			PacketVector oIntersection;
			oIntersection.Mul(oResults.m_sseData, Direction);
			oIntersection.Add(oOrigin, oIntersection);

			Vector128 oVectorU[3];
			Vector128 oVectorV[3];
			Vector128 oPointU[3];
			Vector128 oPointV[3];
			unsigned int uiVertex = uiIndex * 3;

			switch (m_oAxis[uiIndex])
			{
			case XDOMINANT:	//z and y
			{
				//Get vectors from A->B B->C and C->A
				//Vector A->B (1-0)
				oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex]].fGetZ());
				oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetY() -
					m_oVertices[m_oIndices[uiVertex]].GetY());

				//Translate point A according to intersection
				oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetZ()), oIntersection.m_sseZ);
				oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].GetY()), oIntersection.m_sseY);

				//Vector B->C (2-1)
				oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex + 1]].fGetZ());
				oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetY() -
					m_oVertices[m_oIndices[uiVertex + 1]].GetY());

				//Translate point B according to intersection
				oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].fGetZ()), oIntersection.m_sseZ);
				oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetY()), oIntersection.m_sseY);

				//Vector C->A [0-2]
				oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex + 2]].fGetZ());
				oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex]].GetY() -
					m_oVertices[m_oIndices[uiVertex + 2]].GetY());

				//Translate point C according to intersection
				oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].fGetZ()), oIntersection.m_sseZ);
				oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetY()), oIntersection.m_sseY);
			}break;
			case YDOMINANT:	//x and z
			{
				//Get vectors from A->B B->C and C->A
				//Vector A->B (1-0)
				oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex + 0]].fGetZ());
				oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 0]].GetX());

				//Translate point A according to intersection
				oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].fGetZ()), oIntersection.m_sseZ);
				oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetX()), oIntersection.m_sseX);

				//Vector B->C (2-1)
				oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex + 1]].fGetZ());
				oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 1]].GetX());

				//Translate point B according to intersection
				oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].fGetZ()), oIntersection.m_sseZ);
				oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetX()), oIntersection.m_sseX);

				//Vector C->A [0-2]
				oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].fGetZ() -
					m_oVertices[m_oIndices[uiVertex + 2]].fGetZ());
				oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 2]].GetX());

				//Translate point C according to intersection
				oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].fGetZ()), oIntersection.m_sseZ);
				oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetX()), oIntersection.m_sseX);
			}break;
			case ZDOMINANT: //x and y
			default:
			{
				//Get vectors from A->B B->C and C->A
				//Vector A->B (1-0)
				oVectorU[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 0]].GetX());
				oVectorV[0].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetY() -
					m_oVertices[m_oIndices[uiVertex + 0]].GetY());

				//Translate point A according to intersection
				oPointU[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetX()), oIntersection.m_sseX);
				oPointV[0].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetY()), oIntersection.m_sseY);

				//Vector B->C (2-1)
				oVectorU[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 1]].GetX());
				oVectorV[1].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetY() -
					m_oVertices[m_oIndices[uiVertex + 1]].GetY());

				//Translate point B according to intersection
				oPointU[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetX()), oIntersection.m_sseX);
				oPointV[1].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 1]].GetY()), oIntersection.m_sseY);

				//Vector C->A [0-2]
				oVectorU[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetX() -
					m_oVertices[m_oIndices[uiVertex + 2]].GetX());
				oVectorV[2].m_sseData = _mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 0]].GetY() -
					m_oVertices[m_oIndices[uiVertex + 2]].GetY());

				//Translate point C according to intersection
				oPointU[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetX()), oIntersection.m_sseX);
				oPointV[2].m_sseData = _mm_sub_ps(_mm_set_ps1(m_oVertices[m_oIndices[uiVertex + 2]].GetY()), oIntersection.m_sseY);
			}break;
			}

			//Once you get the crossing algorithm done the entire thing is done
			int iCrosses[4] = { 0, 0, 0, 0 };

			for (unsigned int ui = 0; ui < 3; ++ui)
			{

				if (DoesCross(oPointU[ui].m_fData[0], oPointV[ui].m_fData[0],
					oVectorU[ui].m_fData[0], oVectorV[ui].m_fData[0]))
					++iCrosses[0];

				if (DoesCross(oPointU[ui].m_fData[1], oPointV[ui].m_fData[1],
					oVectorU[ui].m_fData[1], oVectorV[ui].m_fData[1]))
					++iCrosses[1];

				if (DoesCross(oPointU[ui].m_fData[2], oPointV[ui].m_fData[2],
					oVectorU[ui].m_fData[2], oVectorV[ui].m_fData[2]))
					++iCrosses[2];

				if (DoesCross(oPointU[ui].m_fData[3], oPointV[ui].m_fData[3],
					oVectorU[ui].m_fData[3], oVectorV[ui].m_fData[3]))
					++iCrosses[3];
			}

			for (unsigned int ui = 0; ui < 4; ++ui)
			{
				if (!(iCrosses[0] % 2))
					oResults.m_fData[ui] = FLT_MAX;
			}
		}
		else
		{
			oResults.m_fData[0] = GetIntersection(uiIndex, Vector(oOrigin.m_fX[0], oOrigin.m_fY[0], oOrigin.m_fZ[0]),
				Vector(Direction.m_fX[0], Direction.m_fY[0], Direction.m_fZ[0]));
			oResults.m_fData[1] = GetIntersection(uiIndex, Vector(oOrigin.m_fX[1], oOrigin.m_fY[1], oOrigin.m_fZ[1]),
				Vector(Direction.m_fX[1], Direction.m_fY[1], Direction.m_fZ[1]));
			oResults.m_fData[2] = GetIntersection(uiIndex, Vector(oOrigin.m_fX[2], oOrigin.m_fY[2], oOrigin.m_fZ[2]),
				Vector(Direction.m_fX[2], Direction.m_fY[2], Direction.m_fZ[2]));
			oResults.m_fData[3] = GetIntersection(uiIndex, Vector(oOrigin.m_fX[3], oOrigin.m_fY[3], oOrigin.m_fZ[3]),
				Vector(Direction.m_fX[3], Direction.m_fY[3], Direction.m_fZ[3]));
		}
	}

	inline float PolygonList::GetIntersection(unsigned int uiIndex, const Math::Vector &oOrigin, const Math::Vector &Direction)
	{
		using Math::Vector;
		using Math::Vector2D;

		float fDistance = m_oPlanes[uiIndex].GetIntersection(oOrigin, Direction);

		if (fDistance < FLT_MAX)
		{
			int iCrosses = 0;
			//Get intersection point
			Vector oIntersect;
			oIntersect.Mul(fDistance, Direction);
			oIntersect.Sub(oOrigin, oIntersect);
			//Get vectors for each set of points (so three vectors for three points), and translate points by 
			//intersection point;
			//A->B intersection (vector = b-a)
			Vector oVec1, oPoint1;
			oVec1.Sub(m_oVertices[m_oIndices[uiIndex + 1]], m_oVertices[m_oIndices[uiIndex + 0]]);
			oPoint1.Sub(m_oVertices[m_oIndices[uiIndex + 0]], oIntersect);
			//B->C intersection (vector = c-b)
			Vector	oVec2, oPoint2;
			oVec2.Sub(m_oVertices[m_oIndices[uiIndex + 2]], m_oVertices[m_oIndices[uiIndex + 1]]);
			oPoint2.Sub(m_oVertices[m_oIndices[uiIndex + 1]], oIntersect);
			//C->A intersection (vector = a-c)
			Vector oVec3, oPoint3;
			oVec3.Sub(m_oVertices[m_oIndices[uiIndex + 0]], m_oVertices[m_oIndices[uiIndex + 2]]);
			oPoint3.Sub(m_oVertices[m_oIndices[uiIndex + 2]], oIntersect);

			Vector2D oPoint2D, oVector2D;
			switch (m_oAxis[uiIndex])
			{
			case XDOMINANT:	//z and y coords
			{
				//1rst intersection (A->B)
				if (DoesCross(oPoint1.fGetZ(), oPoint1.GetY(), oVec1.fGetZ(), oVec1.GetY()))
					++iCrosses;
				//2nd intersection (B->C)
				if (DoesCross(oPoint2.fGetZ(), oPoint2.GetY(), oVec2.fGetZ(), oVec2.GetY()))
					++iCrosses;
				//3rd intersection (C->A)
				if (DoesCross(oPoint3.fGetZ(), oPoint3.GetY(), oVec3.fGetZ(), oVec3.GetY()))
					++iCrosses;
			}break;
			case YDOMINANT: //x and z coords
			{
				//1rst intersection (A->B)
				if (DoesCross(oPoint1.GetX(), oPoint1.fGetZ(), oVec1.GetX(), oVec1.fGetZ()))
					++iCrosses;
				//2nd intersection (B->C)
				if (DoesCross(oPoint2.GetX(), oPoint2.fGetZ(), oVec2.GetX(), oVec2.fGetZ()))
					++iCrosses;
				//3rd intersection (C->A)
				if (DoesCross(oPoint3.GetX(), oPoint3.fGetZ(), oVec3.GetX(), oVec3.fGetZ()))
					++iCrosses;
			}break;
			case ZDOMINANT:	//x and y coords
			default:
			{
				//1rst intersection (A->B)
				if (DoesCross(oPoint1.GetX(), oPoint1.GetY(), oVec1.GetX(), oVec1.GetY()))
					++iCrosses;
				//2nd intersection (B->C)
				if (DoesCross(oPoint2.GetX(), oPoint2.GetY(), oVec2.GetX(), oVec2.GetY()))
					++iCrosses;
				//3rd intersection (C->A)
				if (DoesCross(oPoint3.GetX(), oPoint3.GetY(), oVec3.GetX(), oVec3.GetY()))
					++iCrosses;
			}break;
			}

			if (iCrosses % 2)
				return fDistance;
		}
		return FLT_MAX;
	}

	inline bool PolygonList::DoesCross(float fPointU, float fPointV, float fVectorU, float fVectorV)
	{
		if (fVectorV)
		{
			float t = -fPointV / fVectorV;
			if (t >= 0.0f && t <= 1.0f)
			{
				if (fPointU + t*fVectorU >= 0.0f)
					return true;
			}
		}
		return false;
	}

	bool PolygonList::CanInitialize(AlignedArray<unsigned int>& oIndices, AlignedArray<Math::Vector>& oVertices,
		AlignedArray<Math::Vector>& oNormals, AlignedArray<Math::Vector2D>& oTexCoords)
	{
		using Math::Vector;

		if (m_oVertices.GetSize() == m_oNormals.GetSize() && m_oVertices.GetSize() == m_oTexCoords.GetSize())
		{
			m_oIndices.Set(oIndices);
			m_oVertices.Set(oVertices);
			m_oNormals.Set(oNormals);
			m_oTexCoords.Set(oTexCoords);

			Vector a, b, planeNormal, avgNormal;
			m_oPlanes.Allocate(m_oIndices.GetSize() / 3);
			m_oAxis.Allocate(m_oPlanes.GetSize());

			for (unsigned int ui = 0; ui < m_oPlanes.GetSize(); ++ui)
			{
				//Create two vectors off of the 3 points 
				unsigned int uiIndex = ui * 3;
				a.Sub(m_oVertices[m_oIndices[uiIndex]], m_oVertices[m_oIndices[uiIndex + 1]]);
				a.Normalize();
				b.Sub(m_oVertices[m_oIndices[uiIndex]], m_oVertices[m_oIndices[uiIndex + 2]]);
				b.Normalize();
				//Calculate the dot product to get the plane-normal (although technically this produces 2 normals)
				planeNormal.Cross(a, b);
				//Get the average of all three point-normals
				avgNormal.Add(m_oNormals[m_oIndices[uiIndex]], m_oNormals[m_oIndices[uiIndex + 1]]);
				avgNormal.Add(avgNormal, m_oNormals[m_oIndices[uiIndex + 2]]);
				avgNormal.Normalize();
				//Determine which side the plane normal is supposed to be on
				if (avgNormal.Dot(planeNormal) < 0.0f)
					planeNormal.Mul(-1.0f, planeNormal);
				//Create plane
				planeNormal.Normalize();
				//m_oPlanes[ui].Set(m_oVertices[m_oIndices[uiIndex]],planeNormal);
				m_oPlanes[ui].Set(m_oVertices[m_oIndices[uiIndex]], planeNormal);

				planeNormal.Mul(planeNormal, planeNormal);
				m_oAxis[ui] = ZDOMINANT;
				if (planeNormal.GetX() > planeNormal.GetY() && planeNormal.GetX() > planeNormal.fGetZ())
					m_oAxis[ui] = XDOMINANT;
				if (planeNormal.GetY() > planeNormal.GetX() && planeNormal.GetY() > planeNormal.fGetZ())
					m_oAxis[ui] = YDOMINANT;
				/*
				if(planeNormal.GetX()>planeNormal.GetY())
				{
				if(planeNormal.GetY()>planeNormal.fGetZ())
				m_oAxis[ui]=XDOMINANT;		//x dominant axis
				else
				{
				if(planeNormal.GetX()>planeNormal.fGetZ())
				m_oAxis[ui]=XDOMINANT;	//x dominant axis
				else
				m_oAxis[ui]=ZDOMINANT;	//z dominant axis
				}
				}
				else	//y>x
				{
				if(planeNormal.GetX()>planeNormal.fGetZ())
				m_oAxis[ui]=YDOMINANT;		//y dominant axis
				else
				{
				if(planeNormal.GetY()>planeNormal.fGetZ())
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