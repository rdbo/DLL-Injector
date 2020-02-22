#pragma once
#include "pch.h"
#include "memory.h"

namespace Injector
{
	class Data
	{
	public:
		char* procName = NULL;
		DWORD pid = NULL;
		HANDLE hProc = NULL;
		char* dllPath = (char*)"";
		bool init = false;
		Data();
		Data(char processName[], char dllPath[]);
	};

	class InjectorClass
	{
	public:
		void Inject(Data* injData);
	};
}

extern Injector::InjectorClass pInjector;
extern Injector::Data pIData;