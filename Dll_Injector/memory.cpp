#include "memory.h"

//Global

void Mem::CharToWChar(char str[], wchar_t buffer[])
{
	//use _CRT_SECURE_NO_WARNINGS
	mbstowcs(buffer, str, MAX_PATH);
}

//External
DWORD Mem::Ex::GetCurrentPID(LPCWSTR processName)
{
	DWORD pid = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!lstrcmp(procEntry.szExeFile, processName))
				{
					pid = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	return pid;
}

PTR Mem::Ex::GetModuleAddress(LPCWSTR moduleName, DWORD pid)
{
	PTR moduleAddr = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!lstrcmp(modEntry.szModule, moduleName))
				{
					moduleAddr = (PTR)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return moduleAddr;
}

HANDLE Mem::Ex::GetProcessHandle(DWORD pid)
{
	return OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
}

BOOL Mem::Ex::WriteBuffer(HANDLE hProc, PTR address, const void* value, SIZE_T size)
{
	return WriteProcessMemory(hProc, (BYTE*)address, value, size, nullptr);
}

BOOL Mem::Ex::ReadBuffer(HANDLE hProc, PTR address, void* buffer, SIZE_T size)
{
	return ReadProcessMemory(hProc, (BYTE*)address, &buffer, size, nullptr);
}

PTR Mem::Ex::GetPointer(HANDLE hProc, PTR baseAddress, std::vector<PTR> offsets)
{
	PTR addr = baseAddress;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr += offsets[i];
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
	}
	return addr;
}

//Internal

HANDLE Mem::In::GetCurrentProcessHandle()
{
	return GetCurrentProcess();
}

DWORD Mem::In::GetCurrentPID()
{
	return GetCurrentProcessId();
}

PTR Mem::In::GetModuleAddress(LPCWSTR moduleName)
{
	return (PTR)GetModuleHandle(moduleName);
}

PTR Mem::In::GetPointer(PTR baseAddress, std::vector<PTR> offsets)
{
	PTR addr = baseAddress;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr += offsets[i];
		addr = *(PTR*)addr;
	}
	return addr;
}

bool Mem::In::WriteBuffer(PTR address, const void* value, SIZE_T size)
{
	DWORD oProtection;
	if (VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oProtection))
	{
		memcpy((void*)(address), value, size);
		VirtualProtect((LPVOID)address, size, oProtection, NULL);
		return true;
	}

	return false;
}

bool Mem::In::ReadBuffer(PTR address, void* buffer, SIZE_T size)
{
	Mem::In::WriteBuffer((PTR)buffer, (void*)address, size);
	return false;
}