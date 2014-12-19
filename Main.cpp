#include "CDirectX9.h"
//#include "CFractalTest.h"
#include "CRayTracer.h"
#include "CHtmlError.h"
#include "CWindowsTimer.h"
#include "CTexture.h"
#include "TSierpinski.h"
#include "CBihTree.h"
#include "CMeshLoader.h"
#include "TBlockText.h"
//#include <boost/lexical_cast.hpp>
#include <sstream>

NSystem::CDirectX9			g_DirectX;
NRayTracer::CRayTracer		g_oRayTracer;
NRayTracer::CBihTree		g_oSearch;
NUtility::CHtmlError		g_oError;
NGraphics::CTexture			g_oTexture;

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
	case WM_DESTROY:	
		{
			err(L"DirectX Shutdown",EL_EVENT);
			PostQuitMessage( 0 );
			return 0;
		}break;
		case WM_KEYDOWN:
			{
				if(wParam==VK_ESCAPE)
					PostQuitMessage(0);

				switch(wParam)
				{
				case VK_UP:		g_oRayTracer.m_oCamera.Move(NMath::CVector(0.0f,0.0f,0.5f));	break;
				case VK_DOWN:	g_oRayTracer.m_oCamera.Move(NMath::CVector(0.0f,0.0f,-0.5f));	break;
				case VK_RIGHT:	g_oRayTracer.m_oCamera.Move(NMath::CVector(0.5f,0.0f,0.0f));	break;
				case VK_LEFT:	g_oRayTracer.m_oCamera.Move(NMath::CVector(-0.5f,0.0f,0.0f));	break;
				case 0x5A:		g_oRayTracer.m_oCamera.Move(NMath::CVector(0.0f,-0.5f,0.0f));	break;
				case 0x41:		g_oRayTracer.m_oCamera.Move(NMath::CVector(0.0f,0.5f,0.0f));	break;
				case VK_ESCAPE: PostQuitMessage(0); break;
				};			
			}break;
	case WM_PAINT:
		{
			//g_DirectX.Render();
			ValidateRect( hWnd, NULL );
			return 0;
		}break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"Raytracer", NULL };
    RegisterClassEx( &wc );

	//(1024,768) (800,600) (512,384) (400,300)
	
	//unsigned int uiWidth = 1024;
	//unsigned int uiHeight = 768;
	unsigned int uiWidth	= 800;
	unsigned int uiHeight	= 600;
	//unsigned int uiWidth = 512;
	//unsigned int uiHeight = 384;

    // Create the application window
    HWND hWnd = CreateWindow( L"Raytracer", L"The Beginning", 
                              WS_OVERLAPPEDWINDOW,100,100,uiWidth,uiHeight,
                              NULL, NULL, wc.hInstance, NULL );

#ifdef _DEBUG
	g_oError.bOpen(L"Debug.html",L"errors.css",true);
#else
	g_oError.bOpen(L"Release.html",L"errors.css",true);
#endif
	
	g_oError.OutputVariable(L"Screen Width",int(uiWidth));
	g_oError.OutputVariable(L"Screen Height",int(uiHeight));

	if(g_oTexture.bCanLoadImage(L"banana.bmp"))
	{
		g_oError.OutputVariable(L"Texture Width",g_oTexture.uiGetWidth());
		g_oError.OutputVariable(L"Texture Height",g_oTexture.uiGetHeight());
		g_oError.OutputVariable(L"Texture BPP",g_oTexture.uiGetBpp());
	}

	//El cheapo text generation
	NUtility::TBlockText<float> oText;
	oText.SetColor(NGraphics::TColor<float>(0.5f,1.0f,0.5f));

	g_oRayTracer.m_oLight.Set(NMath::CVector(0.0f,0.0f,0.0f),NGraphics::TColor<float>(1.0f,1.0f,1.0f));

	g_oRayTracer.m_oCamera.bCanSet(NMath::CVector(-8.0f,8.0f,-10.0f),NMath::CVector(0.0f,0.0f,1.0f),
		NMath::CVector(0.0f,1.0f,0.0f));

	NGraphics::CMeshLoader oLoader;

	if(!oLoader.bCanLoad(L"box2.obj",g_oSearch.m_oPolyList))
		err(L"Could not load box2.obj,is it in the directory?",EL_WARNING);

	/*
	if(!oLoader.bCanLoad(L"polygon.obj",g_oSearch.m_oPolyList))
		err(L"Could not load box2.obj, is it in the directory?",EL_WARNING);
	*/

	if(!g_oSearch.bCanInitialize())
		err(L"Could not initialize scene graph.",EL_ERROR);

	if(g_DirectX.bCanInitialize(hWnd,uiWidth,uiHeight))
	{

		err(L"DirectX Initialized",EL_EVENT);
		if(g_oRayTracer.bCanInitialize(uiWidth,uiHeight,&g_oSearch))
		{
			err(L"Raytracer Initialized",EL_EVENT);
			ShowWindow(hWnd,SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			MSG msg;
			ZeroMemory(&msg,sizeof(msg));
			NUtility::CWindowsTimer oTimer;
			oTimer.bInitialize();
			std::string sTimer;
			NUtility::TArray2D< NGraphics::TColor<float> > displayBuffer;

			while(msg.message!=WM_QUIT)
			{
				if(!PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
				{
					//sTimer = "FPS: ";
					//sTimer.append(boost::lexical_cast<std::string>(1000.0f/float(oTimer.ulGetMilliSeconds())));
					auto fps = 1000.0f / float(oTimer.ulGetMilliSeconds());
					std::ostringstream ss;
					ss << "FPS: " << fps;
					sTimer = ss.str();
					oTimer.Start();
					static float f = 0.0f;
					f+=0.01f;

					g_oRayTracer.m_oLight.Move(NMath::CVector(0.0f,cos(f)*5.0f,sin(f)*5.0f));


					if(g_DirectX.bLockDisplay(displayBuffer))
					{
						g_oRayTracer.Render(displayBuffer);

						oText.Print(sTimer,0,0,displayBuffer);
						std::ostringstream locationString;

						/*sTimer = "Location (";
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oCamera.oGetOrigin().fGetX()));
						sTimer.append(", ");
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oCamera.oGetOrigin().fGetY()));
						sTimer.append(", ");
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oCamera.oGetOrigin().fGetZ()));
						sTimer.append(")");*/
						sTimer = "Location " + g_oRayTracer.m_oCamera.oGetOrigin().toString();
						oText.Print(sTimer,0,8,displayBuffer);
					

						/*sTimer = "Light (";
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oLight.oGetPosition().fGetX()));
						sTimer.append(", ");
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oLight.oGetPosition().fGetY()));
						sTimer.append(", ");
						sTimer.append(boost::lexical_cast<std::string>(g_oRayTracer.m_oLight.oGetPosition().fGetZ()));
						sTimer.append(")");*/
						sTimer = "Light " + g_oRayTracer.m_oCamera.oGetDirection().toString();
						oText.Print(sTimer,0,16,displayBuffer);

						g_DirectX.UnlockDisplay();
						g_DirectX.Render();
					}
					oTimer.Stop();
					g_oError.OutputVariable(L"Time",1000.0f/float(oTimer.ulGetMilliSeconds()));
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			displayBuffer.Dereference();
		}
		else
			err(L"Could not initialize Raytracer",EL_ERROR);
	}
	else
		err(L"Could not initialize DirectX",EL_ERROR);
	
	g_DirectX.Shutdown();
	g_oTexture.DeleteImage();
	g_oError.Close();
    UnregisterClass( L"Raytracer", wc.hInstance );
    return 0;
}