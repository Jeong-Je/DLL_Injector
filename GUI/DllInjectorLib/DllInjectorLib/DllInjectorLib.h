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
    };
}