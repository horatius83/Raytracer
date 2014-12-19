#ifndef DIRECTX9_H
#define DIRECTX9_H

#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

#include "Color.h"
#include "HtmlError.h"
#include "Array2D.h"

namespace System
{	
	struct TextureVertex
	{
		float x,y,z;
		float tu,tv;
		static inline DWORD dwGetCustomFVF(){return D3DFVF_XYZ | D3DFVF_TEX1;};
	};

	class DirectX9
	{
	public:
		DirectX9() : m_pD3D(NULL), m_pDevice(NULL), m_pVB(NULL),m_pTexture(NULL){};
		~DirectX9()
		{
			Shutdown();
		}
		bool CanInitialize(HWND hWnd,unsigned int uiWidth,unsigned int uiHeight,bool bWindowed=true);
		bool LockDisplay(Utility::Array2D< Graphics::Color<float> >& tDisplay);
		void UnlockDisplay();

		void Render();
		void Shutdown();
	private:
		bool CanInitVertexBuffer();
		bool CanInitTexture();

		LPDIRECT3D9             m_pD3D;
		LPDIRECT3DDEVICE9       m_pDevice; 
		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DTEXTURE9		m_pTexture;

		unsigned int			m_uiWidth;
		unsigned int			m_uiHeight;
	};
}

#endif