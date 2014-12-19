#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"

namespace RayTracer
{
	class Camera
	{
	public:
		Camera() : m_oOrigin(0.0f,0.0f,0.0f),m_Direction(0.0f,0.0f,1.0f) {};
		inline bool CanSet(const Math::Vector& oOrigin,const Math::Vector& Direction,const Math::Vector& oUp,
			float fAngle=1.570796f,float fAspectRatio=0.75f)
		{
			if(fAspectRatio>0.0f && fAngle>0.0f)
			{
				m_oOrigin = oOrigin;

				m_Direction = Direction;
				if(Direction.fMagSquared()!=1.0f)
					m_Direction.Normalize();
				
				m_oUp = oUp;
				if(oUp.fMagSquared()!=1.0f)
					m_oUp.Normalize();

				m_oRight.Cross(m_Direction,oUp);
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
		inline Math::Vector& GetOrigin()		{return m_oOrigin;};
		inline Math::Vector& GetDirection()	{return m_Direction;};
		inline Math::Vector& GetUp()			{return m_oUp;};
		inline Math::Vector& GetRight()		{return m_oRight;};
		inline float GetAspectRatio() const	{return m_fAspectRatio;};
		inline float GetViewAngle() const		{return m_fAngle;};
	private:
		Math::Vector	m_oOrigin;
		Math::Vector	m_Direction;
		Math::Vector	m_oUp;
		Math::Vector	m_oRight;
		float			m_fAspectRatio;
		float			m_fAngle;
	};
}

#endif