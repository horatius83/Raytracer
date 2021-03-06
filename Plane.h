#ifndef PLANE_H
#define PLANE_H

#include "Vector.h"
#include "PacketVector.h"
#include <cfloat>

namespace Math
{
	class Plane
	{
	public:
		Plane() : m_fa(0.0f),m_fb(0.0f),m_fc(0.0f),m_fd(0.0f) {};
		Plane(const Vector& oPoint,const Vector& oNormal)
		{
			Set(oPoint,oNormal);
		}
		void Set(const Vector& oPoint,const Vector& oNormal);
		inline float GetA(){return m_fa;};
		inline float GetB(){return m_fb;};
		inline float GetC(){return m_fc;};
		inline float GetD(){return m_fd;};
		
		void GetNormal(Vector& oNormal);
		inline Vector GetNormal()
		{
			return Vector(m_fa,m_fb,m_fc);
		};
		float GetIntersection(const Vector& oOrigin,const Vector& Direction);
		void GetIntersection(Vector128& oDistance,PacketVector& oOrigin,PacketVector& oVector);
	private:
		float	m_fa;
		float	m_fb;
		float	m_fc;
		float	m_fd;
	};

	inline void Plane::GetNormal(Vector& oNormal)
	{
		oNormal.Set(m_fa,m_fb,m_fc);
	}
		
	inline void Plane::Set(const Math::Vector &oPoint, const Math::Vector &oNormal)
	{
		m_fa = oNormal.GetX();
		m_fb = oNormal.GetY();
		m_fc = oNormal.GetZ();
		m_fd = -m_fa*oPoint.GetX() - m_fb*oPoint.GetY() -m_fc*oPoint.GetZ(); 
	}

	inline float Plane::GetIntersection(const Math::Vector &oOrigin, const Math::Vector &Direction)
	{
		//HADDPS __m128 _mm_hadd_ps(__m128 a, __m128 b)
		float fDirectionDot = m_fa*Direction.GetX()+m_fb*Direction.GetY()+m_fc*Direction.GetZ();
		
		if(fDirectionDot)
		{
			float fOriginDot = m_fa*oOrigin.GetX()+m_fb*oOrigin.GetY()+m_fc*oOrigin.GetZ();
			float fDistance = -(fOriginDot+m_fd)/fDirectionDot;
			if(fDistance>0.0f)
				return fDistance;
		}
		return FLT_MAX;
	}

	inline void Plane::GetIntersection(Vector128& oDistance,PacketVector& oOrigin,PacketVector& Direction)
	{
		PacketVector oNormal;
		oNormal.m_sseX = _mm_set_ps1(m_fa);
		oNormal.m_sseY = _mm_set_ps1(m_fb);
		oNormal.m_sseZ = _mm_set_ps1(m_fc);

		Vector128 oDotProduct;
		Direction.DotProduct(oDotProduct.m_sseData,oNormal);
		
		if(!_mm_movemask_ps(_mm_cmpeq_ps(oDotProduct.m_sseData,_mm_set_ps1(0.0f))))
		{
			Vector128 oOriginProduct;
			oOrigin.DotProduct(oOriginProduct.m_sseData,oNormal);
			oDistance.m_sseData = _mm_mul_ps(_mm_add_ps(oOriginProduct.m_sseData,_mm_set_ps1(m_fd)),_mm_set_ps1(-1.0f));
			oDistance.m_sseData = _mm_div_ps(oDistance.m_sseData,oDotProduct.m_sseData);
		}
		else
		{
			Vector oOriginVector,DirectionVector;

			oOriginVector.Set(oOrigin.m_fX[0],oOrigin.m_fY[0],oOrigin.m_fZ[0]);
			DirectionVector.Set(Direction.m_fX[0],Direction.m_fY[0],Direction.m_fZ[0]);
			oDistance.m_fData[0] = GetIntersection(oOriginVector,DirectionVector);
			
			oOriginVector.Set(oOrigin.m_fX[1],oOrigin.m_fY[1],oOrigin.m_fZ[1]);
			DirectionVector.Set(Direction.m_fX[1],Direction.m_fY[1],Direction.m_fZ[1]);
			oDistance.m_fData[1] = GetIntersection(oOriginVector,DirectionVector);

			oOriginVector.Set(oOrigin.m_fX[2],oOrigin.m_fY[2],oOrigin.m_fZ[2]);
			DirectionVector.Set(Direction.m_fX[2],Direction.m_fY[2],Direction.m_fZ[2]);
			oDistance.m_fData[2] = GetIntersection(oOriginVector,DirectionVector);

			oOriginVector.Set(oOrigin.m_fX[3],oOrigin.m_fY[3],oOrigin.m_fZ[3]);
			DirectionVector.Set(Direction.m_fX[3],Direction.m_fY[3],Direction.m_fZ[3]);
			oDistance.m_fData[3] = GetIntersection(oOriginVector,DirectionVector);
		}
	}
};

#endif