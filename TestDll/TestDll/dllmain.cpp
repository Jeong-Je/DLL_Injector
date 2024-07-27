#define WIN32_LEAN_AND_MEAN // Windows 헤더에서 자주 사용되지 않는 부분 제외
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <tchar.h>
#include "MinHook.h"
#include "pch.h"

#pragma comment(lib, "ws2_32.lib")

// 원래 WSASend 함수의 함수 포인터 정의
typedef int (WINAPI* WSASend_t)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WSASend_t fpWSASend = NULL;

// 후킹된 WSASend 함수
int WINAPI HookedWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    OutputDebugString(L"WSASend function is called!");

    return fpWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine); // 원래 WSASend 함수 호출
}

void InitHooking1()
{
    // MinHook 초기화
    if (MH_Initialize() != MH_OK)
    {
        return;
    }

    // ws2_32.dll의 핸들을 가져옴
    HMODULE hWs2_32 = GetModuleHandle(_T("ws2_32.dll"));
    if (hWs2_32 == NULL)
    {
        return;
    }

    // WSASend 함수의 주소를 가져옴
    void* pSend = (void*)GetProcAddress(hWs2_32, "WSASend");
    if (pSend == NULL)
    {
        return;
    }

    // WSASend 함수에 대한 후크 생성
    if (MH_CreateHook(pSend, &HookedWSASend, reinterpret_cast<LPVOID*>(&fpWSASend)) != MH_OK)
    {
        return;
    }

    // WSASend 함수에 대한 후크 활성화
    if (MH_EnableHook(pSend) != MH_OK)
    {
        return;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        OutputDebugString(L"DLL 주입 성공");
        DisableThreadLibraryCalls((HMODULE)hinstDLL);
        InitHooking1();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        MH_Uninitialize(); // MinHook 정리
    }

    return TRUE;
}
