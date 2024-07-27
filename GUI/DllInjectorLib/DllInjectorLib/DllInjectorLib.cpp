#include "pch.h"
#include "DllInjectorLib.h"

namespace DllInjectorLib {

    bool Injector::Inject(String^ processName, String^ dllPath)
    {
        pin_ptr<const wchar_t> wProcessName = PtrToStringChars(processName);
        pin_ptr<const wchar_t> wDllPath = PtrToStringChars(dllPath);

        DWORD dwPID = 0;
        while ((dwPID = GetPIDByName(wProcessName)) == 0)
        {
            System::Threading::Thread::Sleep(1000); // 1초 대기 후 다시 시도
        }

        return Inject(dwPID, wDllPath);
    }

    bool Injector::Inject(DWORD PID, LPCWSTR DllPath)
    {
        HANDLE hProcess = NULL;
        HANDLE hThread = NULL;
        LPVOID pRemoteBuf = NULL;
        HMODULE hMod = NULL;
        LPTHREAD_START_ROUTINE pThreadProc = NULL;

        // 프로세스 열기
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
        if (hProcess == NULL)
        {
            return false;
        }

        // DLL 경로 문자열 크기만큼 메모리 할당
        SIZE_T dwBufSize = (wcslen(DllPath) + 1) * sizeof(wchar_t);
        pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
        if (pRemoteBuf == NULL)
        {
            CloseHandle(hProcess);
            return false;
        }

        // 메모리에 DLL 경로 쓰기
        if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)DllPath, dwBufSize, NULL))
        {
            VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        // 원격 스레드를 사용하여 DLL 로드
        hMod = GetModuleHandle(L"kernel32.dll");
        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

        hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
        if (hThread == NULL)
        {
            VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        WaitForSingleObject(hThread, INFINITE);

        // 청소 작업
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);

        return true;
    }

    DWORD Injector::GetPIDByName(LPCWSTR processName)
    {
        DWORD dwPID = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (wcscmp(pe.szExeFile, processName) == 0)
                {
                    dwPID = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return dwPID;
    }
}