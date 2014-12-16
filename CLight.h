#ifndef CLIGHT_H
#define CLIGHT_H

#include "CVector.h"
#include "TColor.h"
//#include "CColor.h"

namespace NRayTracer
{
	class CLight
	{
	public:
		CLight() : m_oDirection(0.0f,-1.0f,0.0f), m_oColor(1.0f,1.0f,1.0f) {};
		CLight(const NMath::CVector& oDirection,const NGraphics::TColor<float>& oColor)
		{
			m_oDirection = oDirection.oGetNormal();
			m_oColor = oColor;
		};
		inline void SetDirection(const NMath::CVector& oDirection)
		{
			m_oDirection = oDirection.oGetNormal();
		};
		inline void SetColor(const NGraphics::TColor<float>& oColor)
		{
			m_oColor = oColor;
		};
		inline NMath::CVector& oGetDirection(){return m_oDirection;};
		inline NGraphics::TColor<float>& oGetColor(){return m_oColor;};
	private:
		NMath::CVector				m_oDirection;
		NGraphics::TColor<float>	m_oColor;
	};
}

#endif