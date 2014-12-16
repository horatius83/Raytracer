#ifndef TAABB_H
#define TAABB_H

#include "CPacketVector.h"
#include "CVector.h"
#include <cfloat>

//debug
#include "CPlane.h"
/*
--------*
|       |
|       |
*--------

For a cube 1 unit on each side the left point is (-1,-1,-1) and the right point is (1,1,1)
(this is important! the right point should be always be greater than the left point, otherwise
it will mess up the intersection calculations)
*/
namespace NMath
{
	template <class T>
	class TAabb
	{
	public:
		/*
		0000 = MMM = 0x0
		0010 = MMP = 0x2
		0100 = MPM = 0x4
		0110 = MPP = 0x6
		1000 = PMM = 0x8
		1010 = PMP = 0xA
		1100 = PPM = 0xC
		1110 = PPP = 0xE
		*/
		enum RAY_ORIENTATION{MMM, MMP=0x2, MPM=0x4, MPP=0x6, PMM=0x8, PMP=0xA, PPM=0xC, PPP=0xE};
		bool bIntersects(const CVector& oOrigin,const CVector& oDirection,RAY_ORIENTATION eOrientation);
		
		bool bIntersects(const CVector& oOrigin,const CVector& oDirection);
		void DoesIntersect(SVector128& oAnswer,const CPacketVector& oOrigin,const CPacketVector& oDirection);

		inline void SetLeftPoint(const NMath::CVector& oPoint){m_oLeftPoint=oPoint;};
		inline void SetRightPoint(const NMath::CVector& oPoint){m_oRightPoint=oPoint;};
		inline CVector& oGetLeftPoint(){return m_oLeftPoint;};
		inline CVector& oGetRightPoint(){return m_oRightPoint;};
		inline void SetData(const T& t){m_oData = t;};
		inline T&		oGetData(){return m_oData;};
	private:
		void DoesIntersectIndividual(SVector128& oAnswer,const CPacketVector& oOrigin,
			const CPacketVector& oDirection,unsigned int uiMask[]);
		CVector	m_oLeftPoint;
		CVector m_oRightPoint;
		T		m_oData;
	};

	template <class T>
	inline void TAabb<T>::DoesIntersect(SVector128& oAnswer,const CPacketVector& oOrigin,
		const CPacketVector& oDirection)
	{
		//Determine if all four vectors have the same orientation
		unsigned int uiMask[3];
		uiMask[0] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseX,_mm_set_ps1(0.0f)));
		uiMask[1] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseY,_mm_set_ps1(0.0f)));
		uiMask[2] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseZ,_mm_set_ps1(0.0f)));
		RAY_ORIENTATION eOrientation = MMM;
		unsigned int uiOrientation = 0;
		//oAnswer.m_sseData = _mm_set_ps1(0.0f);
		oAnswer.m_sseData = _mm_set_ps1(FLT_MAX);

		//Are all four X's in the positive or negative direction
		switch(uiMask[0])
		{
		case 15:	uiOrientation+=0x8;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer,oOrigin,oDirection,uiMask);	return;
		}

		//All are four Y's in the positive or negative direction
		switch(uiMask[1])
		{
		case 15:	uiOrientation+=0x4;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer,oOrigin,oDirection,uiMask);	return;
		}

		//Are all four Z's in the positive or negative direction
		switch(uiMask[2])
		{
		case 15:	uiOrientation+=0x2;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer,oOrigin,oDirection,uiMask);	return;
		}

		eOrientation = RAY_ORIENTATION(uiOrientation);

		NMath::CPacketVector left,right;
		left.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetX()),oOrigin.m_sseX);
		left.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetY()),oOrigin.m_sseY);
		left.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetZ()),oOrigin.m_sseZ);
		right.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetX()),oOrigin.m_sseX);
		right.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetY()),oOrigin.m_sseY);
		right.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetZ()),oOrigin.m_sseZ);

		__m128 c2[3],d2[3],cmp,sub,zero;
		zero = _mm_set_ps1(0.0f);

		switch(eOrientation)
		{
		case MMM:
			{	
				//See if origin is less then m_oLeftPoint, if so then the ray misses
				NMath::SVector128 ltX,ltY,ltZ;
				ltX.m_sseData = _mm_cmplt_ps(oOrigin.m_sseX,_mm_set_ps1(m_oLeftPoint.fGetX()));
				ltY.m_sseData = _mm_cmplt_ps(oOrigin.m_sseY,_mm_set_ps1(m_oLeftPoint.fGetY()));
				ltZ.m_sseData = _mm_cmplt_ps(oOrigin.m_sseZ,_mm_set_ps1(m_oLeftPoint.fGetZ()));
				
				if(_mm_movemask_ps(ltX.m_sseData)==0xF || _mm_movemask_ps(ltY.m_sseData)==0xF ||
					_mm_movemask_ps(ltZ.m_sseData)==0xF)
					return;

				c2[0] = _mm_mul_ps(oDirection.m_sseX,left.m_sseX);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,left.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,left.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmplt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xE)
						return;
				}

				c2[0] = _mm_mul_ps(oDirection.m_sseY,left.m_sseX);
				c2[1] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseX);
				c2[2] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseY);
				d2[0] = _mm_mul_ps(oDirection.m_sseX,right.m_sseY);
				d2[1] = _mm_mul_ps(oDirection.m_sseX,right.m_sseZ);
				d2[2] = _mm_mul_ps(oDirection.m_sseY,right.m_sseZ);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmple_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xE)
						return;
				}
			}break;
		case MMP:
			{
				cmp = _mm_cmplt_ps(oOrigin.m_sseX,_mm_set_ps1(m_oLeftPoint.fGetX()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmplt_ps(oOrigin.m_sseY,_mm_set_ps1(m_oLeftPoint.fGetY()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseZ,_mm_set_ps1(m_oRightPoint.fGetZ()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;

				c2[0] = _mm_mul_ps(oDirection.m_sseX,left.m_sseX);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,left.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,left.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseY);
				
				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmplt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xE)
						return;
				}
				
				c2[0] = _mm_mul_ps(oDirection.m_sseX,right.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,right.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,right.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,left.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmpgt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xE)
						return;
				}
			}break;
		case MPM:
			{
				return;
			}break;
		case MPP:
			{

				cmp = _mm_cmplt_ps(oOrigin.m_sseX,_mm_set_ps1(m_oLeftPoint.fGetX()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseY,_mm_set_ps1(m_oRightPoint.fGetY()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseZ,_mm_set_ps1(m_oRightPoint.fGetZ()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				/*
				if(oOrigin.fGetX() < m_oLeftPoint.fGetX())
					return false;
				if(oOrigin.fGetY() > m_oRightPoint.fGetY())
					return false;
				if(oOrigin.fGetZ() > m_oRightPoint.fGetZ())
					return false;
			
				CVector a,b;
				a.Sub(m_oLeftPoint,oOrigin);
				b.Sub(m_oRightPoint,oOrigin);
				*/

				c2[0] = _mm_mul_ps(oDirection.m_sseX,left.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,left.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,right.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,left.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmplt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
				/*
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*a.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;
				*/
				c2[0] = _mm_mul_ps(oDirection.m_sseX,right.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,right.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,left.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmpgt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
				/*
				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*b.fGetX()>0)
					return false;
				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;*/
			}break;
		case PMM:
			{
				return;
			}break;
		case PMP:
			{
				cmp = _mm_cmpgt_ps(oOrigin.m_sseX,_mm_set_ps1(m_oRightPoint.fGetX()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmplt_ps(oOrigin.m_sseY,_mm_set_ps1(m_oLeftPoint.fGetY()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseZ,_mm_set_ps1(m_oRightPoint.fGetZ()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				/*
								if(oOrigin.fGetX()>m_oRightPoint.fGetX())
					return false;
				if(oOrigin.fGetY()<m_oLeftPoint.fGetY())
					return false;
				if(oOrigin.fGetZ()>m_oRightPoint.fGetZ())
					return false;

				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);
				*/

				c2[0] = _mm_mul_ps(oDirection.m_sseX,right.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,right.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,left.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmplt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
				/*
				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*b.fGetX()<0)
					return false;
				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;
				*/
				c2[0] = _mm_mul_ps(oDirection.m_sseX,left.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,left.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,right.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,left.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmpgt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
				/*
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*a.fGetX()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;*/

			}break;
		case PPM:
			{
				return;
			}break;
		case PPP:
			{
				/*
				//if(oOrigin>m_oRightPoint)
				NMath::SVector128 ans;
				ans.m_sseData = _mm_cmpgt_ps(oOrigin.sseGetVector(),m_oRightPoint.sseGetVector());
				if(_mm_movemask_ps(ans.m_sseData))
					return false;
				
				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);
				*/
				cmp = _mm_cmpgt_ps(oOrigin.m_sseX,_mm_set_ps1(m_oRightPoint.fGetX()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseY,_mm_set_ps1(m_oRightPoint.fGetY()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				cmp = _mm_cmpgt_ps(oOrigin.m_sseZ,_mm_set_ps1(m_oRightPoint.fGetZ()));
				if(_mm_movemask_ps(cmp)==0xF)
					return;
				/*
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*b.fGetX()>0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;
				*/
				c2[0] = _mm_mul_ps(oDirection.m_sseX,left.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseY,left.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseX,left.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,right.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseY);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,right.m_sseX);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmpgt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
				/*
				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*a.fGetX()<0)
					return false;
				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;*/
				c2[0] = _mm_mul_ps(oDirection.m_sseX,right.m_sseY);
				c2[1] = _mm_mul_ps(oDirection.m_sseX,right.m_sseZ);
				c2[2] = _mm_mul_ps(oDirection.m_sseY,right.m_sseZ);
				d2[0] = _mm_mul_ps(oDirection.m_sseY,left.m_sseX);
				d2[1] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseX);
				d2[2] = _mm_mul_ps(oDirection.m_sseZ,left.m_sseY);

				for(int i=0;i<3;++i)
				{
					sub = _mm_sub_ps(c2[i],d2[i]);
					cmp = _mm_cmplt_ps(sub,zero);
					if(_mm_movemask_ps(cmp)==0xF)
						return;
				}
			}break;
		default:
			{
			};
		}
		oAnswer.m_sseData = _mm_set_ps1(1.0f);
	}

	template <class T>
	inline void TAabb<T>::DoesIntersectIndividual(SVector128& oAnswer,const CPacketVector& oOrigin,
		const CPacketVector& oDirection,unsigned int uiMask[])
	{
		/*
		unsigned int uiMask[3];
		uiMask[0] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseX,_mm_set_ps1(0.0f)));
		uiMask[1] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseY,_mm_set_ps1(0.0f)));
		uiMask[2] = _mm_movemask_ps(_mm_cmpge_ps(oDirection.m_sseZ,_mm_set_ps1(0.0f)));
		*/

		RAY_ORIENTATION eOrientation[4];
		eOrientation[0] = RAY_ORIENTATION((uiMask[0] & 0x8)+((uiMask[1] & 0x8)>>1)+
			((uiMask[2] & 0x8)>>2));
		eOrientation[1] = RAY_ORIENTATION((uiMask[0] & 0x4)+((uiMask[1] & 0x4)>>1)+
			((uiMask[2] & 0x4)>>2));
		eOrientation[2] = RAY_ORIENTATION((uiMask[0] & 0x2)+((uiMask[1] & 0x2)>>1)+
			((uiMask[2] & 0x2)>>2));
		eOrientation[3] = RAY_ORIENTATION((uiMask[0] & 0x1)+((uiMask[1] & 0x1)>>1)+
			((uiMask[3] & 0x1)>>2));	

		CVector vecOrigins[4],vecDirections[4];
		for(unsigned int ui=0;ui<4;++ui)
		{
			vecOrigins[ui].Set(oOrigin.m_fX[ui],oOrigin.m_fY[ui],oOrigin.m_fZ[ui]);
			vecDirections[ui].Set(oDirection.m_fX[ui],oDirection.m_fY[ui],oDirection.m_fZ[ui]);
			oAnswer.m_fData[ui]=float(bIntersects(vecOrigins[ui],vecDirections[ui],eOrientation[ui]));
		}
	}

	/*
	template <class T>
	inline void TAabb<T>::DoesIntersect(NMath::SVector128 &oAnswer, const NMath::CPacketVector &oOrigin, 
		const NMath::CPacketVector &oDirection)
	{
		CPacketVector oInverse;
		//oInverse.Div(_mm_set_ps1(1.0f),oDirection);
		oInverse.m_sseX = _mm_rcp_ps(oDirection.m_sseX);
		oInverse.m_sseY = _mm_rcp_ps(oDirection.m_sseY);
		oInverse.m_sseZ = _mm_rcp_ps(oDirection.m_sseZ);

		CPacketVector oBottom,oTop;
		oBottom.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetX()),oOrigin.m_sseX);
		oBottom.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetY()),oOrigin.m_sseY);
		oBottom.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetZ()),oOrigin.m_sseZ);
		oBottom.Mul(oInverse);
		
		oTop.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetX()),oOrigin.m_sseX);
		oTop.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetY()),oOrigin.m_sseY);
		oTop.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetZ()),oOrigin.m_sseZ);
		oTop.Mul(oInverse);

		__m128 sseTempX,sseTempY,sseTempZ;
		sseTempX = _mm_add_ps(
			_mm_and_ps(oTop.m_sseX,_mm_cmpgt_ps(oTop.m_sseX,oBottom.m_sseX)),
			_mm_and_ps(oBottom.m_sseX,_mm_cmpge_ps(oBottom.m_sseX,oTop.m_sseX)));
		sseTempY = _mm_add_ps(
			_mm_and_ps(oTop.m_sseY,_mm_cmpgt_ps(oTop.m_sseY,oBottom.m_sseY)),
			_mm_and_ps(oBottom.m_sseY,_mm_cmpge_ps(oBottom.m_sseY,oTop.m_sseY)));
		sseTempZ = _mm_add_ps(
			_mm_and_ps(oTop.m_sseZ,_mm_cmpgt_ps(oTop.m_sseZ,oBottom.m_sseZ)),
			_mm_and_ps(oBottom.m_sseZ,_mm_cmpge_ps(oBottom.m_sseZ,oTop.m_sseZ)));
		
		oBottom.m_sseX = _mm_add_ps(
			_mm_and_ps(oBottom.m_sseX,_mm_cmplt_ps(oBottom.m_sseX,oTop.m_sseX)),
			_mm_and_ps(oTop.m_sseX,_mm_cmple_ps(oTop.m_sseX,oBottom.m_sseX)));
		oBottom.m_sseY = _mm_add_ps(
			_mm_and_ps(oBottom.m_sseY,_mm_cmplt_ps(oBottom.m_sseY,oTop.m_sseY)),
			_mm_and_ps(oTop.m_sseY,_mm_cmple_ps(oTop.m_sseY,oBottom.m_sseY)));
		oBottom.m_sseZ = _mm_add_ps(
			_mm_and_ps(oBottom.m_sseZ,_mm_cmplt_ps(oBottom.m_sseZ,oTop.m_sseZ)),
			_mm_and_ps(oTop.m_sseZ,_mm_cmple_ps(oTop.m_sseZ,oBottom.m_sseZ)));

		oTop.m_sseX = sseTempX;
		oTop.m_sseY = sseTempY;
		oTop.m_sseZ = sseTempZ;


		NMath::SVector128 y0,yMask,z0,zMask,sseNear,sseFar;
		//find largest min
		yMask.m_sseData = _mm_cmpgt_ps(oBottom.m_sseY,oBottom.m_sseX);
		zMask.m_sseData = _mm_cmpgt_ps(oBottom.m_sseZ,oBottom.m_sseX);

		y0.m_sseData = _mm_add_ps(
			_mm_and_ps(oBottom.m_sseY,yMask.m_sseData),
			_mm_andnot_ps(yMask.m_sseData,oBottom.m_sseX));

		z0.m_sseData = _mm_add_ps(
			_mm_and_ps(oBottom.m_sseZ,zMask.m_sseData),
			_mm_andnot_ps(zMask.m_sseData,oBottom.m_sseX));

		yMask.m_sseData = _mm_cmpgt_ps(y0.m_sseData,z0.m_sseData);

		sseNear.m_sseData = _mm_add_ps(
			_mm_and_ps(y0.m_sseData,yMask.m_sseData),
			_mm_andnot_ps(yMask.m_sseData,z0.m_sseData));
		//find the smallest max
		yMask.m_sseData = _mm_cmplt_ps(oTop.m_sseY,oTop.m_sseX);
		zMask.m_sseData = _mm_cmplt_ps(oTop.m_sseZ,oTop.m_sseX);

		y0.m_sseData = _mm_add_ps(
			_mm_and_ps(oTop.m_sseY,yMask.m_sseData),
			_mm_andnot_ps(yMask.m_sseData,oTop.m_sseX));
		z0.m_sseData = _mm_add_ps(
			_mm_and_ps(oTop.m_sseZ,zMask.m_sseData),
			_mm_andnot_ps(zMask.m_sseData,oTop.m_sseX));
		yMask.m_sseData = _mm_cmplt_ps(y0.m_sseData,z0.m_sseData);
		
		sseFar.m_sseData = _mm_add_ps(
			_mm_and_ps(y0.m_sseData,yMask.m_sseData),
			_mm_andnot_ps(yMask.m_sseData,z0.m_sseData));

		oAnswer.m_sseData = _mm_cmple_ps(sseNear.m_sseData,sseFar.m_sseData);
	}*/
	
	/*
	Plucker Coordinates for a line L passing through points A and B:
	L0 = AxBy - BxAy
	L1 = AxBz - BxAz
	L2 = Ax - Bx
	L3 = AyBz - ByAz
	L4 = Az - Bz
	L5 = By - Ay
	
	So a Ray R would be expressed as Origin O + Direction D <Di, Dj, Dk>:
	R0 = OxDj - DiOy
	R1 = OxDk - DiOz
	R2 = -Di
	R3 = OyDk - DjOz
	R4 = -Dk
	R5 = Dj

	The surface normal N or the plane formed of the points A,B, and Origin O is:
	N = (A-O)X(B-O)

	Thus the function side(R,L) = -D.((A-O)X(B-O))
	(... some ugly math later...)
	side(R,L) = -DiL3 + DjL1 - DkL0 + R1L5 + R0L4 + R3L2
	side(O,D,A,B) = -Di(AyBZ-ByAz) + Dj(AxBz-BxAz) - Dk(AxBy-BxAy) + (OxDk-DiOz)(By-Ay) +
		(OxDj-DiOy)(Az-Bz) + (OyDk-DjOz)(Ax-Bx)
	*/
	template <class T>
	inline bool TAabb<T>::bIntersects(const NMath::CVector& oOrigin,const NMath::CVector& oDirection)
	{
		NMath::SVector128 signs;
		//Compare the direction of the ray, see which are in the positive direction
		signs.m_sseData = _mm_cmp_ge(oDirection.sseGetVector(),_mm_set_ps1(0.0f));
		//Since the ray is in 3-space, set the fourth value to 0
		signs.m_fData[3] = 0.0f;
		//The ray orientation can be determined using the move_mask command on the bit-mask created
		//by determining the orientation of the ray:
		//signs before move-mask: 0xFFFFFFFF||0x00000000||0xFFFFFFFF||0x00000000
		//signs after move-mask:  0x00000000||0x00000000||0x00000000||0x00000006
		return bIntersects(oOrigin,oDirection,RAY_ORIENTATION(_mm_movemask_ps(signs.m_sseData)));
	}

	template <class T>
	inline bool TAabb<T>::bIntersects(const CVector& oOrigin,const CVector& oDirection,RAY_ORIENTATION eOrientation)
	{
		switch(eOrientation)
		{
		case MMM:
			{
				NMath::SVector128 ans;
				//If the origin is less than the left point of the box, it can't possibly intersect
				ans.m_sseData = _mm_cmplt_ps(oOrigin.sseGetVector(),m_oLeftPoint.sseGetVector());
				ans.m_fData[4]=0x0;
				if(_mm_movemask_ps(ans.m_sseData))
					return false;
	
				CVector a,b;
				a.Sub(m_oLeftPoint,oOrigin);
				b.Sub(m_oRightPoint,oOrigin);

				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*b.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*b.fGetX()<0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()<0)
					return false;
	
				if(oDirection.fGetY()*a.fGetX()-oDirection.fGetX()*b.fGetY()<=0)
					return false;
				if(oDirection.fGetZ()*a.fGetX()-oDirection.fGetX()*b.fGetZ()<=0)
					return false;
				if(oDirection.fGetZ()*a.fGetY()-oDirection.fGetY()*b.fGetZ()<=0)
					return false;
			}break;
		case MMP:
			{
				/*
				if ((r->x < b->x0) || (r->y < b->y0) || (r->z > b->z1))
				return false;
				*/
				if(oOrigin.fGetX() < m_oLeftPoint.fGetX())
					return false;
				if(oOrigin.fGetY() < m_oLeftPoint.fGetY())
					return false;
				if(oOrigin.fGetZ() > m_oRightPoint.fGetZ())
					return false;
			/*
			float xa = b->x0 - r->x; 
			float ya = b->y0 - r->y; 
			float za = b->z0 - r->z; 
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;
			*/
				NMath::SVector128 a,b;
				a.m_sseData = _mm_sub_ps(m_oLeftPoint.sseGetVector(),oOrigin.sseGetVector());
				b.m_sseData = _mm_sub_ps(m_oRightPoint.sseGetVector(),oOrigin.sseGetVector());
			/*
			       c    d      e     f
			if(	(r->i * ya - r->j * xb < 0) ||
				(r->i * za - r->k * xa < 0) ||
				(r->j * za - r->k * ya < 0) ||
			*/
				NMath::SVector128 c,d,e,f;

				c.m_sseData = _mm_set_ps1(oDirection.fGetX());
				c.m_fData[2] = oDirection.fGetY();
				d.m_sseData = _mm_set_ps1(a.m_fData[2]);
				d.m_fData[0] = a.m_fData[1];
				e.m_sseData = _mm_set_ps1(oDirection.fGetZ());
				e.m_fData[0] = oDirection.fGetY();
				f.m_fData[0] = b.m_fData[0];
				f.m_fData[1] = a.m_fData[0];
				f.m_fData[2] = a.m_fData[1];

				c.m_sseData = _mm_mul_ps(c.m_sseData,d.m_sseData);
				d.m_sseData = _mm_mul_ps(e.m_sseData,f.m_sseData);
				e.m_sseData = _mm_sub_ps(c.m_sseData,d.m_sseData);
				f.m_sseData = _mm_cmplt_ps(e.m_sseData,_mm_set_ps1(0.0f));
				f.m_fData[3] = 0.0f;
				if(_mm_movemask_ps(f.m_sseData))
					return false;
			/*
				  c     d     e     f
				(r->i * yb - r->j * xa > 0) ||
				(r->i * zb - r->k * xb > 0) ||
				(r->j * zb - r->k * yb > 0))
				return false;

			return true;*/
				c.m_sseData = _mm_set_ps1(oDirection.fGetX());
				c.m_fData[2] = oDirection.fGetY();
				d.m_sseData = _mm_set_ps1(b.m_fData[2]);
				d.m_fData[0] = b.m_fData[1];
				e.m_sseData = _mm_set_ps1(oDirection.fGetZ());
				e.m_fData[0] = oDirection.fGetY();
				f.m_fData[0] = a.m_fData[0];
				f.m_fData[1] = b.m_fData[0];
				f.m_fData[2] = b.m_fData[1];
				c.m_sseData = _mm_mul_ps(c.m_sseData,d.m_sseData);
				d.m_sseData = _mm_mul_ps(e.m_sseData,f.m_sseData);
				e.m_sseData = _mm_sub_ps(c.m_sseData,d.m_sseData);
				f.m_sseData = _mm_cmpgt_ps(e.m_sseData,_mm_set_ps1(0.0f));
				f.m_fData[3] = 0.0f;
				if(_mm_movemask_ps(f.m_sseData))
					return false;
			}break;
		case MPM:
			{
				/*
				if ((r->x < b->x0) || (r->y > b->y1) || (r->z < b->z0))
				return false;
				*/
				if(oOrigin.fGetX()<m_oLeftPoint.fGetX())
					return false;
				if(oOrigin.fGetY()>m_oRightPoint.fGetY())
					return false;
				if(oOrigin.fGetZ()<m_oLeftPoint.fGetZ())
					return false;
				/*
			float xa = b->x0 - r->x; 
			float ya = b->y0 - r->y; 
			float za = b->z0 - r->z; 
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;
			*/
				NMath::SVector128 a,b;
				a.m_sseData = _mm_sub_ps(m_oLeftPoint.sseGetVector(),oOrigin.sseGetVector());
				b.m_sseData = _mm_sub_ps(m_oRightPoint.sseGetVector(),oOrigin.sseGetVector());
				/*
                   c     d     e     f
			if(	(r->i * ya - r->j * xa < 0) ||
				(r->i * za - r->k * xb < 0) ||
				(r->j * zb - r->k * yb < 0) ||
				*/
				NMath::SVector128 c,d,e,f;
				c.m_sseData = _mm_set_ps1(oDirection.fGetX());
				c.m_fData[2] = oDirection.fGetY();
				d.m_fData[0] = a.m_fData[1];
				d.m_fData[1] = a.m_fData[2];
				d.m_fData[2] = b.m_fData[2];
				e.m_sseData = _mm_set_ps1(oDirection.fGetZ());
				e.m_fData[0] = oDirection.fGetY();
				f.m_fData[0] = a.m_fData[0];
				f.m_fData[1] = b.m_fData[0];
				f.m_fData[2] = b.m_fData[1];
				c.m_sseData = _mm_mul_ps(c.m_sseData,d.m_sseData);
				d.m_sseData = _mm_mul_ps(e.m_sseData,f.m_sseData);
				e.m_sseData = _mm_sub_ps(c.m_sseData,d.m_sseData);
				f.m_sseData = _mm_cmplt_ps(e.m_sseData,_mm_set_ps1(0.0f));
				f.m_fData[3]=0.0f;
				if(_mm_movemask_ps(f.m_sseData))
					return false;
				/*
				  c     d     e      f
				(r->i * yb - r->j * xb > 0) ||
				(r->i * zb - r->k * xa > 0) ||
				(r->j * za - r->k * ya > 0))
				return false;

			return true;*/
				c.m_sseData = _mm_set_ps1(oDirection.fGetX());
				c.m_fData[2] = oDirection.fGetY();
				d.m_fData[0] = b.m_fData[1];
				d.m_fData[1] = b.m_fData[2];
				d.m_fData[2] = a.m_fData[2];
				e.m_sseData = _mm_set_ps1(oDirection.fGetZ());
				e.m_fData[0] = oDirection.fGetY();
				f.m_fData[0] = b.m_fData[0];
				f.m_fData[1] = a.m_fData[0];
				f.m_fData[2] = a.m_fData[1];
				c.m_sseData = _mm_mul_ps(c.m_sseData,e.m_sseData);
				d.m_sseData = _mm_mul_ps(e.m_sseData,f.m_sseData);
				e.m_sseData = _mm_sub_ps(c.m_sseData,d.m_sseData);
				f.m_sseData = _mm_cmpgt_ps(e.m_sseData,_mm_set_ps1(0.0f));
				f.m_fData[3]=0.0f;
				if(_mm_movemask_ps(f.m_sseData))
					return false;
			}break;
		case MPP:
			{
				/*
				if ((r->x < b->x0) || (r->y > b->y1) || (r->z > b->z1))
				return false;*/
				if(oOrigin.fGetX() < m_oLeftPoint.fGetX())
					return false;
				if(oOrigin.fGetY() > m_oRightPoint.fGetY())
					return false;
				if(oOrigin.fGetZ() > m_oRightPoint.fGetZ())
					return false;
/*
			float xa = b->x0 - r->x; 
			float ya = b->y0 - r->y; 
			float za = b->z0 - r->z; 
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;
			*/
				CVector a,b;
				a.Sub(m_oLeftPoint,oOrigin);
				b.Sub(m_oRightPoint,oOrigin);
				/*
				  c     d     e     f
			if(	(r->i * ya - r->j * xa < 0) ||
				(r->i * za - r->k * xa < 0) ||
				(r->j * zb - r->k * ya < 0) ||

				(r->i * yb - r->j * xb > 0) ||
				(r->i * zb - r->k * xb > 0) ||
				(r->j * za - r->k * yb > 0))
				return false;
			return true;*/
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*a.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;

				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*b.fGetX()>0)
					return false;
				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;
			}break;
		case PMM:
			{
					/*
					if ((r->x > b->x1) || (r->y < b->y0) || (r->z < b->z0))
				return false;
			*/
				if(oOrigin.fGetX() > m_oRightPoint.fGetX())
					return false;
				if(oOrigin.fGetY() < m_oLeftPoint.fGetY())
					return false;
				if(oOrigin.fGetZ() < m_oLeftPoint.fGetZ())
					return false;
				/*
			float xa = b->x0 - r->x; 
			float ya = b->y0 - r->y; 
			float za = b->z0 - r->z; 
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;
			*/
				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);
				/*
			if(	(r->i * yb - r->j * xb < 0) ||
				(r->i * ya - r->j * xa > 0) ||
				(r->i * za - r->k * xa > 0) ||
				
				(r->i * zb - r->k * xb < 0) ||
				(r->j * za - r->k * yb < 0) ||
				(r->j * zb - r->k * ya > 0))
				return false;

			return true;*/
			
				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*b.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*a.fGetX()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*a.fGetX()>0)
					return false;

				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*b.fGetX()<0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*a.fGetY()>0)
					return false;
			}break;
		case PMP:
			{
				if(oOrigin.fGetX()>m_oRightPoint.fGetX())
					return false;
				if(oOrigin.fGetY()<m_oLeftPoint.fGetY())
					return false;
				if(oOrigin.fGetZ()>m_oRightPoint.fGetZ())
					return false;

				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);

				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*b.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*a.fGetX()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;

				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;
			}break;
		case PPM:
			{
				/*
				if ((r->x > b->x1) || (r->y > b->y1) || (r->z < b->z0))
				return false;

			float xa = b->x0 - r->x; 
			float ya = b->y0 - r->y; 
			float za = b->z0 - r->z; 
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;

			if(	(r->i * yb - r->j * xa < 0) ||
				(r->i * ya - r->j * xb > 0) ||
				(r->i * za - r->k * xa > 0) ||
				
				(r->i * zb - r->k * xb < 0) ||
				(r->j * zb - r->k * yb < 0) ||
				(r->j * za - r->k * ya > 0))
				return false;*/
				if(oOrigin.fGetX()>m_oRightPoint.fGetX())
					return false;
				if(oOrigin.fGetY()>m_oRightPoint.fGetY())
					return false;
				if(oOrigin.fGetZ()<m_oLeftPoint.fGetZ())
					return false;

				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);

				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*a.fGetX()<0)
					return false;
				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*b.fGetX()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*a.fGetX()>0)
					return false;

				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*b.fGetX()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*b.fGetY()<0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*a.fGetY()>0)
					return false;
			}break;
		case PPP:
			{
				//if(oOrigin>m_oRightPoint)
				NMath::SVector128 ans;
				ans.m_sseData = _mm_cmpgt_ps(oOrigin.sseGetVector(),m_oRightPoint.sseGetVector());
				if(_mm_movemask_ps(ans.m_sseData))
					return false;
				
				CVector a;
				a.Sub(m_oLeftPoint,oOrigin);
				CVector b;
				b.Sub(m_oRightPoint,oOrigin);

				if(oDirection.fGetX()*a.fGetY()-oDirection.fGetY()*b.fGetX()>0)
					return false;
				if(oDirection.fGetY()*a.fGetZ()-oDirection.fGetZ()*b.fGetY()>0)
					return false;
				if(oDirection.fGetX()*a.fGetZ()-oDirection.fGetZ()*b.fGetX()>0)
					return false;
				
				if(oDirection.fGetX()*b.fGetY()-oDirection.fGetY()*a.fGetX()<0)
					return false;
				if(oDirection.fGetX()*b.fGetZ()-oDirection.fGetZ()*a.fGetX()<0)
					return false;
				if(oDirection.fGetY()*b.fGetZ()-oDirection.fGetZ()*a.fGetY()<0)
					return false;
				
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}

	/*
	template <class T>
	inline bool TAabb<T>::bIntersects(const NMath::CVector &oOrigin, const NMath::CVector &oDirection)
	{
		CVector oInverse;
		oInverse.SetX(oDirection.fGetX() ? 1.0f/oDirection.fGetX() : NMath::g_fMaxRay);
		oInverse.SetY(oDirection.fGetY() ? 1.0f/oDirection.fGetY() : NMath::g_fMaxRay);
		oInverse.SetZ(oDirection.fGetZ() ? 1.0f/oDirection.fGetZ() : NMath::g_fMaxRay);

		CVector oBottom,oTop;

		oBottom.Sub(m_oLeftPoint,oOrigin);
		oBottom.Mul(oBottom,oInverse);
		oTop.Sub(m_oRightPoint,oOrigin);
		oTop.Mul(oTop,oInverse);
		
		float fTemp;
		//Set min/max
		if(oTop.fGetX() >= oBottom.fGetX());
		else
		{	
			fTemp = oTop.fGetX();
			oTop.SetX(oBottom.fGetX());
			oBottom.SetX(fTemp);
		}	
		
		if(oTop.fGetY()>=oBottom.fGetY());
		else
		{
			fTemp = oTop.fGetY();
			oTop.SetY(oBottom.fGetY());
			oBottom.SetY(fTemp);
		}

		if(oTop.fGetZ()>=oBottom.fGetZ());
		else
		{
			fTemp = oTop.fGetZ();
			oTop.SetZ(oBottom.fGetZ());
			oBottom.SetZ(fTemp);
		}

		//find largest min and smallest max
		float fy0 = (oBottom.fGetX() > oBottom.fGetY()) ? oBottom.fGetX() : oBottom.fGetY();
		float fz0 = (oBottom.fGetX() > oBottom.fGetZ()) ? oBottom.fGetX() : oBottom.fGetZ();
		//max
		float fNear = (fy0 > fz0) ? fy0 : fz0;
		
		fy0 = (oTop.fGetX() > oTop.fGetY()) ? oTop.fGetY() : oTop.fGetX();
		fz0 = (oTop.fGetX() > oTop.fGetZ()) ? oTop.fGetZ() : oTop.fGetX();
		//min
		float fFar = (fy0 < fz0) ? fy0 : fz0;

		if(fNear <= fFar)
			return true;
		else
			return false;
	}*/
}

#endif