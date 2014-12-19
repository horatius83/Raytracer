#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"

namespace RayTracer
{
	class Camera
	{
	public:
		Camera() : m_oOrigin(0.0f,0.0f,0.0f),m_oDirection(0.0f,0.0f,1.0f) {};
		inline bool bCanSet(const Math::Vector& oOrigin,const Math::Vector& oDirection,const Math::Vector& oUp,
			float fAngle=1.570796f,float fAspectRatio=0.75f)
		{
			if(fAspectRatio>0.0f && fAngle>0.0f)
			{
				m_oOrigin = oOrigin;

				m_oDirection = oDirection;
				if(oDirection.fMagSquared()!=1.0f)
					m_oDirection.Normalize();
				
				m_oUp = oUp;
				if(oUp.fMagSquared()!=1.0f)
					m_oUp.Normalize();

				m_oRight.Cross(m_oDirection,oUp);
				if(m_oRight.fMagSquared()!=1.0f)
					m_oRight.Normalize();

				m_fAspectRatio	= fAspectRatio;
				m_fAngle		= fAngle;
				return true;
			}
			else
				return false;
		};

		inline void Move(const Math::Vector& oMove)
		{
			m_oOrigin.Add(m_oOrigin,oMove);
		}
		inline Math::Vector& oGetOrigin()		{return m_oOrigin;};
		inline Math::Vector& oGetDirection()	{return m_oDirection;};
		inline Math::Vector& oGetUp()			{return m_oUp;};
		inline Math::Vector& oGetRight()		{return m_oRight;};
		inline float fGetAspectRatio() const	{return m_fAspectRatio;};
		inline float fGetViewAngle() const		{return m_fAngle;};
	private:
		Math::Vector	m_oOrigin;
		Math::Vector	m_oDirection;
		Math::Vector	m_oUp;
		Math::Vector	m_oRight;
		float			m_fAspectRatio;
		float			m_fAngle;
	};
}

#endif