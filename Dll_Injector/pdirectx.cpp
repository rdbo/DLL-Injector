#include "pch.h"
#include "pdirectx.h"
#include "colors.h"
#include "fonts/roboto_regular.h"

bool CreateDeviceD3D(HWND hWnd);
void ResizeImGuiWindow();
void DrawBorder();

void D3D::Init(HINSTANCE hInstance, LPCWSTR wndName)
{
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"imguiApp";
	RegisterClassEx(&wc);
	hwnd = CreateWindow(wc.lpszClassName, wndName, WS_OVERLAPPEDWINDOW, 100, 100, DEFAULT_WIDTH, DEFAULT_HEIGHT, NULL, NULL, wc.hInstance, NULL);

	if (!CreateDeviceD3D(hwnd))
	{
		Shutdown();
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	roboto = io.Fonts->AddFontFromMemoryTTF(roboto_regular, sizeof(roboto_regular), FONT_SIZE, &font_cfg);
	roboto2 = io.Fonts->AddFontFromMemoryTTF(roboto_regular, sizeof(roboto_regular), FONT_SIZE * 2, &font_cfg);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(pDevice);



	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (resized)
		{
			ResizeImGuiWindow();
			resized = false;
		}

		DrawImGui();

		ImGui::EndFrame();
		pDevice->SetRenderState(D3DRS_ZENABLE, false);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(BG_R, BG_G, BG_B, BG_A);
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		DrawBorder();
		if (pDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			pDevice->EndScene();
		}

		HRESULT result = pDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	D3D::Shutdown();
}

void D3D::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (pDevice) { pDevice->Release(); pDevice = NULL; }
	if (pD3D) { pD3D->Release(); pD3D = NULL; }
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool CreateDeviceD3D(HWND hWnd)
{
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3D_INTERVAL;
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDevice) < 0)
		return false;
	return true;
}

void D3D::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = pDevice->Reset(&d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void ResizeImGuiWindow()
{
	ImVec2 wpos = ImGui::GetWindowPos();
	ImVec2 wsz = ImGui::GetWindowSize();
	if (wCRect.left != wpos.x || wCRect.top != wpos.y)
	{
		ImGui::SetNextWindowPos(ImVec2(wCRect.left, wCRect.top));
	}

	if (wCRect.right - wCRect.left != wsz.x || wCRect.bottom - wCRect.top != wsz.y)
	{
		ImGui::SetNextWindowSize(ImVec2(wCRect.right - wCRect.left, wCRect.bottom - wCRect.top));
	}
}

void D3D::DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT BarRect = { x, y, x + w, y + h };
	pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorder()
{
	D3D::DrawFilledRect(wCRect.left, wCRect.top, BG_BORDER_SIZE, wCRect.bottom - wCRect.top, D3DCOLOR_RGBA(BG_BORDER_COLOR_R, BG_BORDER_COLOR_G, BG_BORDER_COLOR_B, 255));
	D3D::DrawFilledRect(wCRect.right - BG_BORDER_SIZE, wCRect.top, BG_BORDER_SIZE, wCRect.bottom - wCRect.top, D3DCOLOR_RGBA(BG_BORDER_COLOR_R, BG_BORDER_COLOR_G, BG_BORDER_COLOR_B, 255));
	D3D::DrawFilledRect(wCRect.left, wCRect.bottom - BG_BORDER_SIZE, wCRect.right - wCRect.left, BG_BORDER_SIZE, D3DCOLOR_RGBA(BG_BORDER_COLOR_R, BG_BORDER_COLOR_G, BG_BORDER_COLOR_B, 255));
}