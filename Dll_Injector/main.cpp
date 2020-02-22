#include "pch.h"
#include "pdirectx.h"
#include "colors.h"
#include "injector.h"
#define DEFAULT_ID 0
#define BUTTON_SELECT_FILE ImVec2(100, 25)
#define BUTTON_DEFAULT ImVec2(75, 25)

WNDCLASSEX wc;
HWND hwnd;
RECT wCRect;
IDirect3D9* pD3D;
D3DPRESENT_PARAMETERS d3dpp;
IDirect3DDevice9* pDevice;
ImGuiWindowFlags wf;
ImFont* roboto;
ImFont* roboto2;
bool resized;
char processName[MAX_PATH];
char dllPath[MAX_PATH];
int id = DEFAULT_ID;

void ImGuiSetupStyle();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	D3D::Init(hInstance, L"DLL Injector by rdbo");
	return 0;
}

std::string CreateNewLabel(const char* label)
{
	id += 1;
	std::string s;
	s = label;
	s += "##";
	s += std::to_string(id);
	return s;
}

void ResetID()
{
	id = DEFAULT_ID;
}

void GetDllFile()
{
	OPENFILENAME ofn;
	WCHAR filename[MAX_PATH];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"DLL Files\0*.dll\0";
	ofn.nFilterIndex = 1;
	GetOpenFileName(&ofn);
	wcstombs(dllPath, filename, MAX_PATH);
}

bool initstyle = false;
void D3D::DrawImGui()
{
	if (!initstyle)
	{
		ImGuiSetupStyle();
		initstyle = true;
	}
	ImGui::Begin("DLL Injector by rdbo", 0, wf);
	ImGui::PushFont(roboto2);
	ImGui::TextColored(COLOR_PURPLE, "DLL Injector");
	ImGui::PopFont();
	ImGui::PushFont(roboto);
	ImGui::Text("Process Name: "); ImGui::SameLine();
	ImGui::InputText(CreateNewLabel("").c_str(), processName, sizeof(processName));
	ImGui::Text("DLL Path: "); ImGui::SameLine();
	ImGui::InputText(CreateNewLabel("").c_str(), dllPath, sizeof(dllPath), ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	if (ImGui::Button("Select file", BUTTON_SELECT_FILE))
		GetDllFile();
	if (ImGui::Button(CreateNewLabel("Inject").c_str(), BUTTON_DEFAULT))
	{
		pIData = Injector::Data(processName, dllPath);
		pInjector.Inject(&pIData);
	}
	ImGui::PopFont();
	ResetID();
	ImGui::End();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK D3D::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (pDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
			GetClientRect(hwnd, &wCRect);
			resized = true;
		}
		return 0;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = DEFAULT_WIDTH;
		lpMMI->ptMinTrackSize.y = DEFAULT_HEIGHT;
	}
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void ImGuiSetupStyle()
{
	wf |= ImGuiWindowFlags_NoTitleBar;
	wf |= ImGuiWindowFlags_NoMove;
	wf |= ImGuiWindowFlags_NoResize;
	wf |= ImGuiWindowFlags_NoCollapse;
	wf |= ImGuiWindowFlags_NoSavedSettings;
	wf |= ImGuiWindowFlags_NoBackground;
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	style.WindowRounding = 0.f;
	colors[ImGuiCol_Text] = COLOR_WHITE;
	colors[ImGuiCol_FrameBg] =
		colors[ImGuiCol_FrameBgHovered] =
		colors[ImGuiCol_FrameBgActive] = COLOR_GRAY;
	colors[ImGuiCol_Button] = COLOR_DARK_PURPLE;
	colors[ImGuiCol_ButtonHovered] = COLOR_PURPLE;
	colors[ImGuiCol_ButtonActive] = COLOR_LIGHT_PURPLE;
	colors[ImGuiCol_ScrollbarBg] = COLOR_DARK_GRAY;
	colors[ImGuiCol_ScrollbarGrab] = COLOR_LIGHT_GRAY;
	colors[ImGuiCol_ScrollbarGrabHovered] = COLOR_LIGHTER_GRAY;
	colors[ImGuiCol_ScrollbarGrabActive] = COLOR_LIGHTER_GRAY_2;
}