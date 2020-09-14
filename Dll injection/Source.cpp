#include "Source.h"

DWORD GetProcessId(IN PCHAR szExeName)

{
	DWORD dwRet = 0;
	DWORD dwCount = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(PROCESSENTRY32);

		BOOL bRet = Process32First(hSnapshot, &pe);

		while (bRet)
		{
			if (!strcmp(szExeName, pe.szExeFile))
			{
				dwCount++;
				dwRet = pe.th32ProcessID;
			}
			bRet = Process32Next(hSnapshot, &pe);
		}

		if (dwCount > 1)
		{
			dwRet = 0xFFFFFFFF;
		}

		CloseHandle(hSnapshot);
	}

	return dwRet;
}

BOOL CreateRemoteThreadInject(DWORD ID, const char * dll)
{
	LPVOID Memory;
	HANDLE hn;
	HANDLE RemoteThread;
	HANDLE Process;

	if (!ID) return false;

	Process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, ID);
	hn = (HANDLE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	Memory = (LPVOID)VirtualAllocEx(Process, NULL, strlen(dll) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(Process, (LPVOID)Memory, dll, strlen(dll) + 1, NULL);
	RemoteThread = CreateRemoteThread(Process, NULL, NULL, (LPTHREAD_START_ROUTINE)hn, (LPVOID)Memory, NULL, NULL);

	CloseHandle(Process);
	VirtualFreeEx(Process, (LPVOID)Memory, 0, MEM_RELEASE);

	return true;
}

BOOL Start()
{
	char dll[MAX_PATH];

	GetFullPathName("Injection.dll", MAX_PATH, dll, NULL);
	DWORD ID = GetProcessId("Test.exe");
	
	if (!CreateRemoteThreadInject(ID, dll))
	{
		printf("failure\n");
		return false;
	}
	else
	{
		printf("success\n");
		return true;
	}
}