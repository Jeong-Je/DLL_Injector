#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <iostream>

using namespace std;

BOOL Inject(DWORD PID, LPCTSTR DllPath);
DWORD GetPIDByName(LPCTSTR pszProcessName);

int _tmain(int argc, TCHAR* argv[]) {
    _wsetlocale(LC_ALL, _T("korean"));
    if (argc != 3) {
        _tprintf(L"[타켓EXE명] [인젝션할 DLL 경로]를 인자로 넘겨주세요. \n");
        return 1;
    }

    LPCTSTR processName = argv[1];
    LPCTSTR dllPath = argv[2];

    _tprintf(L"[%s]의 실행을 대기중...\n", processName);

    DWORD dwPID = 0;
    while ((dwPID = GetPIDByName(processName)) == 0) {
        Sleep(1000); // 1초 간격으로 타켓 프로세스 탐색
    }

    _tprintf(L"%s 을 발견 : PID %lu. Injecting DLL...\n", processName, dwPID);


    if (Inject(dwPID, dllPath)) {
        _tprintf(L"Injection of %s succeeded.\n", dllPath);
    }
    else {
        _tprintf(L"Injection of %s failed.\n", dllPath);
    }

    return 0;

}

BOOL Inject(DWORD PID, LPCTSTR DllPath) {
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    LPVOID pRemoteBuf = NULL;
    HMODULE hMod = NULL;
    LPTHREAD_START_ROUTINE pThreadProc = NULL;

    // 프로세스 제어권 얻기
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    // DLL 경로 문자열 길이만큼 공간 확보
    SIZE_T dwBufSize = (_tcslen(DllPath) + 1) * sizeof(TCHAR);
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

    // 쓰여진 DLL을 프로세스에서 로드하기
    hMod = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

    // 쓰여진 DLL을 원격쓰레드 생성을 통해 프로세스에서 로드
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
    // 쓰레드가 실행될때 까지 대기
    WaitForSingleObject(hThread, INFINITE);

    // 마무리 작업
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return TRUE;
}

DWORD GetPIDByName(LPCTSTR processName) {
    DWORD dwPID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_tcscmp(pe.szExeFile, processName) == 0) {
                dwPID = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return dwPID;
}
