#pragma once
#include "public.h"

#define WIN32_LEAN_AND_MEAN 
#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)


DWORD GetProcessId(PCHAR szExeName);

BOOL CreateRemoteThreadInject(DWORD ID, const char * dll);

BOOL Start();