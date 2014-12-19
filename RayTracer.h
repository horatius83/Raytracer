#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <math.h>
#include "Vector.h"
#include "Color.h"
#include "Plane.h"
#include "PointLight.h"
#include "Camera.h"
#include "NaiveSearch.h"
#include "BihTree.h"

#define SSEMADNESS

namespace RayTracer
{
	class RayTracer
	{
	public:
		bool bCanInitialize(unsigned int uiWidth,unsigned int uiHeight,BihTree* pSearch);
		
		void Render(Utility::Array2D< Graphics::Color<float> >& displayBuffer);
		
		Camera		m_oCamera;
		PointLight	m_oLight;
	private:
		float	fGetPrimaryIntersection(const Math::Vector& oOrigin,const Math::Vector& oDirection);
		float fGetViewPlaneDistance();

		unsigned int	m_uiWidth;
		unsigned int	m_uiHeight;
	
		BihTree* m_pSearch;
		Utility::AlignedArray< Math::PacketVector> m_oRays;
	};

	bool RayTracer::bCanInitialize(unsigned int uiWidth,unsigned int uiHeight,BihTree* pSearch)
	{
		using Math::Vector;
		
		if(!pSearch)
			return false;
		m_pSearch = pSearch;
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;
		
		//Because rays are in packets of 2x2 due to SSE optimizations, we allocate width/2 and height/2
		m_oRays.Allocate((uiWidth/2)*(uiHeight/2));
		Math::PacketVector oPacket;
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

#ifdef SSEMADNESS

	//inline void RayTracer::Render(Graphics::Color<float>* pData)
	inline void RayTracer::Render(Utility::Array2D< Graphics::Color<float> >& displayBuffer)
	{
		Math::PacketVector oIntersection;
		Math::PacketVector oOrigin;
		oOrigin.Set(m_oCamera.oGetOrigin().fGetX(),m_oCamera.oGetOrigin().fGetY(),m_oCamera.oGetOrigin().fGetZ());
		Math::Vector128 oResults,oDot;
		PolyIndices oIndices;
		
		//TEMPORARY
		for(unsigned int ui=0;ui<displayBuffer.uiGetWidth()*m_uiHeight;++ui)
			displayBuffer[ui].Set(Graphics::Color<float>(0.0f,0.0f,0.0f));

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
					Math::PacketVector oPacketNormal;
					Math::Vector oNormal[4];
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
#else
	inline void RayTracer::Render(Graphics::Color<float>* pData)
	{
		using Math::Vector;
		using Math::Plane;
		using Graphics::Color;

		Vector oOrigin(-float(m_uiWidth/2),float(m_uiHeight/2),-2.0f);
		//Vector oRay(0.0f,0.0f,1.0f);
		
		PointLight	oLight(Vector(0.0f,0.0f,5.0f),TColor<float>(0.0f,0.0f,10.0f));
		//Plane oPlane(Vector(0.0f,0.0f,0.0f),Vector(0.0f,0.0f,-1.0f));

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
					
					Vector oIntersection;
					oIntersection.Mul(fDistance,m_oCamera.oGetDirection());
					oIntersection.Add(oOrigin,oIntersection);
					//float fDot = -oLight.oGetLightRay(oIntersection).fDot(m_oPoly.oGetPlaneNormal());
					Vector oLightRay;
					oLight.GetLightRay(oLightRay,oIntersection);					
					float fDot = -oLightRay.fDot(m_oPoly.oGetPlaneNormal());
					
					//CColor oPixel;
					TColor<float> oPixel;
					oPixel.SetRed(oLight.oGetColor().GetRed()*fDot);
					oPixel.SetGreen(oLight.oGetColor().GetGreen()*fDot);
					oPixel.SetBlue(oLight.oGetColor().GetBlue()*fDot);
					pData[uiy*m_uiWidth+uix]=oPixel;
					//pData[uiy*m_uiWidth+uix]=Graphics::CPixel(1.0f,1.0f,1.0f);
				}
				else
					pData[uiy*m_uiWidth+uix]=Graphics::Color<float>(0.0f,0.0f,0.0f);
				oOrigin.SetX(oOrigin.fGetX()+1.0f);
			}
			oOrigin.SetX(-float(m_uiWidth)*0.5f);
			oOrigin.SetY(oOrigin.fGetY()-1.0f);
		}
	};
#endif

	inline float RayTracer::fGetViewPlaneDistance()
	{
		const float pi = 3.141592654f;
		float fScrnX = float(m_uiWidth)/2.0f;
		float fThetaX = (pi-m_oCamera.fGetViewAngle())/2.0f;
		return fScrnX*fThetaX;
	}
}

#endif