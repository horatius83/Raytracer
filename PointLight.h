#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Vector.h"
#include "Color.h"

namespace RayTracer
{
	class PointLight
	{
	public:
		PointLight() : m_oPosition(0.0f,0.0f,0.0f),m_oColor(1.0f,1.0f,1.0f) {};
		PointLight(const Math::Vector& oPosition,const Graphics::Color<float>& oColor)
		{
			m_oPosition = oPosition;
			m_oColor = oColor;
		};
		
		inline void Set(const Math::Vector& oPosition,const Graphics::Color<float>& oColor)
		{
			m_oPosition = oPosition;
			m_oColor = oColor;
		}
		inline void SetPosition(const Math::Vector& oPosition)
		{
			m_oPosition = oPosition;
		};

		inline void Move(const Math::Vector& oAmount)
		{
			m_oPosition.Add(m_oPosition,oAmount);
		}
		inline void SetColor(const Graphics::Color<float>& oColor)
		{
			m_oColor = oColor;
		};

		
		inline Math::Vector&		oGetPosition()
		{
			return m_oPosition;
		};

		inline Graphics::Color<float>&	oGetColor()
		{
			return m_oColor;
		};
		
		inline void GetLightRay(Math::Vector& oOutRay,const Math::Vector& oPosition)
		{
			oOutRay.Sub(m_oPosition,oPosition);
			oOutRay.Normalize();
		}
		
		inline void GetLightDotProduct(Math::Vector128& oDot,Math::PacketVector& oPosition,
			Math::PacketVector& oSurfaceNormal)
		{	

			Math::PacketVector oLightRay;
			
			oLightRay.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oPosition.GetX()),oPosition.m_sseX);
			oLightRay.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oPosition.GetY()),oPosition.m_sseY);
			oLightRay.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oPosition.fGetZ()),oPosition.m_sseZ);
			
			oLightRay.Normalize();
			oLightRay.Mul(_mm_set_ps1(-1.0f));
			oSurfaceNormal.Normalize();
			oLightRay.DotProduct(oDot.m_sseData,oSurfaceNormal);
		}
	private:
		Math::Vector				m_oPosition;
		Graphics::Color<float>	m_oColor;
	};
}

#endif