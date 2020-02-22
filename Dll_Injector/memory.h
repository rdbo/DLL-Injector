#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

typedef uintptr_t PTR;

namespace Mem
{
	void CharToWChar(char str[], wchar_t buffer[]);
	namespace Ex
	{
		HANDLE GetProcessHandle(DWORD pid);
		DWORD GetCurrentPID(LPCWSTR processName);
		PTR GetModuleAddress(LPCWSTR moduleName, DWORD pid);
		PTR GetPointer(HANDLE hProc, PTR ptr, std::vector<PTR> offsets);
		BOOL WriteBuffer(HANDLE hProc, PTR address, const void* value, SIZE_T size);
		BOOL ReadBuffer(HANDLE hProc, PTR address, void* buffer, SIZE_T size);
	}
	namespace In
	{
		HANDLE GetCurrentProcessHandle();
		DWORD GetCurrentPID();
		PTR GetModuleAddress(LPCWSTR moduleName);
		PTR GetPointer(PTR baseAddress, std::vector<PTR> offsets);
		bool WriteBuffer(PTR address, const void* value, SIZE_T size);
		bool ReadBuffer(PTR address, void* buffer, SIZE_T size);
		template <class type>
		type Read(PTR address);
		template <class type>
		void Write(PTR address, type value);
	}
}

template <class type>
type Mem::In::Read(PTR address)
{
	return *(type*)(address);
}

template <class type>
void Mem::In::Write(PTR address, type value)
{
	*(type*)(address) = value;
}