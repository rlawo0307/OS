/**
 *  file    ConsoleShell.h
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� ��� ����
 */

#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"
#include "Console.h"
////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "MINT64>"

// ���ڿ� �����͸� �Ķ���ͷ� �޴� �Լ� ������ Ÿ�� ����
typedef void ( * CommandFunction ) ( const char* pcParameter );


////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

typedef struct kParameterListStruct
{
    // �Ķ���� ������ ��巹��
    const char* pcBuffer;
    // �Ķ������ ����
    int iLength;
    // ���� ó���� �Ķ���Ͱ� �����ϴ� ��ġ
    int iCurrentPosition;
} PARAMETERLIST;
typedef struct kScreenSaverstruct
{
    BYTE run;
    CHARACTER prev[CONSOLE_HEIGHT*CONSOLE_WIDTH];
    int x;
    int y;
    volatile int time;
}SCREENSAVER;
#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kInsertRootId(void);
void kAddUser(const char* pcParameterBuffer);
void kShowAllUser(void);

static void kExecuteSudo( const char* pcParameterBuffer );

// ���� �� �ڵ�
void kStartConsoleShell( void );
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter );
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter );

// Ŀ�ǵ带 ó���ϴ� �Լ�
static void kHelp( const char* pcParameterBuffer );
static void kCls( const char* pcParameterBuffer );
static void kShowTotalRAMSize( const char* pcParameterBuffer );
static void kStringToDecimalHexTest( const char* pcParameterBuffer );
static void kShutdown( const char* pcParamegerBuffer );
static void kHistory( const char* pcParamegerBuffer );
static void kRaiseFault( const char* pcParamegerBuffer );
static void kDummy( const char* pcParamegerBuffer );
static void InitTabValue(char (*tablog)[100], int* CmdCnt, int* TabCnt);
static void kSetTimer( const char* pcParameterBuffer );
static void kWaitUsingPIT( const char* pcParameterBuffer );
static void kReadTimeStampCounter( const char* pcParameterBuffer );
static void kMeasureProcessorSpeed( const char* pcParameterBuffer );
static void kShowDateAndTime( const char* pcParameterBuffer );
static void kCreateTestTask( const char* pcParameterBuffer );
static void kChangeTaskPriority( const char* pcParameterBuffer );
static void kShowTaskList( const char* pcParameterBuffer );
static void kKillTask( const char* pcParameterBuffer );
static void kCPULoad( const char* pcParameterBuffer );
static void kTestMutex( const char* pcParameterBuffer );
static void kCreateThreadTask( void );
static void kTestThread( const char* pcParameterBuffer );
static void kShowMatrix( const char* pcParameterBuffer );
static void kMakeTestTask( const char* pcParameterBuffer );
static void kShowProcessTime( const char* pcParameterBuffer );
static void kShowDyanmicMemoryInformation( const char* pcParameterBuffer );
static void kTestSequentialAllocation( const char* pcParameterBuffer );
static void kTestRandomAllocation( const char* pcParameterBuffer );
static void kRandomAllocationTask( void );
static void kShowHDDInformation( const char* pcParameterBuffer );
static void kReadSector( const char* pcParameterBuffer );
static void kWriteSector( const char* pcParameterBuffer );
static void kMountHDD( const char* pcParameterBuffer );
static void kFormatHDD( const char* pcParameterBuffer );
static void kShowFileSystemInformation( const char* pcParameterBuffer );
static void kCreateFileInRootDirectory( const char* pcParameterBuffer );
static void kDeleteFileInRootDirectory( const char* pcParameterBuffer );
static void kShowRootDirectory( const char* pcParameterBuffer );
static void kWriteDataToFile( const char* pcParameterBuffer );
static void kReadDataFromFile( const char* pcParameterBuffer );
static void kTestFileIO( const char* pcParameterBuffer );
static void kFlushCache( const char* pcParameterBuffer );
static void kTestPerformance( const char* pcParameterBuffer );
static void kEditor( const char* pcParameterBuffer );
static void kMakeDirectory( const char* pcParamegerBuffer);
static void kShowDirectory( const char* pcParameterBuffer );
static void kRemoveDirectory( const char* pcParameterBuffer);
static void kMoveDirectory( const char* pcParamegerBuffer);
static void kTaskManager( const char* pcParameterBuffer );
static void kKill( const char* pcParameterBuffer );
static void kShowAllFile(void);
static void kChFileMod(const char* pcParameterBuffer);
static void kReservejob( const char* pcParamegerBuffer );
void kRunreservedtask(void);

void kScreenSaver(void);
void kSetScreenSaver(void);
static void kDropScreensSaver( void );
extern SCREENSAVER screensaver;
#endif /*__CONSOLESHELL_H__*/
