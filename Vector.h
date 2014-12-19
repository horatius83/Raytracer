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
		Vector(float fx,float fy,float fz,float fw=0.0f) : m_fX(fx),m_fY(fy),m_fZ(fz),m_fW(fw) {};
		Vector(const Vector& oVec){Set(oVec);};
		Vector(const __m128 sseData){m_sseData=sseData;};

		//Accessors
		inline float	fGetX()	const {return m_fX;};
		inline float	fGetY()	const {return m_fY;};
		inline float	fGetZ()	const {return m_fZ;};
		inline float	fGetW()	const {return m_fW;};

		//inline float*	pGetVector(){return &m_fVector[0];};
		inline __m128	sseGetVector() const{return m_sseData;};

		std::string toString() {
			std::ostringstream ss;
			ss << "(" << fGetX() << ", " << fGetY() << ", " << fGetZ() << ")";
			return ss.str();
		}

		//Mutators
		inline void SetX(float fx){m_fX=fx;};
		inline void SetY(float fy){m_fY=fy;};
		inline void SetZ(float fz){m_fZ=fz;};
		inline void SetW(float fw){m_fW=fw;};
		void	Set(float fx,float fy,float fz,float fw=0.0f);
		void	Set(const Vector& oVec);
		inline void Set(const __m128& sseData){m_sseData = sseData;}; 
		void	Normalize();
		//Vector	oGetNormal() const;
		//void	Normalize(float* fVecA,float* fVecB,float* fVecAns,unsigned int uiSize=4);

		float	fDot(const Vector& oVec) const;
		float	fMagnitude();
		float	fMagSquared() const;
		float	fDistanceSquared(const Vector& oVec);
		//Vector oCross(const Vector& oVec);

		/*friend Vector	operator *(float f,const Vector& oVec);
		friend Vector	operator *(const Vector& oVec,float f);
		friend Vector	operator +(const Vector& oVec1,const Vector& oVec2);
		friend Vector	operator -(const Vector& oVec1,const Vector& oVec2);*/
		void			Mul(float f,const Vector& oVec);
		void			Mul(const Vector& oVecA,const Vector& oVecB);
		void			Add(const Vector& oVecA,const Vector& oVecB);
		void			Sub(const Vector& oVecA,const Vector& oVecB);
		void			Cross(const Vector& oVecA,const Vector& oVecB);
		void			Normal(const Vector& oVec);
		void			LessThan(Vector& oAns,const Vector& oVec);

		void			operator+=(const Vector& oVec);
		void			operator-=(const Vector& oVec);
		void			operator*=(float f);
		bool			operator!=(const Vector& oVec);

		//static float	fFastSqrt(float f);
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

	inline void Vector::LessThan(Vector& oAns,const Vector& oVec)
	{
		oAns.Set(_mm_cmplt_ps(m_sseData,oVec.sseGetVector()));
	}

	inline bool Vector::operator !=(const Vector& oVec)
	{
		Math::Vector128 oAns;
		oAns.m_sseData = _mm_sub_ps(m_sseData,oVec.m_sseData);
		if(oAns.m_fData[0]+oAns.m_fData[1]+oAns.m_fData[2]+oAns.m_fData[3])
			return true;
		else
			return false;
		/*
		if(m_fX!=oVec.m_fX)
			return true;
		else
		{
			if(m_fY!=oVec.m_fY)
				return true;
			else
			{
				if(m_fZ!=oVec.m_fZ)
					return true;
				else
					return false;
			}
		}*/
	}

	inline void Vector::operator -=(const Vector& oVec)
	{
		/*
		m_fX-=oVec.m_fX;	m_fY-=oVec.m_fY;
		m_fZ-=oVec.m_fZ;	//m_fW-=oVec.m_fW;*/
		m_sseData = _mm_sub_ps(m_sseData,oVec.m_sseData);
	}

	inline void Vector::operator +=(const Vector& oVec)
	{
		/*
		m_fX+=oVec.m_fX;	m_fY+=oVec.m_fY;
		m_fZ+=oVec.m_fZ;	//m_fW+=oVec.m_fW;*/
		m_sseData = _mm_add_ps(m_sseData,oVec.m_sseData);
	}

	inline void Vector::operator *=(float f)
	{
		//m_fX*=f;	m_fY*=f;	m_fZ*=f;	//m_fW*=f;
		m_sseData = _mm_mul_ps(m_sseData,_mm_set_ps1(f));
	}
	
	inline void Vector::Sub(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_sseData = _mm_sub_ps(oVecA.m_sseData,oVecB.m_sseData);
	}

	inline void Vector::Add(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_sseData = _mm_add_ps(oVecA.m_sseData,oVecB.m_sseData);
	}

	inline void Vector::Mul(float f, const Math::Vector &oVec)
	{
		m_sseData = _mm_mul_ps(_mm_set_ps1(f),oVec.m_sseData);
	}

	inline void Vector::Mul(const Math::Vector& oVecA,const Math::Vector& oVecB)
	{
		m_sseData = _mm_mul_ps(oVecA.m_sseData,oVecB.m_sseData);
	}

	inline void Vector::Cross(const Math::Vector &oVecA, const Math::Vector &oVecB)
	{
		m_fX = oVecA.m_fY*oVecB.m_fZ-oVecA.m_fZ*oVecB.m_fY;
		m_fY = oVecA.m_fZ*oVecB.m_fX-oVecA.m_fX*oVecB.m_fZ;
		m_fZ = oVecA.m_fX*oVecB.m_fY-oVecA.m_fY*oVecB.m_fX;
		//return Vector(
		//m_fY*oVec.m_fZ-m_fZ*oVec.m_fY,
		//m_fZ*oVec.m_fX-m_fX*oVec.m_fZ,
		//m_fX*oVec.m_fY-m_fY*oVec.m_fX,0.0f);
	}

	inline void Vector::Normal(const Math::Vector &oVec)
	{
		m_sseData = oVec.m_sseData;
		Normalize();
	}
/*
	inline Vector operator-(const Vector& oVec1,const Vector& oVec2)
	{
		Vector oAnswer;
		oAnswer.m_sseData = _mm_sub_ps(oVec1.m_sseData,oVec2.m_sseData);
		return oAnswer;
	}

	inline Vector operator+(const Vector& oVec1,const Vector& oVec2)
	{
		Vector oAnswer;
		oAnswer.m_sseData = _mm_add_ps(oVec1.m_sseData,oVec2.m_sseData);
		return oAnswer;
	}

	inline Vector operator*(const Vector& oVec1,const Vector& oVec2)
	{
		//return Vector(oVec1.fGetX()*oVec2.fGetX(),oVec1.fGetY()*oVec2.fGetY(),oVec1.fGetZ()*oVec2.fGetZ());
		return Vector(_mm_mul_ps(oVec1.sseGetVector(),oVec2.sseGetVector()));
		
	}

	inline Vector operator*(const Vector& oVec,float f)
	{
		Vector oAnswer;
		__m128 sseScalar = _mm_set_ps1(f);
		oAnswer.m_sseData = _mm_mul_ps(oVec.m_sseData,sseScalar);
		return oAnswer;
	}

	inline Vector operator*(float f,const Vector& oVec)
	{
		Vector oAnswer;
		__m128 sseScalar = _mm_set_ps1(f);
		oAnswer.m_sseData = _mm_mul_ps(oVec.m_sseData,sseScalar);
		return oAnswer;
	}

	//Warning: vector is calculated in 3-space, not 4
	inline Vector Vector::oCross(const Vector& oVec)
	{
		return Vector(m_fY*oVec.m_fZ-m_fZ*oVec.m_fY,m_fZ*oVec.m_fX-m_fX*oVec.m_fZ,m_fX*oVec.m_fY-m_fY*oVec.m_fX,0.0f);
	}

	inline Vector Vector::oGetNormal() const
	{
		Vector oAnswer;
		oAnswer.m_sseData = _mm_mul_ps(m_sseData,m_sseData);
		float fResult = oAnswer.fGetX()+oAnswer.fGetY()+oAnswer.fGetZ();	//dunno how to do this in SSE
		__m128 sseSum = _mm_set_ps1(fResult);

		__m128 sseHalf = _mm_set_ps1(0.5f);
		__m128 sseThree = _mm_set_ps1(3.0f);

		__m128 ssePreRecipricol = _mm_rsqrt_ps(sseSum);
		__m128 sseMuls = _mm_mul_ps(_mm_mul_ps(sseSum,ssePreRecipricol),ssePreRecipricol);
		__m128 sseReciprocol = _mm_mul_ps(_mm_mul_ps(sseHalf,ssePreRecipricol),
			_mm_sub_ps(sseThree,sseMuls));

		oAnswer.m_sseData = _mm_mul_ps(m_sseData,sseReciprocol);
		return oAnswer;
	}
*/
	inline void Vector::Normalize()
	{
		/*
		float fInvMag = 1.0f/fMagnitude();
		m_fX*=fInvMag;
		m_fY*=fInvMag;
		m_fZ*=fInvMag;*/
		__m128 sseData = m_sseData;
		m_sseData = _mm_mul_ps(m_sseData,m_sseData);
		float fResult = fGetX()+fGetY()+fGetZ();	//dunno how to do this in SSE
		__m128 sseSum = _mm_set_ps1(fResult);

		__m128 sseHalf = _mm_set_ps1(0.5f);
		__m128 sseThree = _mm_set_ps1(3.0f);

		__m128 ssePreRecipricol = _mm_rsqrt_ps(sseSum);
		__m128 sseMuls = _mm_mul_ps(_mm_mul_ps(sseSum,ssePreRecipricol),ssePreRecipricol);
		__m128 sseReciprocol = _mm_mul_ps(_mm_mul_ps(sseHalf,ssePreRecipricol),
			_mm_sub_ps(sseThree,sseMuls));

		m_sseData = _mm_mul_ps(sseData,sseReciprocol);

		/*_asm
		{
			movups	xmm0,fOutput
			movaps	xmm2,xmm0
			mulps	xmm0,xmm0
			movaps	xmm1,xmm0
			shufps	xmm0,xmm1,01001110b
			addps	xmm0,xmm1
			movaps	xmm1,xmm0
			shufps	xmm1,xmm1,00010001b
			addps	xmm0,xmm1
			rsqrtps	xmm0,xmm0
			mulps	xmm2,xmm0
			movups	fOutput,xmm2
		}*/
		/*
		_asm	
		{
			movups	xmm2,oVec
			movaps	xmm0,xmm2	;load vector
			mulps	xmm0,xmm0	;multiply by itself
			movhlps	xmm1,xmm0	;Move two packed single-precision floating-point values from high quadword of xmm2 to low quadword of xmm1
			addps	xmm0,xmm1	;xmm0+=xmm1
			pshufd	xmm1,xmm0,01010101b	;Shuffle to 2nd element of xmm1
			addss	xmm0,xmm1 ;add single scalar
			rsqrtss	xmm0,xmm0	;square root
			shufps	xmm0,xmm0,00000000b
			mulps	xmm0,xmm2
			movups	oVec,xmm0
		}*/
	}

	inline float Vector::fDistanceSquared(const Vector& oVec)
	{
		return (oVec.m_fX-m_fX)*(oVec.m_fX-m_fX)+
			(oVec.m_fY-m_fY)*(oVec.m_fY-m_fY)+
			(oVec.m_fZ-m_fZ)*(oVec.m_fZ-m_fZ);
	}

	inline float Vector::fMagSquared() const
	{
		return m_fX*m_fX+m_fY*m_fY+m_fZ*m_fZ;
	}

	inline float Vector::fMagnitude()
	{
		//return sqrtf(m_fX*m_fX+m_fY*m_fY+m_fZ*m_fZ);
		//return fFastSqrt(m_fX*m_fX+m_fY*m_fY+m_fZ*m_fZ);
		Vector oVec;
		oVec.Mul(m_sseData,m_sseData);
		//oVec.fGetX()+oVec.fGetY()+oVec.fGetZ());
		oVec.Set(_mm_set_ps1(oVec.fGetX()+oVec.fGetY()+oVec.fGetZ()));
		oVec.Set(_mm_sqrt_ps(oVec.sseGetVector()));
		return oVec.fGetX();
	}

	inline float Vector::fDot(const Vector& oVec) const
	{
		Vector oTemp(_mm_mul_ps(m_sseData,oVec.m_sseData));
		return oTemp.fGetX()+oTemp.fGetY()+oTemp.fGetZ();//+m_fW*oVec.m_fW;
	}
/*
	//Square root approximation (Based off of Taylor Series I think)
	inline float Vector::fFastSqrt(float f)
	{
		_asm
		{
			mov eax,f
			sub eax,0x3f800000
			sar eax,1
			add eax,0x3f800000
			mov f,eax
		}
		return f;
	}
*/
	inline void Vector::Set(const Vector& oVec)
	{
		m_sseData = oVec.m_sseData;
		/*
			m_fX = oVec.m_fX;	m_fY = oVec.m_fY;
			m_fZ = oVec.m_fZ;	m_fW = oVec.m_fW;*/
	}

	inline void Vector::Set(float fx,float fy,float fz,float fw)
	{
		m_fX=fx;	m_fY=fy;	m_fZ=fz;	m_fW=fw;
	}
}
#endif
