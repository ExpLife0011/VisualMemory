// ProcessAddressAnalysis.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include <qstring>
#include "GTJCommon.h"
#include <DbgHelp.h>
#include <iostream>
#include <QDebug>
using namespace std;
using namespace ggp;
const UINT max_name_length = 256;	// Max length of symbols' name.

BOOL CALLBACK SymEnumSymbolsProc(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    QString str;
    if (pSymInfo != 0)
    {                        
        //if (strcmp("KeAttachProcess", pSymInfo->Name) == 0)
        {        
            str = QString("����: %s ��ַ: %08x:").arg(pSymInfo->Name).arg((DWORD)pSymInfo->Address);
            cout<<str.toLocal8Bit().data();
        }                
    }
    return TRUE;
}

std::vector<QString> strDllNameArr;
std::vector<HANDLE> strModleInfoList;
void getProccDllName(HANDLE process)
{
    //// ��ȡ�ý��̵Ŀ���
    //MODULEENTRY32 me32;
    //DWORD nPid = GetProcessId(process);
    //HANDLE hProcessDll = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, nPid);
    //
    //me32.dwSize = sizeof(MODULEENTRY32);
    //if (hProcessDll == NULL)
    //{
    //    return ;
    //}
    //do
    //{
    //    strDllNameArr.push_back(QString::fromWCharArray(me32.szExePath));
    //    LPMODULEINFO modelInfo = new MODULEINFO;
    //    modelInfo->lpBaseOfDll = me32.modBaseAddr;
    //    modelInfo->SizeOfImage = me32.modBaseSize;
    //    modelInfo->EntryPoint = me32.;
    //    strModleInfoList.push_back()
    //}
    //while( Module32Next(hProcessDll, &me32));
}

//��ȡdll�����б�
int getModleFunList()
{
    DWORD dwFileSize;//�ļ���С
    QString str;
    DWORD error;//����
    LPCWSTR FilePathName = L"G:\\myProject\\Blackhouse\\Win32\\Release\\VisualMemory.dll";//�ļ�ȫ·��
    QString pSymPath = "G:\\myProject\\Blackhouse\\Win32\\Release";
    HANDLE pHandle = GetModuleHandle(NULL);
    pHandle = ExeOpt::getProcessHandle(L"ProcessAddressAnalysis.exe");
    SymCleanup(pHandle);
    if (!SymInitialize(pHandle, NULL, true))
    {
        error = GetLastError();
        str = QString("SymInitialize returned error  %x").arg(error);
        qDebug()<<(str);
        return 0;
    }
    //�õ��ļ���С
    HANDLE hFile = CreateFile(FilePathName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        error = GetLastError();
        str = QString("SCreateFile error  %x").arg(error);
        qDebug()<<(str);
        return false;
    }

    if (INVALID_FILE_SIZE == (dwFileSize = GetFileSize(hFile, NULL)))
    {
        error = GetLastError();
        str = QString("GetFileSize error  %x").arg(error);
        qDebug()<<(str);
    }
    CloseHandle(hFile);

    HMODULE pModleHandle = LoadLibrary(FilePathName);
    //����SymLoadModule���������Ӧ���ſ�
    DWORD64 dw64ModAddress = SymLoadModule64(pHandle, NULL, "VisualMemory.dll", NULL, (DWORD64)pModleHandle, dwFileSize);

    if (dw64ModAddress == 0)
    {
        return 0;
    }

    //ʹ��SymEnumSymbols����ö��ģ���еķ���
    if (!SymEnumSymbols(pHandle,
        dw64ModAddress,
        NULL, // Null point out that list all symbols
        SymEnumSymbolsProc,  //�ص�����
        NULL))
    {
        return 0;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE pHandle = ExeOpt::getProcessHandle(L"ProcessAddressAnalysis.exe");
    getProccDllName(pHandle);
    system("pause");
	return 0;
}

