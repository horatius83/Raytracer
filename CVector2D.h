#ifndef CVECTOR2D_H
#define CVECTOR2D_H

#include <cmath>

namespace NMath
{
	class CVector2D
	{
	public:
		CVector2D() : m_fX(0.0f),m_fY(0.0f) {};
		CVector2D(float fx,float fy)
		{
			Set(fx,fy);
		};
		CVector2D(const CVector2D& oVec)
		{
			Set(oVec);
		};

		void Set(float fx,float fy);
		void Set(const CVector2D& oVec);
		inline void SetX(float fx){m_fX=fx;};
		inline void SetY(float fy){m_fY=fy;};
		inline float fGetX() const {return m_fX;};
		inline float fGetY() const {return m_fY;};

		void Normalize();
		CVector2D oGetNormal();
		float fDot(const CVector2D& oVec);
		float fMagnitude();
		float fMagnitudeSquared();
		friend CVector2D	operator *(float f,const CVector2D& oVec);
		friend CVector2D	operator *(const CVector2D& oVec2D,float f);
		friend CVector2D	operator +(const CVector2D& oVec1,const CVector2D& oVec2);
		friend CVector2D	operator -(const CVector2D& oVec1,const CVector2D& oVec2);
		void				operator+=(const CVector2D& oVec);
		void				operator-=(const CVector2D& oVec);
		void				operator*=(float f);
		bool				operator!=(const CVector2D& oVec);
	private:
		float m_fX;
		float m_fY;
	};

	inline void CVector2D::Normalize()
	{
		float f = 1.0f/fMagnitude();
		m_fX*=f;
		m_fY*=f;
	};

	inline CVector2D CVector2D::oGetNormal()
	{
		CVector2D oTemp(*this);
		oTemp.Normalize();
		return oTemp;
	};

	inline float CVector2D::fDot(const NMath::CVector2D &oVec)
	{
		return m_fX*oVec.fGetX()+m_fY*oVec.fGetY();
	};
	
	inline float CVector2D::fMagnitudeSquared()
	{
		return m_fX*m_fX+m_fY*m_fY;
	};

	inline float CVector2D::fMagnitude()
	{
		return sqrtf(fMagnitudeSquared());
	};

	inline bool CVector2D::operator !=(const CVector2D& oVec)
	{
		if(m_fX!=oVec.m_fX)
			return true;
		else
		{
			if(m_fY!=oVec.m_fY)
				return true;
			else
				return false;
		}
	}

	inline void CVector2D::operator -=(const CVector2D& oVec)
	{
		m_fX-=oVec.m_fX;	m_fY-=oVec.m_fY;
	}

	inline void CVector2D::operator +=(const CVector2D& oVec)
	{
		m_fX+=oVec.m_fX;	m_fY+=oVec.m_fY;
	}

	inline void CVector2D::operator *=(float f)
	{
		m_fX*=f;	m_fY*=f;
	}

	inline CVector2D operator-(const CVector2D& oVec1,const CVector2D& oVec2)
	{
		return CVector2D(oVec1.m_fX-oVec2.m_fX,oVec1.m_fY-oVec2.m_fY);
	}

	inline CVector2D operator+(const CVector2D& oVec1,const CVector2D& oVec2)
	{
		return CVector2D(oVec1.m_fX+oVec2.m_fX,oVec1.m_fY+oVec2.m_fY);
	}

	inline CVector2D operator*(const CVector2D& oVec1,const CVector2D& oVec2)
	{
		return CVector2D(oVec1.fGetX()*oVec2.fGetX(),oVec1.fGetY()*oVec2.fGetY());
	}

	inline CVector2D operator*(const CVector2D& oVec,float f)
	{
		return CVector2D(f*oVec.m_fX,f*oVec.m_fY);
	}

	inline CVector2D operator*(float f,const CVector2D& oVec)
	{
		return CVector2D(f*oVec.m_fX,f*oVec.m_fY);
	}

	inline void CVector2D::Set(const CVector2D& oVec)
	{
		m_fX = oVec.fGetX();
		m_fY = oVec.fGetY();
	}

	inline void CVector2D::Set(float fx,float fy)
	{
		m_fX=fx;
		m_fY=fy;
	};
}

#endif