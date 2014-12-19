#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"
#include "Color.h"

namespace RayTracer
{
	class Light
	{
	public:
		Light() : m_oDirection(0.0f,-1.0f,0.0f), m_oColor(1.0f,1.0f,1.0f) {};
		Light(const Math::Vector& oDirection,const Graphics::Color<float>& oColor)
		{
			m_oDirection = oDirection.oGetNormal();
			m_oColor = oColor;
		};
		inline void SetDirection(const Math::Vector& oDirection)
		{
			m_oDirection = oDirection.oGetNormal();
		};
		inline void SetColor(const Graphics::Color<float>& oColor)
		{
			m_oColor = oColor;
		};
		inline Math::Vector& oGetDirection(){return m_oDirection;};
		inline Graphics::Color<float>& oGetColor(){return m_oColor;};
	private:
		Math::Vector				m_oDirection;
		Graphics::Color<float>	m_oColor;
	};
}

#endif