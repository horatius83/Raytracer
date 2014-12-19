#ifndef Vector2D_H
#define Vector2D_H

#include <cmath>

namespace Math
{
	class Vector2D
	{
	public:
		Vector2D() : m_fX(0.0f),m_fY(0.0f) {};
		Vector2D(float fx,float fy)
		{
			Set(fx,fy);
		};
		Vector2D(const Vector2D& oVec)
		{
			Set(oVec);
		};

		void Set(float fx,float fy);
		void Set(const Vector2D& oVec);
		inline void SetX(float fx){m_fX=fx;};
		inline void SetY(float fy){m_fY=fy;};
		inline float fGetX() const {return m_fX;};
		inline float fGetY() const {return m_fY;};

		void Normalize();
		Vector2D oGetNormal();
		float fDot(const Vector2D& oVec);
		float fMagnitude();
		float fMagnitudeSquared();
		friend Vector2D	operator *(float f,const Vector2D& oVec);
		friend Vector2D	operator *(const Vector2D& oVec2D,float f);
		friend Vector2D	operator +(const Vector2D& oVec1,const Vector2D& oVec2);
		friend Vector2D	operator -(const Vector2D& oVec1,const Vector2D& oVec2);
		void				operator+=(const Vector2D& oVec);
		void				operator-=(const Vector2D& oVec);
		void				operator*=(float f);
		bool				operator!=(const Vector2D& oVec);
	private:
		float m_fX;
		float m_fY;
	};

	inline void Vector2D::Normalize()
	{
		float f = 1.0f/fMagnitude();
		m_fX*=f;
		m_fY*=f;
	};

	inline Vector2D Vector2D::oGetNormal()
	{
		Vector2D oTemp(*this);
		oTemp.Normalize();
		return oTemp;
	};

	inline float Vector2D::fDot(const Math::Vector2D &oVec)
	{
		return m_fX*oVec.fGetX()+m_fY*oVec.fGetY();
	};
	
	inline float Vector2D::fMagnitudeSquared()
	{
		return m_fX*m_fX+m_fY*m_fY;
	};

	inline float Vector2D::fMagnitude()
	{
		return sqrtf(fMagnitudeSquared());
	};

	inline bool Vector2D::operator !=(const Vector2D& oVec)
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

	inline void Vector2D::operator -=(const Vector2D& oVec)
	{
		m_fX-=oVec.m_fX;	m_fY-=oVec.m_fY;
	}

	inline void Vector2D::operator +=(const Vector2D& oVec)
	{
		m_fX+=oVec.m_fX;	m_fY+=oVec.m_fY;
	}

	inline void Vector2D::operator *=(float f)
	{
		m_fX*=f;	m_fY*=f;
	}

	inline Vector2D operator-(const Vector2D& oVec1,const Vector2D& oVec2)
	{
		return Vector2D(oVec1.m_fX-oVec2.m_fX,oVec1.m_fY-oVec2.m_fY);
	}

	inline Vector2D operator+(const Vector2D& oVec1,const Vector2D& oVec2)
	{
		return Vector2D(oVec1.m_fX+oVec2.m_fX,oVec1.m_fY+oVec2.m_fY);
	}

	inline Vector2D operator*(const Vector2D& oVec1,const Vector2D& oVec2)
	{
		return Vector2D(oVec1.fGetX()*oVec2.fGetX(),oVec1.fGetY()*oVec2.fGetY());
	}

	inline Vector2D operator*(const Vector2D& oVec,float f)
	{
		return Vector2D(f*oVec.m_fX,f*oVec.m_fY);
	}

	inline Vector2D operator*(float f,const Vector2D& oVec)
	{
		return Vector2D(f*oVec.m_fX,f*oVec.m_fY);
	}

	inline void Vector2D::Set(const Vector2D& oVec)
	{
		m_fX = oVec.fGetX();
		m_fY = oVec.fGetY();
	}

	inline void Vector2D::Set(float fx,float fy)
	{
		m_fX=fx;
		m_fY=fy;
	};
}

#endif