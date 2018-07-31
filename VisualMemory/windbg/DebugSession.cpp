#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <DbgHelp.h>
#include "DebugSession.h"
#include "HelperFunctions.h"



BOOL DispatchDebugEvent(const DEBUG_EVENT* pDebugEvent);

BOOL OnProcessCreated(const CREATE_PROCESS_DEBUG_INFO*);
BOOL OnThreadCreated(const CREATE_THREAD_DEBUG_INFO*);
BOOL OnException(const EXCEPTION_DEBUG_INFO*);
BOOL OnProcessExited(const EXIT_PROCESS_DEBUG_INFO*);
BOOL OnThreadExited(const EXIT_THREAD_DEBUG_INFO*);
BOOL OnOutputDebugString(const OUTPUT_DEBUG_STRING_INFO*);
BOOL OnRipEvent(const RIP_INFO*);
BOOL OnDllLoaded(const LOAD_DLL_DEBUG_INFO*);
BOOL OnDllUnloaded(const UNLOAD_DLL_DEBUG_INFO*);


static HANDLE g_hProcess = NULL; // static ����д�������⣬���ܻᵼ�±���ֵ��һ��
static HANDLE g_hThread = NULL;
static DWORD g_processID = 0;
static DWORD g_threadID = 0;

static int g_debuggeeStatus = STATUS_NONE;
static DWORD g_continueStatus = DBG_EXCEPTION_NOT_HANDLED;



//��ȡ�����Խ��̵�״̬��
int GetDebuggeeStatus() {
	return g_debuggeeStatus;
}

//��ȡ�����Խ��̵ľ��
HANDLE GetDebuggeeHandle() {
	return g_hProcess;
}


//���������Խ��̡�
void StartDebugSession(LPCTSTR path) {

	if (g_debuggeeStatus != STATUS_NONE) {
		std::wcout << TEXT("Debuggee is running.") << std::endl;
		return;
	}

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi = { 0 };

	if (CreateProcess(
		path,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE, // old: DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE | CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi) == FALSE) {

		std::wcout << TEXT("CreateProcess failed: ") << GetLastError() << std::endl;
		return;
	}

	g_hProcess = pi.hProcess;
	g_hThread = pi.hThread;
	g_processID = pi.dwProcessId;
	g_threadID = pi.dwThreadId;

	g_debuggeeStatus = STATUS_SUSPENDED;

	std::wcout << TEXT("Debugee has started and was suspended.") << std::endl;
    ContinueDebugSession();
}



//ָʾ�Ƿ������쳣�����handledΪTRUE����ʾ�������쳣��
//����Ϊû�д����쳣��
void HandledException(BOOL handled) {

	g_continueStatus = (handled == TRUE) ? DBG_CONTINUE : DBG_EXCEPTION_NOT_HANDLED;
}



//���������Խ��̵�ִ�С����ݱ����Խ��̵�״̬��ִ�в�ͬ�Ĳ�����
//���DispatchDebugEvent����TRUE������������Խ���ִ�У�
//�������FALSE������ͣ�����Խ��̣������û�������
void ContinueDebugSession() {

	if (g_debuggeeStatus == STATUS_NONE) {

		std::wcout << TEXT("Debuggee is not started yet.") << std::endl;
		return;
	}

	if (g_debuggeeStatus == STATUS_SUSPENDED) {

		ResumeThread(g_hThread);
	}
	else {

		ContinueDebugEvent(g_processID, g_threadID, g_continueStatus);
	}

	DEBUG_EVENT debugEvent;

	while (WaitForDebugEvent(&debugEvent, INFINITE) == TRUE) {

		if (DispatchDebugEvent(&debugEvent) == TRUE) {

			ContinueDebugEvent(g_processID, g_threadID, DBG_EXCEPTION_NOT_HANDLED);
		}
		else {

			break;
		}
	}
}



//���ݵ����¼������͵��ò�ͬ�Ĵ�������
BOOL DispatchDebugEvent(const DEBUG_EVENT* pDebugEvent) {

	switch (pDebugEvent->dwDebugEventCode) {

		case CREATE_PROCESS_DEBUG_EVENT:
			return OnProcessCreated(&pDebugEvent->u.CreateProcessInfo);
 
		case CREATE_THREAD_DEBUG_EVENT:
			return OnThreadCreated(&pDebugEvent->u.CreateThread);
 
		case EXCEPTION_DEBUG_EVENT:
			return OnException(&pDebugEvent->u.Exception);
 
		case EXIT_PROCESS_DEBUG_EVENT:
			return OnProcessExited(&pDebugEvent->u.ExitProcess);
 
		case EXIT_THREAD_DEBUG_EVENT:
			return OnThreadExited(&pDebugEvent->u.ExitThread);
 
		case LOAD_DLL_DEBUG_EVENT:
			return OnDllLoaded(&pDebugEvent->u.LoadDll);
 
		case OUTPUT_DEBUG_STRING_EVENT:
			return OnOutputDebugString(&pDebugEvent->u.DebugString);
 
		case RIP_EVENT:
			return OnRipEvent(&pDebugEvent->u.RipInfo);
 
		case UNLOAD_DLL_DEBUG_EVENT:
			return OnDllUnloaded(&pDebugEvent->u.UnloadDll);

		default:
			std::wcout << TEXT("Unknown debug event.") << std::endl;
			return FALSE;
	}
}



BOOL OnProcessCreated(const CREATE_PROCESS_DEBUG_INFO* pInfo) {

	//��ʼ�����Ŵ�����
	//ע�⣬���ﲻ��ʹ��pInfo->hProcess����Ϊg_hProcess��pInfo->hProcess
	//��ֵ������ͬ��������DbgHelp����ʹ�õ���g_hProcess��
	if (SymInitialize(g_hProcess, NULL, FALSE) == TRUE) {
	
		//����ģ��ĵ�����Ϣ
		DWORD64 moduleAddress = SymLoadModule64(
			g_hProcess,
			pInfo->hFile, 
			NULL,
			NULL,
			(DWORD64)pInfo->lpBaseOfImage,
			0);

		if (moduleAddress == 0) {

			std::wcout << TEXT("SymLoadModule64 failed: ") << GetLastError() << std::endl;
		}
	}
	else {

		std::wcout << TEXT("SymInitialize failed: ") << GetLastError() << std::endl;
	}

	//CloseHandle(pInfo->hFile);
	//CloseHandle(pInfo->hThread);
	//CloseHandle(pInfo->hProcess);

	return TRUE;
}



BOOL OnThreadCreated(const CREATE_THREAD_DEBUG_INFO* pInfo) {

	CloseHandle(pInfo->hThread);

	return TRUE;
}



//�����쳣��ʱ��Ӧ��֪ͨ�û��������û�����������Ӧ����FALSE��
BOOL OnException(const EXCEPTION_DEBUG_INFO* pInfo) {

	std::wcout << TEXT("An exception was occured at ");
	PrintHex((unsigned int)pInfo->ExceptionRecord.ExceptionAddress, FALSE);
	std::wcout << TEXT(".") << std::endl << TEXT("Exception code: ");
	PrintHex(pInfo->ExceptionRecord.ExceptionCode, TRUE);
	std::wcout << std::endl;

	if (pInfo->dwFirstChance == TRUE) {

		std::wcout << TEXT("First chance.") << std::endl;
	}
	else {

		std::wcout << TEXT("Second chance.") << std::endl;
	}

	g_debuggeeStatus = STATUS_INTERRUPTED;
	return FALSE;
}



//�����Խ������˳�����Ӧ�ټ���ִ�У����Է���FALSE��
BOOL OnProcessExited(const EXIT_PROCESS_DEBUG_INFO* pInfo) {

	std::wcout << TEXT("Debuggee was terminated.") << std::endl
			   << TEXT("Exit code: ") << pInfo->dwExitCode << std::endl;

	//���������Ϣ
	SymCleanup(g_hProcess);

	//�����ڴ�������¼���ʱ��Debuggee��δ����������
	//����Ҫ����һ��ContinueDebugEvent��ʹ��˳��������
	ContinueDebugEvent(g_processID, g_threadID, DBG_CONTINUE);

	CloseHandle(g_hThread);
	CloseHandle(g_hProcess);

	g_hProcess = NULL;
	g_hThread = NULL;
	g_processID = 0;
	g_threadID = 0;
	g_debuggeeStatus = STATUS_NONE;
	g_continueStatus = DBG_EXCEPTION_NOT_HANDLED;

	return FALSE;
}



BOOL OnThreadExited(const EXIT_THREAD_DEBUG_INFO* pInfo) {

	return TRUE;
}



//�����Խ������������Ϣ��Ӧ֪ͨ�û������û�����������FALSE��
BOOL OnOutputDebugString(const OUTPUT_DEBUG_STRING_INFO* pInfo) {

	BYTE* pBuffer = (BYTE*)malloc(pInfo->nDebugStringLength);

	SIZE_T bytesRead;

	ReadProcessMemory(
		g_hProcess,
		pInfo->lpDebugStringData,
		pBuffer, 
		pInfo->nDebugStringLength,
		&bytesRead);

	int requireLen = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED,
		(LPCSTR)pBuffer,
		pInfo->nDebugStringLength,
		NULL,
		0);

	TCHAR* pWideStr = (TCHAR*)malloc(requireLen * sizeof(TCHAR));

	//MultiByteToWideChar(
	//	CP_ACP,
	//	MB_PRECOMPOSED,
	//	(LPCSTR)pBuffer,
	//	pInfo->nDebugStringLength,
	//	pWideStr,
	//	requireLen);

	std::wcout << TEXT("Debuggee debug string: ") << pWideStr <<  std::endl;

	free(pWideStr);
	free(pBuffer);

	g_debuggeeStatus = STATUS_INTERRUPTED;
	return FALSE;
}



BOOL OnRipEvent(const RIP_INFO* pInfo) {

	std::wcout << TEXT("A RIP_EVENT occured.") << std::endl;

	g_debuggeeStatus = STATUS_INTERRUPTED;
	return FALSE;
}



BOOL OnDllLoaded(const LOAD_DLL_DEBUG_INFO* pInfo) {

	//����ģ��ĵ�����Ϣ
	DWORD64 moduleAddress = SymLoadModule64(
		g_hProcess,
		pInfo->hFile, 
		NULL,
		NULL,
		(DWORD64)pInfo->lpBaseOfDll,
		0);

	if (moduleAddress == 0) {

		std::wcout << TEXT("SymLoadModule64 failed: ") << GetLastError() << std::endl;
	}

	CloseHandle(pInfo->hFile);

	return TRUE;
}



BOOL OnDllUnloaded(const UNLOAD_DLL_DEBUG_INFO* pInfo) {

	SymUnloadModule64(g_hProcess, (DWORD64)pInfo->lpBaseOfDll);

	return TRUE;
}



//��ȡ�����Խ��̵����̵߳������Ļ�����
BOOL GetDebuggeeContext(CONTEXT* pContext) {

	pContext->ContextFlags = CONTEXT_FULL;

	if (GetThreadContext(g_hThread, pContext) == FALSE) {

		std::wcout << TEXT("GetThreadContext failed: ") << GetLastError() << std::endl;
		return FALSE;
	}

	return TRUE;
}



//ֹͣ�����Խ��̵�ִ�С�����Ҫ����һ��ContinueDebugSession��
//�õ���������EXIT_PROCESS_DEBUG_EVENT�¼���
void StopDebugSeesion() {

	if (TerminateProcess(g_hProcess, -1) == TRUE) {

		ContinueDebugSession();
	}
	else {

		std::wcout << TEXT("TerminateProcess failed: ") << GetLastError() << std::endl;
	}
}



//��ȡ�����Խ��̵��ڴ档
BOOL ReadDebuggeeMemory(unsigned int address, unsigned int length, void* pData) {

	SIZE_T bytesRead;

	return ReadProcessMemory(g_hProcess, (LPCVOID)address, pData, length, &bytesRead);
}