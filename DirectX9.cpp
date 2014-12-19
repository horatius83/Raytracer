#include "DirectX9.h"

namespace System
{
	bool DirectX9::CanInitialize(HWND hWnd, unsigned int uiWidth, unsigned int uiHeight, bool bWindowed)
	{
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_pD3D)
		{
			D3DPRESENT_PARAMETERS d3dpp;
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.Windowed = bWindowed;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.BackBufferFormat = bWindowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
			d3dpp.BackBufferWidth = uiWidth;
			d3dpp.BackBufferHeight = uiHeight;
			d3dpp.BackBufferCount = 1;
			d3dpp.EnableAutoDepthStencil = true;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;

			HRESULT hResult;

			if (SUCCEEDED(hResult = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice)))
			{
				m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
				m_pDevice->SetRenderState(D3DRS_ZENABLE, true);
				m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

				m_uiHeight = uiHeight;
				m_uiWidth = uiWidth;
				if (CanInitVertexBuffer() && CanInitTexture())
				{
					return true;
				}
			}
			else
			{
				switch (hResult)
				{
				case D3DERR_DEVICELOST:			err(L"Error Device Lost", EL_ERROR);				break;
				case D3DERR_INVALIDCALL:		err(L"Error Invalid Call", EL_ERROR);			break;
				case D3DERR_NOTAVAILABLE:		err(L"Error Not Available", EL_ERROR);			break;
				case D3DERR_OUTOFVIDEOMEMORY:	err(L"Error Out of Video Memory", EL_ERROR);		break;
				}
			}
		}
		return false;
	}

	bool DirectX9::LockDisplay(Utility::Array2D< Graphics::Color<float> >& tDisplay)
	{
		D3DLOCKED_RECT lockedRect;
		m_pTexture->LockRect(0, &lockedRect, NULL, 0);
		//Remember that the Pitch does not necessarily equal width
		if (lockedRect.pBits)
		{
			tDisplay.Set((Graphics::Color<float>*)lockedRect.pBits, lockedRect.Pitch / 16, m_uiHeight);
			return true;
		}
		else
			return false;
	}

	void DirectX9::UnlockDisplay()
	{
		m_pTexture->UnlockRect(0);
	}

	void DirectX9::Render()
	{
		if (m_pDevice)
		{
			m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

			if (SUCCEEDED(m_pDevice->BeginScene()))
			{
				m_pDevice->SetTexture(0, m_pTexture);
				m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(TextureVertex));
				m_pDevice->SetFVF(TextureVertex::dwGetCustomFVF());
				m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
				m_pDevice->EndScene();
			}
			m_pDevice->Present(NULL, NULL, NULL, NULL);
		}
	}

	void DirectX9::Shutdown()
	{
		if (m_pTexture)
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
		if (m_pVB)
		{
			m_pVB->Release();
			m_pVB = NULL;
		}
		if (m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = NULL;
		}
		if (m_pD3D)
		{
			m_pD3D->Release();
			m_pD3D = NULL;
		}
	}

	bool DirectX9::CanInitVertexBuffer()
	{
		TextureVertex vertices[6] =
		{
			{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
			{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
			{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
			{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		};

		if (SUCCEEDED(m_pDevice->CreateVertexBuffer(6 * sizeof(TextureVertex), 0,
			TextureVertex::dwGetCustomFVF(), D3DPOOL_DEFAULT, &m_pVB, NULL)))
		{
			void* pVertices = NULL;
			if (SUCCEEDED(m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
			{
				memcpy(pVertices, vertices, sizeof(vertices));
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

	bool DirectX9::CanInitTexture()
	{
		m_pTexture = CreateTexture(m_pDevice, m_uiWidth, m_uiHeight);
		return m_pTexture != NULL;
	}
}