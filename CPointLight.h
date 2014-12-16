#ifndef CPOINTLIGHT_H
#define CPOINTLIGHT_H

#include "CVector.h"
//#include "CColor.h"
#include "TColor.h"

namespace NRayTracer
{
	class CPointLight
	{
	public:
		CPointLight() : m_oPosition(0.0f,0.0f,0.0f),m_oColor(1.0f,1.0f,1.0f) {};
		CPointLight(const NMath::CVector& oPosition,const NGraphics::TColor<float>& oColor)
		{
			m_oPosition = oPosition;
			m_oColor = oColor;
		};

		
		inline void Set(const NMath::CVector& oPosition,const NGraphics::TColor<float>& oColor)
		{
			m_oPosition = oPosition;
			m_oColor = oColor;
		}
		inline void SetPosition(const NMath::CVector& oPosition)
		{
			m_oPosition = oPosition;
		};

		inline void Move(const NMath::CVector& oAmount)
		{
			m_oPosition.Add(m_oPosition,oAmount);
		}
		inline void SetColor(const NGraphics::TColor<float>& oColor)
		{
			m_oColor = oColor;
		};

		
		inline NMath::CVector&		oGetPosition()
		{
			return m_oPosition;
		};

		inline NGraphics::TColor<float>&	oGetColor()
		{
			return m_oColor;
		};
		
		inline void GetLightRay(NMath::CVector& oOutRay,const NMath::CVector& oPosition)
		{
			oOutRay.Sub(m_oPosition,oPosition);
			oOutRay.Normalize();
		}
		/*
		inline NMath::CVector		oGetLightRay(NMath::CVector& oPosition)
		{
			NMath::CVector oRay;
			oRay.Sub(m_oPosition,oPosition);
			oRay.Normalize();
			return oRay;
		};*/
		inline void GetLightDotProduct(NMath::SVector128& oDot,NMath::CPacketVector& oPosition,
			NMath::CPacketVector& oSurfaceNormal)
		{	

			NMath::CPacketVector oLightRay;
			//oSurfaceNormal.Set(0.0f,0.0f,-1.0f);
			/*
			oLightRay.m_sseX = _mm_sub_ps(oPosition.m_sseX,_mm_set_ps1(m_oPosition.fGetX()));
			oLightRay.m_sseY = _mm_sub_ps(oPosition.m_sseY,_mm_set_ps1(m_oPosition.fGetY()));
			oLightRay.m_sseZ = _mm_sub_ps(oPosition.m_sseZ,_mm_set_ps1(m_oPosition.fGetZ()));
			*/
			oLightRay.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oPosition.fGetX()),oPosition.m_sseX);
			oLightRay.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oPosition.fGetY()),oPosition.m_sseY);
			oLightRay.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oPosition.fGetZ()),oPosition.m_sseZ);
			
			oLightRay.Normalize();
			oLightRay.Mul(_mm_set_ps1(-1.0f));
			oSurfaceNormal.Normalize();
			oLightRay.DotProduct(oDot.m_sseData,oSurfaceNormal);
			
			//oDot.m_sseData = _mm_mul_ps(oDot.m_sseData,_mm_set_ps1(-1.0f));
			
			//float fDot = -oLight.oGetLightRay(oOrigin+fDistance*m_oCamera.oGetDirection()).fDot(m_oPoly.oGetPlaneNormal());
		}
	private:
		NMath::CVector				m_oPosition;
		NGraphics::TColor<float>	m_oColor;
	};
}

#endif