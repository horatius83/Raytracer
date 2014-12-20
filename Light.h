#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"
#include "Color.h"

namespace RayTracer
{
	class Light
	{
	public:
		Light() : m_Direction(0.0f,-1.0f,0.0f), m_oColor(1.0f,1.0f,1.0f) {};
		Light(const Math::Vector& Direction,const Graphics::Color<float>& oColor)
		{
			m_Direction = Direction.GetNormal();
			m_oColor = oColor;
		};
		inline void SetDirection(const Math::Vector& Direction)
		{
			m_Direction = Direction.GetNormal();
		};
		inline void SetColor(const Graphics::Color<float>& oColor)
		{
			m_oColor = oColor;
		};
		inline Math::Vector& GetDirection(){return m_Direction;};
		inline Graphics::Color<float>& GetColor(){return m_oColor;};
	private:
		Math::Vector				m_Direction;
		Graphics::Color<float>	m_oColor;
	};
}

#endif