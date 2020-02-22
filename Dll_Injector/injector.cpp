#include "pch.h"
#include "injector.h"

Injector::InjectorClass pInjector;
Injector::Data pIData = Injector::Data();

Injector::Data::Data()
{
	procName = NULL;
	dllPath = (char*)"";
	pid = NULL;
	hProc = NULL;
	init = false;
}

Injector::Data::Data(char processName[], char path[])
{
	wchar_t buf[MAX_PATH];
	Mem::CharToWChar(processName, buf);
	procName = processName;
	pid = Mem::Ex::GetCurrentPID(buf);
	hProc = Mem::Ex::GetProcessHandle(pid);
	dllPath = path;
	FILE* file = fopen(dllPath, "r");
	init = pid != NULL && hProc && hProc != INVALID_HANDLE_VALUE && dllPath != NULL && file;
	if(file)
		fclose(file);
}

void Injector::InjectorClass::Inject(Injector::Data* injData)
{
	if (!injData->init)
	{
		MessageBox(NULL, L"Unable to Inject", L"Injector", MB_OK);
		return;
	}

	void* loc = VirtualAllocEx(injData->hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	char pDllPath[MAX_PATH];
	strcpy(pDllPath, injData->dllPath);
	WriteProcessMemory(injData->hProc, loc, pDllPath, strlen(pDllPath) + 1, 0);
	HANDLE hThread = CreateRemoteThread(injData->hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

	MessageBox(NULL, L"Injected!", L"Injector", MB_OK);
}