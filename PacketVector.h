#ifndef PACKETVECTOR_H
#define PACKETVECTOR_H

//http://www.intel.com/cd/ids/developer/asmo-na/eng/dc/threading/245711.htm?page=4
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclang/html/vcrefcomparisonsforthestreamingsimdextensions.asp
#include <xmmintrin.h>
#include <emmintrin.h>

namespace Math
{
	const float g_fMaxRay=10000.0f;

	struct Vector128
	{
		union
		{
			float m_fData[4];
			__m128	m_sseData;
		};
	};

	struct Vector128ui
	{
		union
		{
			unsigned int m_uiData[4];
			__m128i		 m_sseData;
		};
	};

	class PacketVector
	{
	public:
		PacketVector(){};
		PacketVector(const PacketVector& oPacket)
		{
			Set(oPacket);
		}
		//Are all these rays going to the same place?
		inline bool IsCoherentPacket()
		{
			int im1 = _mm_movemask_ps(m_sseX);
			int im2 = _mm_movemask_ps(m_sseY);
			int im3 = _mm_movemask_ps(m_sseZ);
			if(((im1==0)||(im1==15))&&((im2==0)||(im2==15))&&((im3==0)||(im3==15)))
				return true;
			else
				return false;
		}
		inline void Normalize()
		{
			__m128 sseSum;
			DotProduct(sseSum,*this);

			__m128 sseHalf = _mm_set_ps1(0.5f);
			__m128 sseThree = _mm_set_ps1(3.0f);

			__m128 ssePreRecipricol = _mm_rsqrt_ps(sseSum);
			__m128 sseMuls = _mm_mul_ps(_mm_mul_ps(sseSum,ssePreRecipricol),ssePreRecipricol);
			__m128 sseReciprocol = _mm_mul_ps(_mm_mul_ps(sseHalf,ssePreRecipricol),
				_mm_sub_ps(sseThree,sseMuls));
			
			m_sseX = _mm_mul_ps(m_sseX,sseReciprocol);
			m_sseY = _mm_mul_ps(m_sseY,sseReciprocol);
			m_sseZ = _mm_mul_ps(m_sseZ,sseReciprocol);
		}
		inline void DotProduct(__m128& sseAnswer,PacketVector& oVector)
		{
			__m128 sseV1 = _mm_mul_ps(m_sseX,oVector.m_sseX);
			__m128 sseV2 = _mm_mul_ps(m_sseY,oVector.m_sseY);
			__m128 sseV3 = _mm_mul_ps(m_sseZ,oVector.m_sseZ);
			sseAnswer = _mm_add_ps(sseV1,_mm_add_ps(sseV2,sseV3));
		}	
		inline void Set(const PacketVector& oPacket)
		{
			m_sseX = oPacket.m_sseX;
			m_sseY = oPacket.m_sseY;
			m_sseZ = oPacket.m_sseZ;
		}
		inline void Set(float fx,float fy,float fz)
		{
			m_sseX = _mm_set_ps1(fx);
			m_sseY = _mm_set_ps1(fy);
			m_sseZ = _mm_set_ps1(fz);
		}
		inline void Mul(const __m128& sseScalar,const PacketVector& oPacket)
		{
			m_sseX = _mm_mul_ps(sseScalar,oPacket.m_sseX);
			m_sseY = _mm_mul_ps(sseScalar,oPacket.m_sseY);
			m_sseZ = _mm_mul_ps(sseScalar,oPacket.m_sseZ);
		}
		inline void Mul(const __m128& sseScalar)
		{
			m_sseX = _mm_mul_ps(sseScalar,m_sseX);
			m_sseY = _mm_mul_ps(sseScalar,m_sseY);
			m_sseZ = _mm_mul_ps(sseScalar,m_sseZ);
		}
		inline void Mul(const PacketVector& oPacketA,const PacketVector& oPacketB)
		{
			m_sseX = _mm_mul_ps(oPacketA.m_sseX,oPacketB.m_sseX);
			m_sseY = _mm_mul_ps(oPacketA.m_sseY,oPacketB.m_sseY);
			m_sseZ = _mm_mul_ps(oPacketA.m_sseZ,oPacketB.m_sseZ);
		}
		inline void Mul(const PacketVector& oPacket)
		{
			m_sseX = _mm_mul_ps(m_sseX,oPacket.m_sseX);
			m_sseY = _mm_mul_ps(m_sseY,oPacket.m_sseY);
			m_sseZ = _mm_mul_ps(m_sseZ,oPacket.m_sseZ);
		}
		inline void Div(const __m128& sseScalar,const PacketVector& oPacket)
		{
			m_sseX = _mm_div_ps(sseScalar,oPacket.m_sseX);
			m_sseY = _mm_div_ps(sseScalar,oPacket.m_sseY);
			m_sseZ = _mm_div_ps(sseScalar,oPacket.m_sseZ);
		}
		inline void Div(const PacketVector& oPacket,const __m128& sseScalar)
		{
			m_sseX = _mm_div_ps(oPacket.m_sseX,sseScalar);
			m_sseY = _mm_div_ps(oPacket.m_sseY,sseScalar);
			m_sseZ = _mm_div_ps(oPacket.m_sseZ,sseScalar);
		}
		inline void Add(const PacketVector& oPacketA,const PacketVector& oPacketB)
		{
			m_sseX = _mm_add_ps(oPacketA.m_sseX,oPacketB.m_sseX);
			m_sseY = _mm_add_ps(oPacketA.m_sseY,oPacketB.m_sseY);
			m_sseZ = _mm_add_ps(oPacketA.m_sseZ,oPacketB.m_sseZ);
		}
		inline void Add(const PacketVector& oPacket)
		{
			m_sseX = _mm_add_ps(m_sseX,oPacket.m_sseX);
			m_sseY = _mm_add_ps(m_sseY,oPacket.m_sseY);
			m_sseZ = _mm_add_ps(m_sseZ,oPacket.m_sseZ);
		}
		inline void Sub(const PacketVector& oPacketA,const PacketVector& oPacketB)
		{
			m_sseX = _mm_sub_ps(oPacketA.m_sseX,oPacketB.m_sseX);
			m_sseY = _mm_sub_ps(oPacketA.m_sseY,oPacketB.m_sseY);
			m_sseZ = _mm_sub_ps(oPacketA.m_sseZ,oPacketB.m_sseZ);
		}
		inline void Sub(const PacketVector& oPacket)
		{
			m_sseX = _mm_sub_ps(m_sseX,oPacket.m_sseX);
			m_sseY = _mm_sub_ps(m_sseY,oPacket.m_sseY);
			m_sseZ = _mm_sub_ps(m_sseZ,oPacket.m_sseZ);
		}
		union{__m128 m_sseX;float m_fX[4];};
		union{__m128 m_sseY;float m_fY[4];};
		union{__m128 m_sseZ;float m_fZ[4];};
	};
}

#endif