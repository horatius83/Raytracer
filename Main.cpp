#include "DirectX9.h"
#include "RayTracer.h"
#include "HtmlError.h"
#include "WindowsTimer.h"
#include "Texture.h"
#include "Sierpinski.h"
#include "BihTree.h"
#include "MeshLoader.h"
#include "BlockText.h"
#include <sstream>

System::DirectX9 g_DirectX;
RayTracer::RayTracer g_RayTracer;
RayTracer::BihTree	g_Search;
Utility::HtmlError g_Error;
Graphics::Texture g_Texture;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
	{
		err(L"DirectX Shutdown", EL_EVENT);
		PostQuitMessage(0);
		return 0;
	}break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);

		switch (wParam)
		{
		case VK_UP:		g_RayTracer.Camera.Move(Math::Vector(0.0f, 0.0f, 0.5f));	break;
		case VK_DOWN:	g_RayTracer.Camera.Move(Math::Vector(0.0f, 0.0f, -0.5f));	break;
		case VK_RIGHT:	g_RayTracer.Camera.Move(Math::Vector(0.5f, 0.0f, 0.0f));	break;
		case VK_LEFT:	g_RayTracer.Camera.Move(Math::Vector(-0.5f, 0.0f, 0.0f));	break;
		case 0x5A:		g_RayTracer.Camera.Move(Math::Vector(0.0f, -0.5f, 0.0f));	break;
		case 0x41:		g_RayTracer.Camera.Move(Math::Vector(0.0f, 0.5f, 0.0f));	break;
		case VK_ESCAPE: PostQuitMessage(0); break;
		};
	}break;
	case WM_PAINT:
	{
		ValidateRect(hWnd, NULL);
		return 0;
	}break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"Raytracer", NULL };
	RegisterClassEx(&wc);

	//(1024,768) (800,600) (512,384) (400,300)

	//unsigned int uiWidth = 1024;
	//unsigned int uiHeight = 768;
	unsigned int uiWidth = 800;
	unsigned int uiHeight = 600;
	//unsigned int uiWidth = 512;
	//unsigned int uiHeight = 384;

	// Create the application window
	HWND hWnd = CreateWindow(L"Raytracer", L"The Beginning",
		WS_OVERLAPPEDWINDOW, 100, 100, uiWidth, uiHeight,
		NULL, NULL, wc.hInstance, NULL);

#ifdef _DEBUG
	g_Error.Open(L"Debug.html", L"errors.css", true);
#else
	g_Error.Open(L"Release.html",L"errors.css",true);
#endif

	g_Error.OutputVariable(L"Screen Width", int(uiWidth));
	g_Error.OutputVariable(L"Screen Height", int(uiHeight));

	if (g_Texture.CanLoadImage(L"banana.bmp"))
	{
		g_Error.OutputVariable(L"Texture Width", g_Texture.GetWidth());
		g_Error.OutputVariable(L"Texture Height", g_Texture.GetHeight());
		g_Error.OutputVariable(L"Texture BPP", g_Texture.GetBpp());
	}

	//El cheapo text generation
	Utility::BlockText<float> oText;
	oText.SetColor(Graphics::Color<float>(0.5f, 1.0f, 0.5f));

	g_RayTracer.Light.Set(Math::Vector(10.0f, 10.0f, 0.0f), Graphics::Color<float>(1.0f, 0.0f, 1.0f));

	g_RayTracer.Camera.CanSet(Math::Vector(-8.0f, 8.0f, -10.0f), Math::Vector(0.0f, 0.0f, 1.0f),
		Math::Vector(0.0f, 1.0f, 0.0f));

	Graphics::MeshLoader oLoader;

	if (!oLoader.CanLoad(L"box2.obj", g_Search.PolygonList))
		err(L"Could not load box2.obj,is it in the directory?", EL_WARNING);

	if (!g_Search.CanInitialize())
		err(L"Could not initialize scene graph.", EL_ERROR);

	if (g_DirectX.CanInitialize(hWnd, uiWidth, uiHeight))
	{

		err(L"DirectX Initialized", EL_EVENT);
		if (g_RayTracer.CanInitialize(uiWidth, uiHeight, &g_Search))
		{
			err(L"Raytracer Initialized", EL_EVENT);
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			Utility::WindowsTimer oTimer;
			oTimer.Initialize();
			std::string sTimer;
			Utility::Array2D< Graphics::Color<float> > displayBuffer;

			while (msg.message != WM_QUIT)
			{
				if (!PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					auto fps = 1000.0f / float(oTimer.GetMilliSeconds());
					std::ostringstream ss;
					ss << "FPS: " << fps;
					sTimer = ss.str();
					oTimer.Start();
					static float f = 0.0f;
					f += 0.01f;

					g_RayTracer.Light.Move(Math::Vector(0.0f, cos(f)*5.0f, sin(f)*5.0f));

					if (g_DirectX.LockDisplay(displayBuffer))
					{
						g_RayTracer.Render(displayBuffer);

						oText.Print(sTimer, 0, 0, displayBuffer);
						std::ostringstream locationString;

						sTimer = "Location " + g_RayTracer.Camera.GetOrigin().toString();
						oText.Print(sTimer, 0, 8, displayBuffer);

						sTimer = "Light " + g_RayTracer.Light.GetPosition().toString();
						oText.Print(sTimer, 0, 16, displayBuffer);

						g_DirectX.UnlockDisplay();
						g_DirectX.Render();
					}
					oTimer.Stop();
					g_Error.OutputVariable(L"Time", 1000.0f / float(oTimer.GetMilliSeconds()));
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
			err(L"Could not initialize Raytracer", EL_ERROR);
	}
	else
		err(L"Could not initialize DirectX", EL_ERROR);

	g_DirectX.Shutdown();
	g_Texture.DeleteImage();
	g_Error.Close();
	UnregisterClass(L"Raytracer", wc.hInstance);
	return 0;
}