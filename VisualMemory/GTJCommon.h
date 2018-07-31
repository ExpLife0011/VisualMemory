#pragma once
#include <QString>
#include <Windows.h>

namespace ggp
{
    enum EnumCalcType{GGJ, GCL, GTJ, NONECALCTYPE};
    enum EnumCompareType{THIRTEEN_WITH_SEVENTEEN, SEVENTEEN_WITH_SEVENTEEN, NONECOMPARETYPE};
    enum EnumCompareMeans{ONEBYONE, ALLBYONE};

    enum EnumRunType{CMDRUNTYE, EXERUNTYPE, NONAMERUNTYPE};
    //�ļ���׺����
    const QString g_cGTJExtend = "GTJ";
    const QString g_cGCLExtend = "GCL10";
    const QString g_cGGJExtend = "GGJ12";

    //���г�������
    const QString g_cGaeaName           = "GAEA.exe";
    const QString g_cCollectName        = "GTJCalcExe.exe";
    const QString g_cCommpareName       = "GTJCompare.exe";
    const QString g_cCommpareCMDName    = "CompareCMD.exe";
    const QString g_cUpgrateName        = "GTJUpgrade.exe";
    const QString g_cUpgrateCMDName     = "GTJUpgradeCMD.exe";
    const QString g_cCheckDataName      = "UnName";

    //�����ļ��У����г���key
    const QString g_cGaeaKey           = "GAEA";
    const QString g_cCollectExeKey     = "GTJCalcExe";
    const QString g_cCommpareKey       = "GTJCompare";
    const QString g_cCommpareCMDKey    = "CompareCMD";
    const QString g_cUpgrateKey        = "GTJUpgrade";
    const QString g_cUpgrateCMDKey     = "GTJUpgradeCMD";
    const QString g_cCheckDataKey      = "UnName";

    //�����ļ��У�Ŀ¼key
    const QString g_cCollectMidDataPathKey       = "CollectMidData";        //�����м������·��
    const QString g_cCollectProjectsPathKey    = "CollectFilePath";      //���ܹ���·��
    const QString g_cCprBasePathKey            = "CprBasePath";          //��׼����·��
    const QString g_cCprPathKey                = "CprPath";              //�Աȹ���·��
    const QString g_cCprResultPathKey          = "CprResultPath";        //�ԱȽ��·��
    const QString g_cTempCalcPathKey           = "TempCalcPath";         //���ܼ�����ʱĿ¼
    const QString g_cTempProjectFilesPathKey   = "TempProjectFilesPath";

    //����ļ�����
    const QString g_cColResultFileName      = "CollectResult.txt";
    const QString g_cColErrorMessageFileName    = "CollectErrorMessage.txt";
    //exe Path
    struct stConfigureExePath
    {
        QString gaeaPath;           //gaea����·��
        QString collectExePath;     //����exe����·��
        QString comparePath;        //�Աȹ�������·��
        QString compareCMDPath;     //�Աȹ�������·��
        QString upgratePath;        //��汾��������·��
        QString upgrateCMDPath;     //��汾��������������·��
        QString checkDataPath;      //�м����ݲ�У������·��
    };

    //dir Path
    struct stConfigureDirPath
    {
        QString collectMidDataPath;         //�����м������·��
        QString collectProjectsPath;        //���ܼ��㹤��·��
        QString cprBasePath;           //��׼����·��
        QString cprPath;               //�Աȹ���·��
        QString cprResultPath;         //�ԱȽ��·��
        QString tempCalcPath;          //���ܼ�����ʱĿ¼
        QString tempProjectFilesPath;  
    };

	//exe operation
	namespace ExeOpt
	{
		/*!
		*@brief    ��ȡ���ڽ��̵ľ��
		*@author caob 2017��8��4��
		*@param[in]    char* strPath	����·��
		*@return       HANDLE ���ؾ����ʧ�ܷ���nullptr
		*/
        HANDLE getProcessHandle( char* strExePath );

		/*!
		*@brief    ����exe
		*@author caob 2017��8��4��
		*@param[in]    QString strPath	����·��
		*@param[in]    QString strCmd   �����в���
		*@return       bool �����ɹ�����true
		*/
		bool startExe(QString strPath, QString strCmd);
            
		/*!
		*@brief    ֹͣexe
		*@author caob 2017��8��4��
		*@param[in]    QString strPath  exe���ƣ�����·��������չ��
		*@return       bool 
		*/
		bool stopCalcExe(QString strPath);	

		/*!
		*@brief    exe�Ƿ��Ѿ�����
		*@author caob 2017��8��4��
		*@param[in]    QString strPath  exe���ƣ�����·��������չ��
		*@return       bool �����ڷ���true
		*/
		bool isExist(QString strPath);
	};

	//file operation
	namespace FileOpt
	{
        //��ȡ�ļ���Ŀ¼
        QString getFileDir(const QString &filePath);

        //��ȡ�ļ����ƣ�AIncludeExtensionName:�Ƿ������չ��
        QString getFileName(const QString &AFilePath, bool AIncludeExtensionName = false);

        //�ж������ļ������Ƿ���ͬ��includeExtendName���Ƿ�ԱȺ�׺��
        bool sameNameFile(const QString &file1, const QString &file2, bool includeExtendName = false);
         
        //��ȡ�ļ���׺��
        QString getFileExtend(const QString &filePath);
	};

    namespace DirOpt
    {
        QStringList getFilesByDir( const QString &ADir );
        QStringList getSubFolderFiles(const QString &ADir, const QString filters);
        QStringList getSpecifiedFilesByDir( const QString &ADir , const QString &AFilter);

        int removeFilesByDir( const QString &ADir );
    };
};

