#ifndef CRAYTRACER_H
#define CRAYTRACER_H

#include <math.h>
#include "CVector.h"
#include "TColor.h"
#include "CPlane.h"
#include "CPointLight.h"
//#include "CPolygon.h"
#include "CCamera.h"
#include "CNaiveSearch.h"
#include "CBihTree.h"

#define SSEMADNESS

namespace NRayTracer
{
	//const float g_fMaxRayLength = 1000.0f;

	class CRayTracer
	{
	public:
		bool bCanInitialize(unsigned int uiWidth,unsigned int uiHeight,CBihTree* pSearch);
		
		void Render(NUtility::TArray2D< NGraphics::TColor<float> >& displayBuffer);
		//void Render(NGraphics::TColor<float>* pData);
		
		CCamera		m_oCamera;
		CPointLight	m_oLight;
	private:
		float	fGetPrimaryIntersection(const NMath::CVector& oOrigin,const NMath::CVector& oDirection);
		//bool	bIsShadowed(const NMath::CVector& oPoint,const NMath::CVector& LightRay);
		//void GetStartingVector(NMath::CPacketVector& oStart);
		float fGetViewPlaneDistance();

		unsigned int	m_uiWidth;
		unsigned int	m_uiHeight;
	
		
		//CNaiveSearch*	m_pSearch;
		CBihTree* m_pSearch;
		NUtility::TAlignedArray< NMath::CPacketVector> m_oRays;
	};

	bool CRayTracer::bCanInitialize(unsigned int uiWidth,unsigned int uiHeight,CBihTree* pSearch)
	{
		using NMath::CVector;
		
		if(!pSearch)
			return false;
		m_pSearch = pSearch;
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;
		
		//Because rays are in packets of 2x2 due to SSE optimizations, we allocate width/2 and height/2
		m_oRays.Allocate((uiWidth/2)*(uiHeight/2));
		NMath::CPacketVector oPacket;
		float fDistance = fGetViewPlaneDistance();
		float fWidth	= float(uiWidth);
		float fHeight	= float(uiHeight);
		unsigned int uiIndex=0;

		for(float fy=(fHeight/2.0f);fy>-(fHeight/2.0f);fy-=2.0f)
		{
			for(float fx=-(fWidth/2.0f);fx<(fWidth/2.0f);fx+=2.0f)
			{
				oPacket.m_sseZ = _mm_set_ps1(fDistance);

				oPacket.m_fY[0]=oPacket.m_fY[1]=fy;
				oPacket.m_fY[2]=oPacket.m_fY[3]=fy+1.0f;
				
				oPacket.m_fX[0]=oPacket.m_fX[2]=fx;
				oPacket.m_fX[1]=oPacket.m_fX[3]=fx+1.0f;
				
				oPacket.Normalize();
				m_oRays[uiIndex++].Set(oPacket);
			}
		}

		return true;
	};
/*
	inline float CRayTracer::fGetPrimaryIntersection(const NMath::CVector& oOrigin,const NMath::CVector& oDirection)
	{
		return m_oPoly.fGetIntersection(oOrigin,oDirection);
		//return m_oPlane.fGetIntersection(oOrigin,oDirection);
	}*/

#ifdef SSEMADNESS

	//inline void CRayTracer::Render(NGraphics::TColor<float>* pData)
	inline void CRayTracer::Render(NUtility::TArray2D< NGraphics::TColor<float> >& displayBuffer)
	{
		NMath::CPacketVector oIntersection;
		NMath::CPacketVector oOrigin;
		oOrigin.Set(m_oCamera.oGetOrigin().fGetX(),m_oCamera.oGetOrigin().fGetY(),m_oCamera.oGetOrigin().fGetZ());
		NMath::SVector128 oResults,oDot;
		NRayTracer::SPolyIndices oIndices;
		
		//TEMPORARY
		for(unsigned int ui=0;ui<displayBuffer.uiGetWidth()*m_uiHeight;++ui)
			displayBuffer[ui].Set(NGraphics::TColor<float>(0.0f,0.0f,0.0f));

		for(unsigned int uiy=0;uiy<m_uiHeight;uiy+=2)
		{
			for(unsigned int uix=0;uix<m_uiWidth;uix+=2)
			{
				unsigned int ui=uiy*m_uiWidth+uix;
				unsigned int uiRayIndex = ((uiy/2)*(m_uiWidth/2))+(uix/2);

				m_pSearch->GetPrimaryIntersection(oResults,oIndices,oOrigin,m_oRays[uiRayIndex]);
				if(_mm_movemask_ps(_mm_cmplt_ps(oResults.m_sseData,_mm_set_ps1(FLT_MAX))))
				{
					//oIntersection.Set(m_oRays[uiRayIndex]);
					oIntersection.Mul(oResults.m_sseData,m_oRays[uiRayIndex]);
					oIntersection.Add(oOrigin);
				
					//Get surface normal
					NMath::CPacketVector oPacketNormal;
					NMath::CVector oNormal[4];
					for(unsigned int ui=0;ui<4;++ui)
					{
						m_pSearch->m_oPolyList.GetPlaneNormal(oNormal[ui],oIndices.m_uiPoly[ui]);
					}
					oPacketNormal.m_sseX = _mm_set_ps(oNormal[0].m_fX,oNormal[1].m_fX,oNormal[2].m_fX,oNormal[3].m_fX);
					oPacketNormal.m_sseY = _mm_set_ps(oNormal[0].m_fY,oNormal[1].m_fY,oNormal[2].m_fY,oNormal[3].m_fY);
					oPacketNormal.m_sseZ = _mm_set_ps(oNormal[0].m_fZ,oNormal[1].m_fZ,oNormal[2].m_fZ,oNormal[3].m_fZ);
					
					//Calculate dot product of light and surface normal
					m_oLight.GetLightDotProduct(oDot,oIntersection,oPacketNormal);
					
					//Make sure it's greater than 0
					oDot.m_sseData = _mm_and_ps(oDot.m_sseData,_mm_cmpgt_ps(oDot.m_sseData,_mm_set_ps1(0.0f)));
				
					unsigned int uiIndex = uiy*displayBuffer.uiGetWidth()+uix;
					displayBuffer[uiIndex].Mul(oDot.m_fData[0],m_oLight.oGetColor());
					displayBuffer[uiIndex+1].Mul(oDot.m_fData[1],m_oLight.oGetColor());
					displayBuffer[uiIndex+displayBuffer.uiGetWidth()].Mul(oDot.m_fData[2],m_oLight.oGetColor());
					displayBuffer[uiIndex+displayBuffer.uiGetWidth()+1].Mul(oDot.m_fData[3],m_oLight.oGetColor());
				}
			}
		}
	}
	/*
	inline void CRayTracer::Render(NGraphics::TColor<float> *pData)
	{
		using namespace NMath;
		using NGraphics::TColor;
		
		//DEBUG: Create Light
		//m_oLight.Set(CVector(0.0f,0.0f,10.0f),TColor<float>(0.0f,100.0f,5.0f));
		//DEBUG: Create plane
		//CPlane oPlane(CVector(0.0f,0.0f,0.0f),CVector(0.0f,0.0f,-1.0f));
		
		//Screen height divided by two because we are tracing in packets of 4
		unsigned int uiHeight = m_uiHeight/2;
		unsigned int uiWidth = m_uiWidth/2;

		//Parallel Projection: where to start from
		float fStartY = float(m_uiHeight)/2.0f;
		float fStartX = -float(m_uiWidth)/2.0f;

		//Ray to trace, set according to the look-at vector of the camera
		CPacketVector oOrigin;
		oOrigin.Set(m_oCamera.oGetOrigin().fGetX(),m_oCamera.oGetOrigin().fGetY(),m_oCamera.oGetOrigin().fGetZ());
		CPacketVector oDirection;
		oDirection.Set(m_oCamera.oGetDirection().fGetX(),m_oCamera.oGetDirection().fGetY(),m_oCamera.oGetDirection().fGetZ());
		CPacketVector oCenter,oCurrent;
		oCenter.Mul(_mm_set_ps1(fGetViewPlaneDistance()),oDirection);
		CPacketVector oX,oY;

		//Generic variable for storing Ray distance and other things
		SVector128 oResults,oResults2;
		//float fy=fStartY-2.0f;
		float fy=fStartY;
		
		for(unsigned int uiy=0;uiy<uiHeight;++uiy,fy-=2.0f)
		{		
			float fx=fStartX;
			oY.Set(m_oCamera.oGetUp().fGetX(),m_oCamera.oGetUp().fGetY(),m_oCamera.oGetUp().fGetZ());
			oY.Mul(_mm_set_ps(fy,fy,fy-1.0f,fy-1.0f));

			for(unsigned int uix=0;uix<uiWidth;++uix,fx+=2.0f)
			{
				oX.Set(m_oCamera.oGetRight().fGetX(),m_oCamera.oGetRight().fGetY(),m_oCamera.oGetRight().fGetZ());
				oX.Mul(_mm_set_ps(-fx,-fx-1.0f,-fx,-fx-1.0f));
				//Index of the first pixel
				unsigned int uiIndex = ((uiy<<1)*m_uiWidth)+(uix<<1);
				NGraphics::TColor<float> oBlank(0.0f,0.0f,0.0f); 
				pData[uiIndex]=oBlank;
				pData[uiIndex+1]=oBlank;
				pData[uiIndex+m_uiWidth]=oBlank;
				pData[uiIndex+m_uiWidth+1]=oBlank;

				
				oCurrent.Add(oX,oY);
				oCurrent.Add(oCenter);
				oCurrent.Normalize();

				NRayTracer::SPolyIndices oPolys;
				m_pSearch->GetPrimaryIntersection(oResults,oPolys,oOrigin,oCurrent);

				//Look into checking signs later on
				//if(oResults.m_fData[0]>0.0f && oResults.m_fData[1]>0.0f && 
				//	oResults.m_fData[2]>0.0f && oResults.m_fData[3]>0.0f)
				int iMask = _mm_movemask_ps(_mm_cmpgt_ps(oResults.m_sseData,_mm_set_ps1(0.0f)));
				//if(oResults.m_fData[0]>0.0f || oResults.m_fData[1]>0.0f || oResults.m_fData[2]>0.0f
				//	|| oResults.m_fData[3]>0.0f)
				if(iMask)
				{

					CPacketVector oIntersection(oDirection);
					oIntersection.Mul(oResults.m_sseData,oIntersection);
					oIntersection.Add(oOrigin);

					CPacketVector	oSurfaceNormal;
					CVector			oNormal[4];
					//DEBUG code
					m_pSearch->m_oPolyList.GetPlaneNormal(oNormal[0],oPolys.m_uiPolyA);
					m_pSearch->m_oPolyList.GetPlaneNormal(oNormal[1],oPolys.m_uiPolyB);
					m_pSearch->m_oPolyList.GetPlaneNormal(oNormal[2],oPolys.m_uiPolyC);
					m_pSearch->m_oPolyList.GetPlaneNormal(oNormal[3],oPolys.m_uiPolyD);
					
					oSurfaceNormal.m_sseX = _mm_set_ps(oNormal[0].fGetX(),oNormal[1].fGetX(),oNormal[2].fGetX(),oNormal[3].fGetX());
					oSurfaceNormal.m_sseY = _mm_set_ps(oNormal[0].fGetY(),oNormal[1].fGetY(),oNormal[2].fGetY(),oNormal[3].fGetY());
					oSurfaceNormal.m_sseZ = _mm_set_ps(oNormal[0].fGetZ(),oNormal[1].fGetZ(),oNormal[2].fGetZ(),oNormal[3].fGetZ());

					m_oLight.GetLightDotProduct(oResults2,oIntersection,oSurfaceNormal);
					

					CPacketVector oColor;
					oColor.m_sseX = _mm_set_ps1(m_oLight.oGetColor().GetRed());
					oColor.m_sseY = _mm_set_ps1(m_oLight.oGetColor().GetGreen());
					oColor.m_sseZ = _mm_set_ps1(m_oLight.oGetColor().GetBlue());
					
					oColor.m_sseX = _mm_mul_ps(oColor.m_sseX,oResults2.m_sseData);
					oColor.m_sseY = _mm_mul_ps(oColor.m_sseY,oResults2.m_sseData);
					oColor.m_sseZ = _mm_mul_ps(oColor.m_sseZ,oResults2.m_sseData);
					oColor.m_sseX = _mm_and_ps(oColor.m_sseX,_mm_cmpgt_ps(oColor.m_sseX,_mm_set_ps1(0.0f)));
					oColor.m_sseY = _mm_and_ps(oColor.m_sseY,_mm_cmpgt_ps(oColor.m_sseY,_mm_set_ps1(0.0f)));
					oColor.m_sseZ = _mm_and_ps(oColor.m_sseZ,_mm_cmpgt_ps(oColor.m_sseZ,_mm_set_ps1(0.0f)));
					
					TColor<float> tColor(oColor.m_fX[0],oColor.m_fY[0],oColor.m_fZ[0]);
					pData[uiIndex]=tColor;
					tColor.Set(oColor.m_fX[1],oColor.m_fY[1],oColor.m_fZ[1]);
					pData[uiIndex+1]=tColor;
					tColor.Set(oColor.m_fX[2],oColor.m_fY[2],oColor.m_fZ[2]);
					pData[uiIndex+m_uiWidth]=tColor;
					tColor.Set(oColor.m_fX[3],oColor.m_fY[3],oColor.m_fZ[3]);
					pData[uiIndex+m_uiWidth+1]=tColor;
				}
			}
		}
	}
	*/
#else
	inline void CRayTracer::Render(NGraphics::TColor<float>* pData)
	{
		using NMath::CVector;
		using NMath::CPlane;
		using NGraphics::TColor;

		CVector oOrigin(-float(m_uiWidth/2),float(m_uiHeight/2),-2.0f);
		//CVector oRay(0.0f,0.0f,1.0f);
		
		CPointLight	oLight(CVector(0.0f,0.0f,5.0f),TColor<float>(0.0f,0.0f,10.0f));
		//CPlane oPlane(CVector(0.0f,0.0f,0.0f),CVector(0.0f,0.0f,-1.0f));

		for(unsigned int uiy=0;uiy<m_uiHeight;++uiy)
		{
			for(unsigned int uix=0;uix<m_uiWidth;++uix)
			{
				float fDistance = fGetPrimaryIntersection(oOrigin,m_oCamera.oGetDirection());
				//float fDistance = oPlane.fGetIntersection(oOrigin,m_oCamera.oGetDirection());

				if(fDistance > 0.0f)
				{
					//Get dot product of the surface normal and the light ray
					//float fDot = oLight.oGetDirection().fDot(m_oPlane.oGetNormal());
					//float fDot = oLight.oGetLightRay(fDistance*oRay).fDot(m_oPlane.oGetNormal());
					
					CVector oIntersection;
					oIntersection.Mul(fDistance,m_oCamera.oGetDirection());
					oIntersection.Add(oOrigin,oIntersection);
					//float fDot = -oLight.oGetLightRay(oIntersection).fDot(m_oPoly.oGetPlaneNormal());
					CVector oLightRay;
					oLight.GetLightRay(oLightRay,oIntersection);					
					float fDot = -oLightRay.fDot(m_oPoly.oGetPlaneNormal());
					
					//CColor oPixel;
					TColor<float> oPixel;
					oPixel.SetRed(oLight.oGetColor().GetRed()*fDot);
					oPixel.SetGreen(oLight.oGetColor().GetGreen()*fDot);
					oPixel.SetBlue(oLight.oGetColor().GetBlue()*fDot);
					pData[uiy*m_uiWidth+uix]=oPixel;
					//pData[uiy*m_uiWidth+uix]=NGraphics::CPixel(1.0f,1.0f,1.0f);
				}
				else
					pData[uiy*m_uiWidth+uix]=NGraphics::TColor<float>(0.0f,0.0f,0.0f);
				oOrigin.SetX(oOrigin.fGetX()+1.0f);
			}
			oOrigin.SetX(-float(m_uiWidth)*0.5f);
			oOrigin.SetY(oOrigin.fGetY()-1.0f);
		}
	};
#endif

	inline float CRayTracer::fGetViewPlaneDistance()
	{
		const float pi = 3.141592654f;
		float fScrnX = float(m_uiWidth)/2.0f;
		float fThetaX = (pi-m_oCamera.fGetViewAngle())/2.0f;
		return fScrnX*fThetaX;
	}
/*
	inline void CRayTracer::GetStartingVector(NMath::CPacketVector &oStart)
	{
		const float pi = 3.141592654f;
		float fScrnX = float(m_uiWidth)/2.0f;
		float fThetaX = (pi-m_oCamera.fGetViewAngle())/2.0f;
		float fDistance = fScrnX*fThetaX;
		NMath::CPacketVector x,y,z;
		z.m_sseX = _mm_set_ps1(m_oCamera.oGetDirection().fGetX());
		z.m_sseY = _mm_set_ps1(m_oCamera.oGetDirection().fGetY());
		z.m_sseZ = _mm_set_ps1(m_oCamera.oGetDirection().fGetZ());
		z.Mul(_mm_set_ps1(fDistance),z);

		y.m_sseX = _mm_set_ps1(m_oCamera.oGetUp().fGetX());
		y.m_sseY = _mm_set_ps1(m_oCamera.oGetUp().fGetY());
		y.m_sseZ = _mm_set_ps1(m_oCamera.oGetUp().fGetZ());
		y.Mul(_mm_set_ps1(float(m_uiHeight)/2.0f),y);

		x.m_sseX = _mm_set_ps1(m_oCamera.oGetRight().fGetX());
		x.m_sseY = _mm_set_ps1(m_oCamera.oGetRight().fGetY());
		x.m_sseZ = _mm_set_ps1(m_oCamera.oGetRight().fGetZ());
		x.Mul(_mm_set_ps1(float(m_uiWidth)/2.0f),x);

		oStart.Add(x,y);
		oStart.Add(oStart,z);
	}*/
}

#endif