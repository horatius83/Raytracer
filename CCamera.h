#ifndef CCAMERA_H
#define CCAMERA_H

#include "CVector.h"

namespace NRayTracer
{
	class CCamera
	{
	public:
		CCamera() : m_oOrigin(0.0f,0.0f,0.0f),m_oDirection(0.0f,0.0f,1.0f) {};
		inline bool bCanSet(const NMath::CVector& oOrigin,const NMath::CVector& oDirection,const NMath::CVector& oUp,
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

		inline void Move(const NMath::CVector& oMove)
		{
			m_oOrigin.Add(m_oOrigin,oMove);
		}
		inline NMath::CVector& oGetOrigin()		{return m_oOrigin;};
		inline NMath::CVector& oGetDirection()	{return m_oDirection;};
		inline NMath::CVector& oGetUp()			{return m_oUp;};
		inline NMath::CVector& oGetRight()		{return m_oRight;};
		inline float fGetAspectRatio() const	{return m_fAspectRatio;};
		inline float fGetViewAngle() const		{return m_fAngle;};
	private:
		NMath::CVector	m_oOrigin;
		NMath::CVector	m_oDirection;
		NMath::CVector	m_oUp;
		NMath::CVector	m_oRight;
		float			m_fAspectRatio;
		float			m_fAngle;
	};
}

#endif