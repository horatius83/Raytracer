#ifndef CDIRECTX9_H
#define CDIRECTX9_H

#include <d3d9.h>
//#include <d3dx9.h>
//#include <D3dx9tex.h>
#pragma comment( lib, "d3d9.lib" )
//#pragma comment( lib, "d3dx9.lib")

//#include "CPixel.h"
#include "TColor.h"
#include "CHtmlError.h"
#include "TArray2D.h"

namespace NSystem
{	
	struct STextureVertex
	{
		float x,y,z;
		float tu,tv;
		static inline DWORD dwGetCustomFVF(){return D3DFVF_XYZ | D3DFVF_TEX1;};
	};

	class CDirectX9
	{
	public:
		CDirectX9() : m_pD3D(NULL), m_pDevice(NULL), m_pVB(NULL),m_pTexture(NULL){};
		~CDirectX9()
		{
			Shutdown();
		}
		bool bCanInitialize(HWND hWnd,unsigned int uiWidth,unsigned int uiHeight,bool bWindowed=true);
		
		/*NGraphics::TColor<float>* pLockDisplay();
		unsigned int uiGetDisplayPitch();*/
		bool bLockDisplay(NUtility::TArray2D< NGraphics::TColor<float> >& tDisplay);
		void UnlockDisplay();

		void Render();
		void Shutdown();
	private:
		bool CanInitVertexBuffer();
		bool CanInitTexture();
		//void Draw();

		LPDIRECT3D9             m_pD3D;
		LPDIRECT3DDEVICE9       m_pDevice; 
		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DTEXTURE9		m_pTexture;

		unsigned int			m_uiWidth;
		unsigned int			m_uiHeight;
	};

	bool CDirectX9::bCanInitialize(HWND hWnd,unsigned int uiWidth,unsigned int uiHeight,bool bWindowed)
	{
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if(m_pD3D)
		{
			D3DPRESENT_PARAMETERS d3dpp;
			ZeroMemory(&d3dpp,sizeof(d3dpp));
			d3dpp.Windowed = bWindowed;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.BackBufferFormat = bWindowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
			d3dpp.BackBufferWidth = uiWidth;
			d3dpp.BackBufferHeight = uiHeight;
			d3dpp.BackBufferCount = 1;
			d3dpp.EnableAutoDepthStencil = true;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			d3dpp.PresentationInterval=bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;

			HRESULT hResult;

			if(SUCCEEDED(hResult = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&m_pDevice)))
			{
				m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
				m_pDevice->SetRenderState(D3DRS_ZENABLE,true);
				m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

				m_uiHeight = uiHeight;
				m_uiWidth = uiWidth;
				if(CanInitVertexBuffer() && CanInitTexture())
				{
					return true;
				}
			}
			else
			{
				switch(hResult)
				{
				case D3DERR_DEVICELOST:			err(L"Error Device Lost",EL_ERROR);				break;
				case D3DERR_INVALIDCALL:		err(L"Error Invalid Call",EL_ERROR);			break;
				case D3DERR_NOTAVAILABLE:		err(L"Error Not Available",EL_ERROR);			break;
				case D3DERR_OUTOFVIDEOMEMORY:	err(L"Error Out of Video Memory",EL_ERROR);		break;
				}
			}
		}
		return false;
	}
	inline bool CDirectX9::bLockDisplay(NUtility::TArray2D<NGraphics::TColor<float> > &tDisplay)
	{
		D3DLOCKED_RECT lockedRect;
		m_pTexture->LockRect(0,&lockedRect,NULL,0);
		//Remember that the Pitch does not necessarily equal width
		if(lockedRect.pBits)
		{
			tDisplay.Set((NGraphics::TColor<float>*)lockedRect.pBits,lockedRect.Pitch/16,m_uiHeight);
			return true;
		}
		else
			return false;
	}
/*
	inline NGraphics::TColor<float>* CDirectX9::pLockDisplay()
	{
		using namespace NGraphics;
		D3DLOCKED_RECT lockedRect;
		m_pTexture->LockRect(0,&lockedRect,NULL,0);
		
		return (TColor<float>*)lockedRect.pBits;
	}

	inline unsigned int CDirectX9::uiGetDisplayPitch()
	{
		D3DLOCKED_RECT lockedRect;
		m_pTexture->LockRect(0,&lockedRect,NULL,0);
		return lockedRect.Pitch/16;
	}*/

	inline void CDirectX9::UnlockDisplay()
	{
		m_pTexture->UnlockRect(0);
	}

	/*
	inline void CDirectX9::Draw()
	{
		using namespace NGraphics;
		D3DLOCKED_RECT lockedRect;
		m_pTexture->LockRect(0,&lockedRect,NULL,0);
		CPixel* pData = (CPixel*)lockedRect.pBits;
		
		
		for(unsigned int ui=0;ui<m_uiHeight*m_uiWidth;++ui)
		{
			pData[ui].SetAlpha(float(ui)*1/float(m_uiWidth*m_uiHeight));
			pData[ui].SetBlue(float(ui)*1/float(m_uiWidth*m_uiHeight));
			pData[ui].SetGreen(float(ui)*1/float(m_uiWidth*m_uiHeight));
			pData[ui].SetRed(float(ui)*1/float(m_uiWidth*m_uiHeight));
		}

		m_pTexture->UnlockRect(0);
	}*/

	inline void CDirectX9::Render()
	{
		if(m_pDevice)
		{
			m_pDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);
			
			//Draw();
			
			if(SUCCEEDED(m_pDevice->BeginScene()))
			{
				m_pDevice->SetTexture(0,m_pTexture);
				//m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(SCustomVertex));
				m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(STextureVertex));
				//m_pDevice->SetFVF(SCustomVertex::dwGetCustomVertexFormat());
				m_pDevice->SetFVF(STextureVertex::dwGetCustomFVF());
				m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
				m_pDevice->EndScene();
			}
			m_pDevice->Present(NULL,NULL,NULL,NULL);
		}
	}
	void CDirectX9::Shutdown()
	{
		if(m_pTexture)
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
		if(m_pVB)
		{
			m_pVB->Release();
			m_pVB = NULL;
		}
		if(m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = NULL;
		}
		if(m_pD3D)
		{
			m_pD3D->Release();
			m_pD3D = NULL;
		}
	}

	bool CDirectX9::CanInitVertexBuffer()
	{
		/*
		STextureVertex vertices[6] =
		{
			{-1.0f,-1.0f,0.5f,	0.0f,0.0f},
			{1.0f,1.0f,1.0f,	1.0f,1.0f},
			{-1.0f,1.0f,1.0f,	0.0f,1.0f},
			{-1.0f,-1.0f,1.0f,	0.0f,0.0f},
			{1.0f,1.0f,1.0f,	1.0f,1.0f},
			{1.0f,-1.0f,1.0f,	1.0f,0.0f},
		};*/
		/*
		(-1.0f,1.0f)(0.0,0.0)		(1.0f,1.0f)(1.0f,0.0)
		(-1.0f,-1.0f)(0.0,1.0f)		(1.0f,-1.0f)(1.0f,1.0f)*/
		STextureVertex vertices[6] =
		{
			{-1.0f,-1.0f,1.0f,	0.0f,1.0f},
			{1.0f,1.0f,1.0f,	1.0f,0.0f},
			{-1.0f,1.0f,1.0f,	0.0f,0.0f},
			{-1.0f,-1.0f,1.0f,	0.0f,1.0f},
			{1.0f,1.0f,1.0f,	1.0f,0.0f},
			{1.0f,-1.0f,1.0f,	1.0f,1.0f},
		};

		if(SUCCEEDED(m_pDevice->CreateVertexBuffer(6*sizeof(STextureVertex),0,
			STextureVertex::dwGetCustomFVF(),D3DPOOL_DEFAULT,&m_pVB,NULL)))
		{
			void* pVertices=NULL;
			if(SUCCEEDED(m_pVB->Lock(0,sizeof(vertices),(void**)&pVertices,0)))
			{
				memcpy(pVertices,vertices,sizeof(vertices));
				m_pVB->Unlock();
				return true;
			}
		}
		return false;
	}

	LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 device, unsigned int width, unsigned int height) {
		LPDIRECT3DTEXTURE9 texture = NULL;
		auto result = device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &texture, NULL);
		if (SUCCEEDED(result)) {
			return texture;
		}
		else {
			return NULL;
		}
	}

	bool CDirectX9::CanInitTexture()
	{	
		/*if(SUCCEEDED(D3DXCreateTexture(m_pDevice,m_uiWidth,m_uiHeight,1,D3DUSAGE_DYNAMIC,D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,&m_pTexture)))
			return true;
		else
			return false;*/
		m_pTexture = CreateTexture(m_pDevice, m_uiWidth, m_uiHeight);
		return m_pTexture != NULL;
	}
}

#endif