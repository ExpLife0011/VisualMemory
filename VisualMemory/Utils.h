#pragma once

#include <windows.h>
#include <QString>

class Utils
{
public:
    static double getRealMemoryRatio(int singleSize);
    static double getRealMemorySize(int singleSize);
    static BOOL PauseResumeThreadList(DWORD dwOwnerPID, bool bResumeThread);
    static BOOL ProcessList();
    static int KillProcessByName(const QString& processName, const QString& processPath);
    static int PauseResumeProcessByName(const QString& processName, bool bResume);
    static int AddProcessDirToPath(const QString& processName);
    static QString s_openAppName;

    static int getProcessIDByName(const QString& processName);

    // �ݹ��ȡָ���ļ����������ļ�
    static QStringList getFileList(const QString &path);

    static void dumpStack(void);
    static int dumpStack3(PVOID *Callers, ULONG Count);
    static bool readFuncInfoFromAddressList(QSet<__int64> &addressList);
    static QString getInfoFromAddress(int address);
    static int getAddressFromFuncName(const QString funcName);

    static QVector<int> GetProcessThreadIDList();
    static QVector<HANDLE> GetProcessThreadHandleList();

    static void initProcessWindbg();
    static void cleanupWindbg();

    static bool lookHeapMemory(PVOID ptr);
    static bool lookHeapMemory2(PVOID ptr, int size);
    static bool lookHeapMemory3(PVOID ptr);
    static bool lookHeapMemory4(PVOID ptr);
    static bool clearHeapMemoryCache();

    // �ⲿ��Ӧ�ý���ר������
    static bool initModulesInfo();
    static QString getModuleNameFromAddress(PVOID address);
    static QString getModuleNameAndRelativeAddress(PVOID address, DWORD &relativeAddress);

};

// ȫ�ֱ�����������һЩȫ�ֱ��������ã��������ͼ��ݶ���ģʽ����ʱ�ŵ�����

class GlobalData
{
public:
    GlobalData()
    {
        memoryMode = 1;
        isAutoMemoryLeakMode = false;
        isPreciseMemoryLeakMode = false;
        isInMemoryLeakRange = false;
        curStep = 0;
        totalLeakMemorySize = 0;
        totalLeakMemoryTimes = 0;
        hookTotalLeakMemorySize = 0;
        hookTotalLeakMemoryTimes = 0;
        totalLeakMemoryTimes_BeforeHook = 0;
        totalLeakMemorySize_BeforeHook = 0;
        totalLeakMemoryTimes_Last = 0;
        totalLeakMemorySize_Last = 0;
        relativeTotalLeakMemoryTimes = 0;
        relativeTotalLeakMemorySize = 0;
    }

    static GlobalData *getInstance()
    {
        static GlobalData s_globalData;
        return &s_globalData;
    }

    int memoryMode; // �ڴ�ģʽ��0-�ڴ�й©ģʽ��1-�ڴ�ͳ��ģʽ
    bool isPreciseMemoryLeakMode; // �Ƿ�׼�ڴ�й©ģʽ����׼й©ģʽ��ֻͳ���ڴ�й©�ĸ��ڵ㣬�������������ڴ�й©
    bool isAutoMemoryLeakMode;
    bool isInMemoryLeakRange;
    int curStep; // ��ǰ����״̬ 0-��ʼ�� 1-���������̣� 2-��ʼͳ�ƣ� 3-����ͳ�ƣ� 4-�����˳�

    int totalLeakMemorySize;
    int totalLeakMemoryTimes;
    int hookTotalLeakMemorySize;
    int hookTotalLeakMemoryTimes;

    int totalLeakMemoryTimes_BeforeHook; // hookǰ���е��ڴ���
    int totalLeakMemorySize_BeforeHook; // hookǰ���е��ڴ���
    int totalLeakMemoryTimes_Last; // ��һ�ε��ڴ���
    int totalLeakMemorySize_Last; // ��һ�ε��ڴ���
    int relativeTotalLeakMemoryTimes; // ��ʼ������ʱ���ܹ��ڴ�������
    int relativeTotalLeakMemorySize; // ��ʼ������ʱ���ܹ��ڴ�������
};

// ��Ϣ�������ȷ�Utils��
#define GLOBAL_STATICS_START_EVENT  ("Local\\GLOBAL_STATICS_START_EVENT") 
#define GLOBAL_STATICS_STOP_EVENT  ("Local\\GLOBAL_STATICS_STOP_EVENT") 
#define GLOBAL_PROCESS_EXIT_EVENT  ("Local\\GLOBAL_PROCESS_EXIT_EVENT") 
#define GLOBAL_CATHE_RECEIVED_EVENT  ("Local\\GLOBAL_CATHE_RECEIVED_EVENT") 

class EventController
{
public:
    EventController()
    {
        hStartEvent = CreateEventA(NULL, TRUE, FALSE, GLOBAL_STATICS_START_EVENT);
        hStopEvent = CreateEventA(NULL, TRUE, FALSE, GLOBAL_STATICS_STOP_EVENT);
        hExitEvent = CreateEventA(NULL, TRUE, FALSE, GLOBAL_PROCESS_EXIT_EVENT);
        hCatheReceivedEvent = CreateEventA(NULL, TRUE, FALSE, GLOBAL_CATHE_RECEIVED_EVENT);
    }

    static EventController *getInstance()
    {
        static EventController s_eventController;
        return &s_eventController;
    }

    HANDLE hStartEvent;
    HANDLE hStopEvent;
    HANDLE hExitEvent;
    HANDLE hCatheReceivedEvent;
};
