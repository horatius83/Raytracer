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
		inline float fGetA(){return m_fa;};
		inline float fGetB(){return m_fb;};
		inline float fGetC(){return m_fc;};
		inline float fGetD(){return m_fd;};
		
		void GetNormal(Vector& oNormal);
		inline Vector oGetNormal()
		{
			return Vector(m_fa,m_fb,m_fc);
		};
		float fGetIntersection(const Vector& oOrigin,const Vector& oDirection);
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
		m_fa = oNormal.fGetX();
		m_fb = oNormal.fGetY();
		m_fc = oNormal.fGetZ();
		m_fd = -m_fa*oPoint.fGetX() - m_fb*oPoint.fGetY() -m_fc*oPoint.fGetZ(); 
	}

	inline float Plane::fGetIntersection(const Math::Vector &oOrigin, const Math::Vector &oDirection)
	{
		//HADDPS __m128 _mm_hadd_ps(__m128 a, __m128 b)
		float fDirectionDot = m_fa*oDirection.fGetX()+m_fb*oDirection.fGetY()+m_fc*oDirection.fGetZ();
		
		if(fDirectionDot)
		{
			float fOriginDot = m_fa*oOrigin.fGetX()+m_fb*oOrigin.fGetY()+m_fc*oOrigin.fGetZ();
			float fDistance = -(fOriginDot+m_fd)/fDirectionDot;
			if(fDistance>0.0f)
				return fDistance;
		}
		return FLT_MAX;
	}

	inline void Plane::GetIntersection(Vector128& oDistance,PacketVector& oOrigin,PacketVector& oDirection)
	{
		PacketVector oNormal;
		oNormal.m_sseX = _mm_set_ps1(m_fa);
		oNormal.m_sseY = _mm_set_ps1(m_fb);
		oNormal.m_sseZ = _mm_set_ps1(m_fc);

		Vector128 oDotProduct;
		oDirection.DotProduct(oDotProduct.m_sseData,oNormal);
		//Vector128 oComparison;

		//oComparison.m_sseData = _mm_cmpeq_ps(oDotProduct.m_sseData,_mm_set_ps1(0.0f));
		//oDotProduct.m_sseData = _mm_add_ps(
		//	_mm_andnot_ps(oComparison.m_sseData,oDotProduct.m_sseData),
		//	_mm_and_ps(oComparison.m_sseData,_mm_set_ps1(Math::g_fMaxRay)));

		//oComparison.m_sseData = _mm_cmpeq_ss(oDotProduct.m_sseData,_mm_set_ps1(0.0f));
		//if(!oComparison.m_fData[0])
		if(!_mm_movemask_ps(_mm_cmpeq_ps(oDotProduct.m_sseData,_mm_set_ps1(0.0f))))
		{
			Vector128 oOriginProduct;
			oOrigin.DotProduct(oOriginProduct.m_sseData,oNormal);
			oDistance.m_sseData = _mm_mul_ps(_mm_add_ps(oOriginProduct.m_sseData,_mm_set_ps1(m_fd)),_mm_set_ps1(-1.0f));
			oDistance.m_sseData = _mm_div_ps(oDistance.m_sseData,oDotProduct.m_sseData);
			//oDistance.m_sseData = _mm_div_ps(oDotProduct.m_sseData,oDistance.m_sseData);
		}
		else
		{
			//Vector oOrigin, oDirection;
			Vector oOriginVector,oDirectionVector;

			oOriginVector.Set(oOrigin.m_fX[0],oOrigin.m_fY[0],oOrigin.m_fZ[0]);
			oDirectionVector.Set(oDirection.m_fX[0],oDirection.m_fY[0],oDirection.m_fZ[0]);
			oDistance.m_fData[0] = fGetIntersection(oOriginVector,oDirectionVector);
			
			oOriginVector.Set(oOrigin.m_fX[1],oOrigin.m_fY[1],oOrigin.m_fZ[1]);
			oDirectionVector.Set(oDirection.m_fX[1],oDirection.m_fY[1],oDirection.m_fZ[1]);
			oDistance.m_fData[1] = fGetIntersection(oOriginVector,oDirectionVector);

			oOriginVector.Set(oOrigin.m_fX[2],oOrigin.m_fY[2],oOrigin.m_fZ[2]);
			oDirectionVector.Set(oDirection.m_fX[2],oDirection.m_fY[2],oDirection.m_fZ[2]);
			oDistance.m_fData[2] = fGetIntersection(oOriginVector,oDirectionVector);

			oOriginVector.Set(oOrigin.m_fX[3],oOrigin.m_fY[3],oOrigin.m_fZ[3]);
			oDirectionVector.Set(oDirection.m_fX[3],oDirection.m_fY[3],oDirection.m_fZ[3]);
			oDistance.m_fData[3] = fGetIntersection(oOriginVector,oDirectionVector);
			
		}
	}
};

#endif