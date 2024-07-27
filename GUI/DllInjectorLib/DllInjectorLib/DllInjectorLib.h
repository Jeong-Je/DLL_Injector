#pragma once
#include <windows.h>
#include <tchar.h>
#include<vcclr.h>
#include <tlhelp32.h>

using namespace System;

namespace DllInjectorLib {

    public ref class Injector
    {
    public:
        bool Inject(String^ processName, String^ dllPath);
    private:
        bool Inject(DWORD PID, LPCWSTR DllPath);
        DWORD GetPIDByName(LPCWSTR processName);

        // 자식 프로세스에 DLL을 주입하는 메서드
        void InjectDLLIntoChildProcesses(DWORD parentPID, LPCWSTR dllPath);
    };
}