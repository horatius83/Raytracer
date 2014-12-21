#ifndef Vector_H
#define Vector_H

#include <math.h>
#include <xmmintrin.h>
#include <fvec.h>
#include <sstream>
#include "PacketVector.h"

namespace Math
{
	//const float g_fMaxRayLength = 100000.0f;

	class Vector
	{
	public:
		Vector() : m_fX(0.0f), m_fY(0.0f), m_fZ(0.0f), m_fW(0.0f) {};
		Vector(float fx, float fy, float fz, float fw = 0.0f) : m_fX(fx), m_fY(fy), m_fZ(fz), m_fW(fw) {};
		Vector(const Vector& oVec){ Set(oVec); };
		Vector(const __m128 sseData){ m_sseData = sseData; };

		//Accessors
		inline float	GetX()	const { return m_fX; };
		inline float	GetY()	const { return m_fY; };
		inline float	GetZ()	const { return m_fZ; };
		inline float	GetW()	const { return m_fW; };

		inline __m128	GetVector() const{ return m_sseData; };

		std::string toString() {
			std::ostringstream ss;
			ss << "(" << GetX() << ", " << GetY() << ", " << GetZ() << ")";
			return ss.str();
		}

		//Mutators
		inline void SetX(float fx){ m_fX = fx; };
		inline void SetY(float fy){ m_fY = fy; };
		inline void SetZ(float fz){ m_fZ = fz; };
		inline void SetW(float fw){ m_fW = fw; };
		void	Set(float fx, float fy, float fz, float fw = 0.0f);
		void	Set(const Vector& oVec);
		inline void Set(const __m128& sseData){ m_sseData = sseData; };
		void	Normalize();
		
		float	Dot(const Vector& oVec) const;
		float	Magnitude();
		float	fMagSquared() const;
		float	fDistanceSquared(const Vector& oVec);
	
		void			Mul(float f, const Vector& oVec);
		void			Mul(const Vector& oVecA, const Vector& oVecB);
		void			Add(const Vector& oVecA, const Vector& oVecB);
		void			Sub(const Vector& oVecA, const Vector& oVecB);
		void			Cross(const Vector& oVecA, const Vector& oVecB);
		void			Normal(const Vector& oVec);
		void			LessThan(Vector& oAns, const Vector& oVec);

		void			operator+=(const Vector& oVec);
		void			operator-=(const Vector& oVec);
		void			operator*=(float f);
		bool			operator!=(const Vector& oVec);

		const __m128 GetSse() { return m_sseData; }
	private:
		union
		{
			__m128	m_sseData;
			float	m_fVector[4];
			struct
			{
				float m_fX;
				float m_fY;
				float m_fZ;
				float m_fW;
			};
		};
	};

	inline void Vector::LessThan(Vector& oAns, const Vector& oVec)
	{
		oAns.Set(_mm_cmplt_ps(m_sseData, oVec.GetVector()));
	}

	inline bool Vector::operator !=(const Vector& oVec)
	{
		Math::Vector128 oAns;
		oAns.m_sseData = _mm_sub_ps(m_sseData, oVec.m_sseData);
		if (oAns.m_fData[0] + oAns.m_fData[1] + oAns.m_fData[2] + oAns.m_fData[3])
			return true;
		else
			return false;
	}

	inline void Vector::operator -=(const Vector& oVec)
	{
		m_sseData = _mm_sub_ps(m_sseData, oVec.m_sseData);
	}

	inline void Vector::operator +=(const Vector& oVec)
	{
		m_sseData = _mm_add_ps(m_sseData, oVec.m_sseData);
	}

	inline void Vector::operator *=(float f)
	{
		m_sseData = _mm_mul_ps(m_sseData, _mm_set_ps1(f));
	}

	inline void Vector::Sub(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_sseData = _mm_sub_ps(oVecA.m_sseData, oVecB.m_sseData);
	}

	inline void Vector::Add(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_sseData = _mm_add_ps(oVecA.m_sseData, oVecB.m_sseData);
	}

	inline void Vector::Mul(float f, const Math::Vector &oVec)
	{
		m_sseData = _mm_mul_ps(_mm_set_ps1(f), oVec.m_sseData);
	}

	inline void Vector::Mul(const Math::Vector& oVecA, const Math::Vector& oVecB)
	{
		m_sseData = _mm_mul_ps(oVecA.m_sseData, oVecB.m_sseData);
	}

	inline void Vector::Cross(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_fX = oVecA.m_fY*oVecB.m_fZ - oVecA.m_fZ*oVecB.m_fY;
		m_fY = oVecA.m_fZ*oVecB.m_fX - oVecA.m_fX*oVecB.m_fZ;
		m_fZ = oVecA.m_fX*oVecB.m_fY - oVecA.m_fY*oVecB.m_fX;
	}

	inline void Vector::Normal(const Math::Vector &oVec)
	{
		m_sseData = oVec.m_sseData;
		Normalize();
	}

	inline void Vector::Normalize()
	{
		__m128 sseData = m_sseData;
		m_sseData = _mm_mul_ps(m_sseData, m_sseData);
		float fResult = GetX() + GetY() + GetZ();	//dunno how to do this in SSE
		__m128 sseSum = _mm_set_ps1(fResult);

		__m128 sseHalf = _mm_set_ps1(0.5f);
		__m128 sseThree = _mm_set_ps1(3.0f);

		__m128 ssePreRecipricol = _mm_rsqrt_ps(sseSum);
		__m128 sseMuls = _mm_mul_ps(_mm_mul_ps(sseSum, ssePreRecipricol), ssePreRecipricol);
		__m128 sseReciprocol = _mm_mul_ps(_mm_mul_ps(sseHalf, ssePreRecipricol),
			_mm_sub_ps(sseThree, sseMuls));

		m_sseData = _mm_mul_ps(sseData, sseReciprocol);
	}

	inline float Vector::fDistanceSquared(const Vector& oVec)
	{
		return (oVec.m_fX - m_fX)*(oVec.m_fX - m_fX) +
			(oVec.m_fY - m_fY)*(oVec.m_fY - m_fY) +
			(oVec.m_fZ - m_fZ)*(oVec.m_fZ - m_fZ);
	}

	inline float Vector::fMagSquared() const
	{
		return m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ;
	}

	inline float Vector::Magnitude()
	{
		Vector oVec;
		oVec.Mul(m_sseData, m_sseData);
		oVec.Set(_mm_set_ps1(oVec.GetX() + oVec.GetY() + oVec.GetZ()));
		oVec.Set(_mm_sqrt_ps(oVec.GetVector()));
		return oVec.GetX();
	}

	inline float Vector::Dot(const Vector& oVec) const
	{
		Vector oTemp(_mm_mul_ps(m_sseData, oVec.m_sseData));
		return oTemp.GetX() + oTemp.GetY() + oTemp.GetZ();
	}

	inline void Vector::Set(const Vector& oVec)
	{
		m_sseData = oVec.m_sseData;
	}

	inline void Vector::Set(float fx, float fy, float fz, float fw)
	{
		m_fX = fx;	m_fY = fy;	m_fZ = fz;	m_fW = fw;
	}
}
#endif
