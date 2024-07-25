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
        _tprintf(L"[Ÿ��EXE��] [�������� DLL ���]�� ���ڷ� �Ѱ��ּ���. \n");
        return 1;
    }

    LPCTSTR processName = argv[1];
    LPCTSTR dllPath = argv[2];

    _tprintf(L"[%s]�� ������ �����...\n", processName);

    DWORD dwPID = 0;
    while ((dwPID = GetPIDByName(processName)) == 0) {
        Sleep(1000); // 1�� �������� Ÿ�� ���μ��� Ž��
    }

    _tprintf(L"%s �� �߰� : PID %lu. Injecting DLL...\n", processName, dwPID);


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

    // ���μ��� ����� ���
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    // DLL ��� ���ڿ� ���̸�ŭ ���� Ȯ��
    SIZE_T dwBufSize = (_tcslen(DllPath) + 1) * sizeof(TCHAR);
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

    // ������ DLL�� ���μ������� �ε��ϱ�
    hMod = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

    // ������ DLL�� ���ݾ����� ������ ���� ���μ������� �ε�
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
    // �����尡 ����ɶ� ���� ���
    WaitForSingleObject(hThread, INFINITE);

    // ������ �۾�
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
