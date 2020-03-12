#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "Task.h"
#include "AssemblyUtility.h"
#include "Synchronization.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"
#include "vi.h"
// Ŀ�ǵ� ���̺� ����
COMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "history", "Show history", kHistory}, 
        { "dummyabb", "dummyabb", kDummy},
        { "dummyacc", "dummyacc", kDummy}, 
        { "dummyb", "dummyb", kDummy},
        { "dummyc", "dummyc", kDummy},

        { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
        { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
        { "date", "Show Date And Time", kShowDateAndTime },
        { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },

        { "tasklist", "Show Task List", kShowTaskList },

        { "kill", "End Task, ", kKill },
        { "cpuload", "Show Processor Load", kCPULoad },
	    { "testthread", "Test Thread And Process Function", kTestThread },
        { "showmatrix", "Show Matrix Screen", kShowMatrix },
        { "maketask", "Make task", kMakeTestTask },

        { "taskmanager", "Task Manager", kTaskManager},            
        { "dynamicmeminfo", "Show Dyanmic Memory Information", kShowDyanmicMemoryInformation },
        { "testseqalloc", "Test Sequential Allocation & Free", kTestSequentialAllocation },
        { "testranalloc", "Test Random Allocation & Free", kTestRandomAllocation },
        { "hddinfo", "Show HDD Information", kShowHDDInformation },
        { "readsector", "Read HDD Sector, ex)readsector 0(LBA) 10(count)", 
                kReadSector },
        { "writesector", "Write HDD Sector, ex)writesector 0(LBA) 10(count)", 
                kWriteSector },
        { "mounthdd", "Mount HDD", kMountHDD },
        { "filesysteminfo", "Show File System Information", kShowFileSystemInformation },
        { "createfile", "Create File, ex)createfile a.txt", kCreateFileInRootDirectory },
        { "deletefile", "Delete File, ex)deletefile a.txt", kDeleteFileInRootDirectory },
        { "dir", "Show Directory", kShowRootDirectory },
        { "writefile", "Write Data To File, ex) writefile a.txt", kWriteDataToFile },
        { "readfile", "Read Data From File, ex) readfile a.txt", kReadDataFromFile },
        { "testfileio", "Test File I/O Function", kTestFileIO },
        { "testperformance", "Test File Read/WritePerformance", kTestPerformance },
        { "flush", "Flush File System Cache", kFlushCache },
        { "vi", "Code Editor", kEditor },
        { "mkdir", "Make Directory, ex) mkdir folder", kMakeDirectory},
        { "ls", "Show Directory", kShowDirectory },
        { "cd", "Move Directory, ex) cd folder", kMoveDirectory},
        { "rmdir", "Remove emptyed Directory ex) rmdir folder", kRemoveDirectory},
        {"showallfileinfo", "Show All Directory and File, Authorization",kShowAllFile},
        { "sudo", "Execute Root Command", kExecuteSudo },
        { "reservetask", "Reserve Your Job", kReservejob},

};       

COMMANDENTRY gs_sudoCommandTable[] =
{
	    { "raisefault", "Raisefault", kRaiseFault},
		{ "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer },
		{ "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
		{ "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)", kChangeTaskPriority },
		{ "killtask", "End Task, ex)killtask 1(ID) or 0xffffffff (All Task) ", kKillTask },
		{ "formathdd", "Format HDD", kFormatHDD },
		{ "adduser", "Add User", kAddUser },
        {"showalluser","Show All User",kShowAllUser},
		{"chmod","Change File Mode",kChFileMod},
};

char log[10][100];
char tablog[5][100];
char* commandp;
int history = 0;
char ID[ 50 ];
char PWD[20];

char vcHour[ 30 ];
char vcMinute[ 30 ];
char vcSecond [ 30 ];

char path[100] = "/";
DWORD currentDirectoryClusterIndex = 0;

FUSER* userinfo [5];
int usernumber = 1;
SCREENSAVER screensaver;
void kInsertRootId(void)
{
    char ID[30];
    char Password[30];
    char IDIndex = 0;
    char PassIndex = 0;
    BYTE bKey;
	int x, y;

    userinfo[0] = kOpenUserFile("RootInfo","w",0,"root","qwer",4,4);
	fclose(userinfo[0]);

	while(1)
	{
		IDIndex = 0;
		PassIndex = 0;
        
		kPrintf("ID : ");
		while((bKey = kGetCh()) != KEY_ENTER)
        {
			switch(bKey)
			{
				case 8:
					kGetCursor(&x, &y);
					if(x > kStrLen("ID : "))
					{
						ID[--IDIndex] = '\0';
						kSetCursor(--x, y);
						kPrintStringXY(x, y, " ");
					}
					break;
				default:
            		kPrintf("%c", bKey);
            		ID[IDIndex++] = bKey;
			}
        }
        ID[IDIndex] = '\0';
        
        kPrintf("\nPassword : ");
        while((bKey = kGetCh()) != KEY_ENTER)
        {
			switch(bKey)
			{
				case 8:
					kGetCursor(&x, &y);
					if(x > kStrLen("Password : "))
					{
						Password[--PassIndex] = '\0';
						kSetCursor(--x, y);
						kPrintStringXY(x, y, " ");
					}
					break;
				default:
            		Password[PassIndex++] = bKey;
            		kPrintf("%c", bKey);
			}
        }
        Password[PassIndex] = '\0';

        
        if(kMemCmp(userinfo[0] -> Id, ID, kStrLen(ID) + 1) == 0 && kMemCmp(userinfo[0] -> Passwd, Password, kStrLen(Password)+1) == 0)
        {
			kCls(NULL);
            kPrintf("Wellcome!\n");
            break;
        }
        else
        {
            kPrintf("Wrong Password\n");
        }
    }
    kStartConsoleShell();
}

static void kExecuteSudo( const char* pcParameterBuffer )
{
	PARAMETERLIST stList;
	char Command[30];
	char Password[30];
	char PassIndex = 0;

	BYTE bKey;

	kPrintf("Insert Password for Root : ");
	bKey = kGetCh();
	while(bKey != KEY_ENTER)
	{
		Password[PassIndex++] = bKey;
		//kPrintf("%c", bKey);
		bKey = kGetCh();
	}
	Password[PassIndex] = '\0';

	
	if(kMemCmp(userinfo[0] -> Passwd, Password, kStrLen(Password)) == 0)
	{
		kPrintf("\n");
		kExecuteCommand(pcParameterBuffer , gs_sudoCommandTable, sizeof(gs_sudoCommandTable) / sizeof(COMMANDENTRY), NULL);
	}
	else
	{
		kPrintf("Wrong Password\n");
	}
}


void kAddUser(const char* pcParameterBuffer){

    PARAMETERLIST stList;
    int iLength;
    int iEnterCount;
    int i = 0;
    BYTE bKey;
    BYTE		vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
	int			iCommandBufferIndex = 0;
    int			iCursorX, iCursorY;
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, ID );
    ID[ iLength ] = '\0';

    kPrintf("Insert Password for User : ");
    while (1){


        bKey = kGetCh();
        PWD[i] = bKey;
        if( bKey == KEY_BACKSPACE )//backspace
        {
            if( iCommandBufferIndex > 0 )
            {
				iCommandBufferIndex--;
				vcCommandBuffer[iCommandBufferIndex] = '\0';
                kGetCursor( &iCursorX, &iCursorY );
            }
        }
        else if( bKey == KEY_ENTER )//enter
        {
            
			if(iCommandBufferIndex > 0)
			{
            	vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
			    iCommandBufferIndex = 0;
                break;
			}
            break;
        }
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) ) {;}
        i++;
    }
    PWD[ i ] = '\0';
    userinfo[usernumber]=(kOpenUserFile(ID,"w", usernumber,ID, PWD,iLength,i));

    fclose(userinfo[usernumber]);

    kMemSet( PWD, '\0', kStrLen(PWD));
    kMemSet( ID, '\0', kStrLen(ID));
    usernumber++;
    kPrintf("\n");
}
void kShowAllUser(void){

    for (int i = 0; i<usernumber;i++){
    kPrintf("\t user %d : %s \t", i, userinfo[i]->Id);
    kPrintf("Password : %s \n",userinfo[i]->Passwd);
    }
    
}
//==============================================================================
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
 */
void kStartConsoleShell( void )
{
    BYTE		vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
	BYTE 		bKey;
	int			iCommandBufferIndex = 0;
    int			iCursorX, iCursorY;
	int	 		j=0, CommandTableCnt=0, TabCnt=0, CmdCnt = 0, TabCheck = 0;
	int			logcur=-1, logcnt=0;
	int			klogIndex=0, kUpCheck=0, kDownCheck=0, kSB = 0;
	CHARACTER*	pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    
    
    kPrintf(CONSOLESHELL_PROMPTMESSAGE);
    kPrintf(path);
    kPrintf(">");
    kSetClusterIndex(currentDirectoryClusterIndex);
    kScreenSaver();
	while( 1 )
    {

        bKey = kGetCh();

        if ((TabCnt == 1) && (bKey != KEY_TAB))
			InitTabValue(tablog, &CmdCnt, &TabCnt);

        if( bKey == KEY_BACKSPACE )//backspace
        {
            if( iCommandBufferIndex > 0 )
            {
				iCommandBufferIndex--;
				vcCommandBuffer[iCommandBufferIndex] = '\0';
                kGetCursor( &iCursorX, &iCursorY );
				kSetCursor(iCursorX-1, iCursorY);
				kPrintStringXY(iCursorX-1, iCursorY, " ");
            }
        }
        else if( bKey == KEY_ENTER )//enter
        {
            kPrintf( "\n" );
            
			if(iCommandBufferIndex > 0)
			{
            	vcCommandBuffer[ iCommandBufferIndex ] = '\0';

				logcnt++;
				if(logcnt > 10 && logcnt % 10 == 1) logcur = -1;
				    logcur++;

                kMemSet(log[logcur], '\0', kStrLen(log[logcur]));

				for(int i=0; i<iCommandBufferIndex; i++)
					log[logcur][i] = vcCommandBuffer[i];
			
				klogIndex = logcur+1;
				kUpCheck = 0;
				kDownCheck = 0;
                kSB = 0;
                history++;
                kExecuteCommand( vcCommandBuffer, gs_vstCommandTable, sizeof(gs_vstCommandTable) / sizeof(COMMANDENTRY), gs_sudoCommandTable );//execute commmand
			}
			kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
            kPrintf(path);
            kPrintf(">");                     
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
			iCommandBufferIndex = 0;
        }
		//ignore Shift, NumLock, Caps Lock, Scroll Lock
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) ) {;}
        else if(bKey == KEY_UP || bKey == KEY_DOWN)
		{
			if(bKey == KEY_UP)
			{
				klogIndex--;
                if(kSB == 1){
                    kSB = 0;
                    klogIndex++;
                }
				else if(logcnt < 10 && klogIndex < 0) klogIndex = 0;
				else if(logcnt % 10 == 0 && klogIndex < 0) klogIndex = 0;
				else if(logcnt % 10 != 0)
				{
					if(klogIndex < 0) klogIndex = 9;
					else if(klogIndex == logcur && kUpCheck == 1) klogIndex++;
				}
				kUpCheck = 1;
				commandp = log[klogIndex];
			}
			else
			{
				klogIndex++;
				if (klogIndex-logcur == 2 && kDownCheck == 0){
                    klogIndex = logcur;
                    commandp = "";
                }
				else if(klogIndex-logcur == 1){ 
                    klogIndex--;
                    kSB = 1;
                    commandp = "";
                }
				else if(klogIndex == 10 && logcnt > 10){
                    klogIndex = 0;
                    commandp = log[klogIndex];
                }
                else
				    commandp = log[klogIndex];
                
			}
			kDownCheck = 1;
			kGetCursor(&iCursorX, &iCursorY);
			for(int i=kStrLen(path)+8; i < CONSOLE_WIDTH; i++)
			{
				pstScreen[(iCursorY)*CONSOLE_WIDTH+i].bCharactor = ' ';
				pstScreen[(iCursorY)*CONSOLE_WIDTH+i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			}
			kSetCursor(kStrLen(path)+8, iCursorY);
			kPrintf("%s", commandp);
			j = 0;
			while(commandp[j] != '\0')
			{
				vcCommandBuffer[j] = commandp[j];
				j++;
			}
			iCommandBufferIndex = j;
		}
        else if(bKey==KEY_RIGHT)
        {
			kGetCursor( &iCursorX, &iCursorY);
			if (iCommandBufferIndex+7 != iCursorX)
				kSetCursor(iCursorX+1, iCursorY);
	
		}
		else if (bKey == KEY_LEFT)
		{
			kGetCursor(&iCursorX, &iCursorY);
			if(7<iCursorX)
				kSetCursor(iCursorX-1, iCursorY);
		}
		else if( bKey == KEY_TAB )//tab
		{	
			TabCnt++;

			if(TabCnt == 1)
			{
				if(iCommandBufferIndex > 0)
				{
				 	CommandTableCnt = sizeof(gs_vstCommandTable) / sizeof(COMMANDENTRY);
		       		for(int i=0; i<CommandTableCnt; i++)
					{
						if(kMemCmp(gs_vstCommandTable[i].pcCommand, vcCommandBuffer, iCommandBufferIndex)==0)
						{		
							kMemCpy(tablog[CmdCnt], gs_vstCommandTable[i].pcCommand, kStrLen(gs_vstCommandTable[i].pcCommand));
							CmdCnt++;
						}
					}
                    TabCheck = 0;
                    j = 0;
                    while(j < kStrLen(tablog[0]))
                    {
                        for(int i=0; i<CmdCnt; i++)
                            if(tablog[i][j] != tablog[0][j])
                            {
                                TabCheck = 1;
                                break;
                            }
                        if(TabCheck == 1)
                            break;
                        j++;
                    }
                    if(CmdCnt == 1)
                    {
                        TabCheck = 1;
                        j = kStrLen(tablog[0]);
                    }
					if(TabCheck == 1)
                    {
                        for(int i=0; i<j; i++)
                            vcCommandBuffer[i] = tablog[0][i];
						iCommandBufferIndex = j;				
						kGetCursor(&iCursorX, &iCursorY);
						kSetCursor(kStrLen(path)+8, iCursorY);
						kPrintf("%s",vcCommandBuffer);
					}
				}
			} 	//tabcnt = 1
			else
			{
				j = 0;
				while (j<CmdCnt)
				{
					if(kMemCmp(vcCommandBuffer, tablog[j],kStrLen(tablog[j]))==0)
						break;
					j++;
				}
				if(j == CmdCnt-1)//completed command
				{
					InitTabValue(tablog, &CmdCnt, &TabCnt);
					continue;
				}
				if(iCommandBufferIndex > 0)
				{
					for (int i = 0; i<CmdCnt;i++)
						kPrintf("\n%s",tablog[i]);
					if(CmdCnt > 1)
					    kPrintf("\n%s%s", CONSOLESHELL_PROMPTMESSAGE, vcCommandBuffer);
					InitTabValue(tablog, &CmdCnt, &TabCnt);
				}
			}
		}
		else
		{
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

void InitTabValue(char (*tablog)[100], int* CmdCnt, int* TabCnt)
{
	for(int i=0; i<*CmdCnt; i++)
		kMemSet(tablog[i], '\0', kStrLen(tablog[i]));
	*CmdCnt = 0;
	*TabCnt = 0;
}
/**
 *  �Ķ���� �ڷᱸ���� �ʱ�ȭ
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������ ����� ���̸� ��ȯ
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ� ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �Ķ���͸� �����ϰ� ���̸� ��ȯ
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������ ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���
 */
static void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    
    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( COMMANDENTRY );

    // ���� �� Ŀ�ǵ��� ���̸� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ���� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // ����� ���� ��� ������ ������
        if( ( i != 0 ) && ( ( i % 20 ) == 0 ) )
        {
            kPrintf( "Press any key to continue... ('q' is exit) : " );
            if( kGetCh() == 'q' )
            {
                kPrintf( "\n" );
                break;
            }        
            kPrintf( "\n" );
        }
    }
}

/**
 *  ȭ���� ���� 
 */
static void kCls( const char* pcParameterBuffer )
{
    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  �� �޸� ũ�⸦ ���
 */
static void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���
 */
static void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // ���� �Ķ���͸� ����, �Ķ������ ���̰� 0�̸� �Ķ���Ͱ� ���� ���̹Ƿ�
        // ����
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // �Ķ���Ϳ� ���� ������ ����ϰ� 16�������� 10�������� �Ǵ��Ͽ� ��ȯ�� ��
        // ����� printf�� ���
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0x�� �����ϸ� 16����, �׿ܴ� 10������ �Ǵ�
        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }
        
        iCount++;
    }
}

/**
 *  PC�� �����(Reboot)
 */
static void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // ���� �ý��� ĳ�ÿ� ����ִ� ������ �ϵ� ��ũ�� �ű�
    kPrintf( "Cache Flush... ");
    if( kFlushFileSystemCache() == TRUE )
    {
        kPrintf( "Pass\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
    
    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}
// history
static void kHistory( const char* pcParamegerBuffer )
{   
    if(history < 11){
        for(int i = 0; i < history; i++)
            kPrintf("%d %s\n",  i+1,  log[i]);
    }
    else
    {
        for(int i = 0; i < 10; i++)
            kPrintf("%d %s\n",  i+1,  log[(history+i)%10]);
    }
    
}
// fault �߻� �κ�
static void kRaiseFault( const char* pcParamegerBuffer )
{
    long *ptr = (long*)0x1ff000;
	*ptr = 0;
}

static void kDummy( const char* pcParamegerBuffer )
{
    kPrintf( "This is Dummy\n");

}

/**
 *  PIT ��Ʈ�ѷ��� ī���� 0 ����
 */
static void kSetTimer( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    
    // milisecond ����
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    lValue = kAToI( vcParameter, 10 );

    // Periodic ����
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }    
    bPeriodic = kAToI( vcParameter, 10 );
    
    kInitializePIT( MSTOCOUNT( lValue ), bPeriodic );
    kPrintf( "Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic );
}

/**
 *  PIT ��Ʈ�ѷ��� ���� ����Ͽ� ms ���� ���  
 */
static void kWaitUsingPIT( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;
    
    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)wait 100(ms)\n" );
        return ;
    }
    
    lMillisecond = kAToI( pcParameterBuffer, 10 );
    kPrintf( "%d ms Sleep Start...\n", lMillisecond );
    
    // ���ͷ�Ʈ�� ��Ȱ��ȭ�ϰ� PIT ��Ʈ�ѷ��� ���� ���� �ð��� ����
    kDisableInterrupt();
    for( i = 0 ; i < lMillisecond / 30 ; i++ )
    {
        kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
    }
    kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );   
    kEnableInterrupt();
    kPrintf( "%d ms Sleep Complete\n", lMillisecond );
    
    // Ÿ�̸� ����
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

/**
 *  Ÿ�� ������ ī���͸� ����  
 */
static void kReadTimeStampCounter( const char* pcParameterBuffer )
{
    QWORD qwTSC;
    
    qwTSC = kReadTSC();
    kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

/**
 *  ���μ����� �ӵ��� ����
 */
static void kMeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    kPrintf( "Now Measuring." );
    
    // 10�� ���� ��ȭ�� Ÿ�� ������ ī���͸� �̿��Ͽ� ���μ����� �ӵ��� ���������� ����
    kDisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf( "." );
    }
    // Ÿ�̸� ����
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );    
    kEnableInterrupt();
    
    kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC ��Ʈ�ѷ��� ����� ���� �� �ð� ������ ǥ��
 */
static void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC ��Ʈ�ѷ����� �ð� �� ���ڸ� ����
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �½�ũ 1
 *      ȭ�� �׵θ��� ���鼭 ���ڸ� ���
 */
static void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    
    // �ڽ��� ID�� �� ȭ�� ���������� ���
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;
    
    // ȭ�� �� �����̸� ���鼭 ���� ���
    for( j = 0 ; j < 20000 ; j++ )
    {
        switch( i )
        {
        case 0:
            iX++;
            if( iX >= ( CONSOLE_WIDTH - iMargin ) )
            {
                i = 1;
            }
            break;
            
        case 1:
            iY++;
            if( iY >= ( CONSOLE_HEIGHT - iMargin ) )
            {
                i = 2;
            }
            break;
            
        case 2:
            iX--;
            if( iX < iMargin )
            {
                i = 3;
            }
            break;
            
        case 3:
            iY--;
            if( iY < iMargin )
            {
                i = 0;
            }
            break;
        }
        
        // ���� �� ���� ����
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;
        
        // �ٸ� �½�ũ�� ��ȯ
        // kSchedule();
    }
    kExitTask();

}

/**
 *  �½�ũ 2
 *      �ڽ��� ID�� �����Ͽ� Ư�� ��ġ�� ȸ���ϴ� �ٶ����� ���
 */
static void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };
    QWORD qwLastSpendTickInIdleTask, qwLastMeasureTickCount, qwCurrentMeasureTickCount, qwCurrentSpendTickInIdleTask;
    // �ڽ��� ID�� �� ȭ�� ���������� ���
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - 
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    // ���μ��� ��뷮 ����� ���� ���� ������ ����
    qwLastSpendTickInIdleTask = pstRunningTask->qwSpendProcessorTimeInIdleTask;
    qwLastMeasureTickCount = kGetTickCount();

    while( 1 )
    {   
        qwCurrentMeasureTickCount = kGetTickCount();
        qwCurrentSpendTickInIdleTask = pstRunningTask->qwSpendProcessorTimeInIdleTask;;
        
        // ���μ��� ��뷮�� ���
        // 100 - ( ���� �½�ũ�� ����� ���μ��� �ð� ) * 100 / ( �ý��� ��ü���� 
        // ����� ���μ��� �ð� )
        if( qwCurrentMeasureTickCount - qwLastMeasureTickCount == 0 )
        {
            pstRunningTask->qwProcessorLoad = 0;
        }
        else
        {
            pstRunningTask->qwProcessorLoad = ( qwCurrentSpendTickInIdleTask - qwLastSpendTickInIdleTask ) * 
                100 /( qwCurrentMeasureTickCount - qwLastMeasureTickCount );
        }

        // ȸ���ϴ� �ٶ����� ǥ��
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        // ���� ����
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;
        
        // �ٸ� �½�ũ�� ��ȯ
        //kSchedule();
    }
}

/**
 *  �½�ũ�� �����ؼ� ��Ƽ �½�ŷ ����
 */
static void kCreateTestTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcType[ 30 ];
    char vcCount[ 30 ];
    char vcPriority [ 30 ];
    int i;
    int bPriority;
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcType );
    kGetNextParameter( &stList, vcCount );
    kGetNextParameter( &stList, vcPriority );
    bPriority = vcPriority[0] - 48;
    // kPrintf("%d", bPriority);
    switch( kAToI( vcType, 10 ) )
    {
    // Ÿ�� 1 �½�ũ ����
    case 1:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {    
            if( kCreateTask( bPriority | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask1 ) == NULL )
            {
                break;
            }
        }
        
        kPrintf( "Task1 %d Created\n", i );
        break;
        
    // Ÿ�� 2 �½�ũ ����
    case 2:
    default:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {
            if( kCreateTask( bPriority | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 ) == NULL )
            {
                break;
            }
        }
        kPrintf( "Task2 %d Created\n", i );
        break;
    }
}   
 
/**
 *  �½�ũ�� �켱 ������ ����
 */
static void kChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    kGetNextParameter( &stList, vcPriority );
    
    // �½�ũ�� �켱 ������ ����
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }
    
    bPriority = kAToI( vcPriority, 10 );
    
    kPrintf( "Change Task Priority ID [0x%q] Priority[%d] ", qwID, bPriority );
    if( kChangePriority( qwID, bPriority ) == TRUE )
    {
        kPrintf( "Success\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
}

/**
 *  ���� ������ ��� �½�ũ�� ������ ���
 */
static void kShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    
    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        // TCB�� ���ؼ� TCB�� ��� ���̸� ID�� ���
        pstTCB = kGetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �½�ũ�� 10�� ��µ� ������, ��� �½�ũ ������ ǥ������ ���θ� Ȯ��
            if( ( iCount != 0 ) && ( ( iCount % 10 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
            }
            
            kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", 1 + iCount++,
                     pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ), 
                     pstTCB->qwFlags, kGetListCount( &( pstTCB->stChildThreadList ) ) );

            kPrintf( "    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n",
                    pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize );
        }
    }
}

/**
 *  �½�ũ�� ����
 */
static void kKill( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    int bcID;
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    
    bcID = vcID[0] - 48;
    
    

    pstTCB = kGetTCBInTCBPool(bcID + 1);
    qwID = pstTCB->stLink.qwID;
    kPrintf("%Q", qwID);
    // �ý��� �׽�Ʈ�� ����
    if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
    {
        kPrintf( "Kill Task ID [0x%q] ", qwID );
        if( kEndTask( qwID ) == TRUE )
        {
            kPrintf( "Success\n" );
        }
        else
        {
            kPrintf( "Fail\n" );
        }
    }
    else
    {
        kPrintf( "Task does not exist or task is system task\n" );
    }
    
}
/**
 *  �½�ũ�� ����
 */
static void kKillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    
    // �½�ũ�� ����
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }
    
    // Ư�� ID�� �����ϴ� ���
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �ý��� �׽�Ʈ�� ����
        if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
        {
            kPrintf( "Kill Task ID [0x%q] ", qwID );
            if( kEndTask( qwID ) == TRUE )
            {
                kPrintf( "Success\n" );
            }
            else
            {
                kPrintf( "Fail\n" );
            }
        }
        else
        {
            kPrintf( "Task does not exist or task is system task\n" );
        }
    }
    // �ܼ� �а� ���� �½�ũ�� �����ϰ� ��� �½�ũ ����
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = kGetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �ý��� �׽�Ʈ�� ���� ��Ͽ��� ����
            if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
            {
                kPrintf( "Kill Task ID [0x%q] ", qwID );
                if( kEndTask( qwID ) == TRUE )
                {
                    kPrintf( "Success\n" );
                }
                else
                {
                    kPrintf( "Fail\n" );
                }
            }
        }
    }
}

/**
 *  ���μ����� ������ ǥ��
 */
static void kCPULoad( const char* pcParameterBuffer )
{
    kPrintf( "Processor Load : %d%%\n", kGetProcessorLoad() );
}
    
// ���ؽ� �׽�Ʈ�� ���ؽ��� ����
static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ
 */
static void kPrintNumberTask( void )
{
    int i;
    int j;
    QWORD qwTickCount;

    // 50ms ���� ����Ͽ� �ܼ� ���� ����ϴ� �޽����� ��ġ�� �ʵ��� ��
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 50 )
    {
        kSchedule();
    }    
    
    // ������ ���鼭 ���ڸ� ���
    for( i = 0 ; i < 5 ; i++ )
    {
        kLock( &( gs_stMutex ) );
        kPrintf( "Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID,
                gs_qwAdder );
        
        gs_qwAdder += 1;
        kUnlock( & ( gs_stMutex ) );
    
        // ���μ��� �Ҹ� �ø����� �߰��� �ڵ�
        for( j = 0 ; j < 30000 ; j++ ) ;
    }
    
    // ��� �½�ũ�� ������ ������ 1��(100ms) ���� ���
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 1000 )
    {
        kSchedule();
    }    
    
    // �½�ũ ����
    //kExitTask();
}

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ ����
 */
static void kTestMutex( const char* pcParameterBuffer )
{
    int i;
    
    gs_qwAdder = 1;
    
    // ���ؽ� �ʱ�ȭ
    kInitializeMutex( &gs_stMutex );
    
    for( i = 0 ; i < 3 ; i++ )
    {
        // ���ؽ��� �׽�Ʈ�ϴ� �½�ũ�� 3�� ����
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kPrintNumberTask );
    }    
    kPrintf( "Wait Util %d Task End...\n", i );
    kGetCh();
}

/**
 *  �½�ũ 2�� �ڽ��� ������� �����ϴ� �½�ũ
 */
static void kCreateThreadTask( void )
{
    int i;
    
    for( i = 0 ; i < 3 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 );
    }
    
    while( 1 )
    {
        kSleep( 1 );
    }
}

/**
 *  �����带 �׽�Ʈ�ϴ� �½�ũ ����
 */
static void kTestThread( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_PROCESS, ( void * )0xEEEEEEEE, 0x1000, 
                              ( QWORD ) kCreateThreadTask );
    if( pstProcess != NULL )
    {
        kPrintf( "Process [0x%Q] Create Success\n", pstProcess->stLink.qwID ); 
    }
    else
    {
        kPrintf( "Process Create Fail\n" );
    }
}

// ������ �߻���Ű�� ���� ����
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  ������ ������ ��ȯ
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  ö�ڸ� �귯������ �ϴ� ������
 */
static void kDropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // ��� �����
        kSleep( kRandom() % 20 );
        
        if( ( kRandom() % 20 ) < 16 )
        {
            vcText[ 0 ] = ' ';
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                kPrintStringXY( iX, i , vcText );
                kSleep( 50 );
            }
        }        
        else
        {
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                vcText[ 0 ] = i + kRandom();
                kPrintStringXY( iX, i, vcText );
                kSleep( 50 );
            }
        }
    }
}

/**
 *  �����带 �����Ͽ� ��Ʈ���� ȭ��ó�� �����ִ� ���μ���
 */
static void kMatrixProcess( void )
{
    int i;
    
    for( i = 0 ; i < 300 ; i++ )
    {
        if( kCreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, 
                         ( QWORD ) kDropCharactorThread ) == NULL )
        {
            break;
        }
        
        kSleep( kRandom() % 5 + 5 );
    }
    
    kPrintf( "%d Thread is created\n", i );

    // Ű�� �ԷµǸ� ���μ��� ����
    kGetCh();
}

/**
 *  ��Ʈ���� ȭ���� ������
 */
static void kShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) kMatrixProcess );
    if( pstProcess != NULL )
    {
        kPrintf( "Matrix Process [0x%Q] Create Success\n" );

        // �½�ũ�� ���� �� ������ ���
        while( ( pstProcess->stLink.qwID >> 32 ) != 0 )
        {
            kSleep( 100 );
        }
    }
    else
    {
        kPrintf( "Matrix Process Create Fail\n" );
    }
}

static void kMakeTestTask( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    for( i = 1 ; i < 4 ; i++ )
    {    
        if( kCreateTask( i | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 ) == NULL )
        {
            break;
        }
        if( kCreateTask( i | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 ) == NULL )
        {
            break;
        }
    }
    kPrintf( "Task2 %d Created\n", (i -1)*2);

}
//kch 
static void kTaskManager( const char* pcParameterBuffer ){
    kCreateTask( 4 | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kShowProcessTime );
}

static void kShowProcessTime( const char* pcParameterBuffer ){
    int i, j;
    int cls = 0;
    TCB* pstTCB;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    kCls(NULL);
    QWORD temp = kGetTickCount() / 1000;
    while(1){
        
        QWORD tickCount = kGetTickCount() / 1000;
        if(tickCount > temp){
            kCls(NULL);
            kSetCursor( 0, 1);
            cls = 0;
            kPrintf( "machine time = %ds\n", tickCount);
            temp = tickCount;
            for( i = 2 ; i < TASK_MAXCOUNT  ; i++ )
            {
                // TCB�� ���ؼ� TCB�� ��� ���̸� ID�� ���
                pstTCB = kGetTCBInTCBPool( i );
                if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
                {
                    kPrintf( "[%d] Task ID[0x%Q], CPU Usage[%d%%]\n", i - 1,pstTCB->stLink.qwID, pstTCB->qwProcessorLoad );
                    cls++;
                }
            }
        }
    }
}



/**
 *  ���� �޸� ������ ǥ��
 */
static void kShowDyanmicMemoryInformation( const char* pcParameterBuffer )
{
    QWORD qwStartAddress, qwTotalSize, qwMetaSize, qwUsedSize;
    
    kGetDynamicMemoryInformation( &qwStartAddress, &qwTotalSize, &qwMetaSize, 
            &qwUsedSize );

    kPrintf( "============ Dynamic Memory Information ============\n" );
    kPrintf( "Start Address: [0x%Q]\n", qwStartAddress );
    kPrintf( "Total Size:    [0x%Q]byte, [%d]MB\n", qwTotalSize, 
            qwTotalSize / 1024 / 1024 );
    kPrintf( "Meta Size:     [0x%Q]byte, [%d]KB\n", qwMetaSize, 
            qwMetaSize / 1024 );
    kPrintf( "Used Size:     [0x%Q]byte, [%d]KB\n", qwUsedSize, qwUsedSize / 1024 );
}
/**
 *  ��� ��� ����Ʈ�� ����� ���������� �Ҵ��ϰ� �����ϴ� �׽�Ʈ
 */
static void kTestSequentialAllocation( const char* pcParameterBuffer )
{
    DYNAMICMEMORY* pstMemory;
    long i, j, k;
    QWORD* pqwBuffer;
    
    kPrintf( "============ Dynamic Memory Test ============\n" );
    pstMemory = kGetDynamicMemoryManager();
    
    for( i = 0 ; i < pstMemory->iMaxLevelCount ; i++ )
    {
        kPrintf( "Block List [%d] Test Start\n", i );
        kPrintf( "Allocation And Compare: ");
        
        // ��� ����� �Ҵ� �޾Ƽ� ���� ä�� �� �˻�
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            pqwBuffer = kAllocateMemory( DYNAMICMEMORY_MIN_SIZE << i );
            if( pqwBuffer == NULL )
            {
                kPrintf( "\nAllocation Fail\n" );
                return ;
            }

            // ���� ä�� �� �ٽ� �˻�
            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                pqwBuffer[ k ] = k;
            }
            
            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                if( pqwBuffer[ k ] != k )
                {
                    kPrintf( "\nCompare Fail\n" );
                    return ;
                }
            }
            // ���� ������ . ���� ǥ��
            kPrintf( "." );
        }
        
        kPrintf( "\nFree: ");
        // �Ҵ� ���� ����� ��� ��ȯ
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            if( kFreeMemory( ( void * ) ( pstMemory->qwStartAddress + 
                         ( DYNAMICMEMORY_MIN_SIZE << i ) * j ) ) == FALSE )
            {
                kPrintf( "\nFree Fail\n" );
                return ;
            }
            // ���� ������ . ���� ǥ��
            kPrintf( "." );
        }
        kPrintf( "\n" );
    }
    kPrintf( "Test Complete~!!!\n" );
}

/**
 *  ���Ƿ� �޸𸮸� �Ҵ��ϰ� �����ϴ� ���� �ݺ��ϴ� �½�ũ
 */
static void kRandomAllocationTask( void )
{
    TCB* pstTask;
    QWORD qwMemorySize;
    char vcBuffer[ 200 ];
    BYTE* pbAllocationBuffer;
    int i, j;
    int iY;
    
    pstTask = kGetRunningTask();
    iY = ( pstTask->stLink.qwID ) % 15 + 9;

    for( j = 0 ; j < 10 ; j++ )
    {
        // 1KB ~ 32M���� �Ҵ��ϵ��� ��
        do
        {
            qwMemorySize = ( ( kRandom() % ( 32 * 1024 ) ) + 1 ) * 1024;
            pbAllocationBuffer = kAllocateMemory( qwMemorySize );

            // ���� ���۸� �Ҵ� ���� ���ϸ� �ٸ� �½�ũ�� �޸𸮸� ����ϰ� 
            // ���� �� �����Ƿ� ��� ����� �� �ٽ� �õ�
            if( pbAllocationBuffer == 0 )
            {
                kSleep( 1 );
            }
        } while( pbAllocationBuffer == 0 );
            
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Allocation Success", 
                  pbAllocationBuffer, qwMemorySize );
        // �ڽ��� ID�� Y ��ǥ�� �Ͽ� �����͸� ���
        kPrintStringXY( 20, iY, vcBuffer );
        kSleep( 200 );
        
        // ���۸� ������ ������ ������ �����͸� �Ȱ��� ä�� 
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Write...     ", 
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            pbAllocationBuffer[ i ] = kRandom() & 0xFF;
            pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] = pbAllocationBuffer[ i ];
        }
        kSleep( 200 );
        
        // ä�� �����Ͱ� ���������� �ٽ� Ȯ��
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Verify...   ", 
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            if( pbAllocationBuffer[ i ] != pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] )
            {
                kPrintf( "Task ID[0x%Q] Verify Fail\n", pstTask->stLink.qwID );
                kExitTask();
            }
        }
        kFreeMemory( pbAllocationBuffer );
        kSleep( 200 );
    }
    
    kExitTask();
}

/**
 *  �½�ũ�� ���� �� �����Ͽ� ������ �޸𸮸� �Ҵ��ϰ� �����ϴ� ���� �ݺ��ϴ� �׽�Ʈ
 */
static void kTestRandomAllocation( const char* pcParameterBuffer )
{
    int i;
    
    for( i = 0 ; i < 1000 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kRandomAllocationTask );
    }
}
/**
 *  �ϵ� ��ũ�� ������ ǥ��
 */
static void kShowHDDInformation( const char* pcParameterBuffer )
{
    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // �ϵ� ��ũ�� ������ ����
    if( kGetHDDInformation( &stHDD ) == FALSE )
    {
        kPrintf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    kPrintf( "============ Primary Master HDD Information ============\n" );
    
    // �� ��ȣ ���
    kMemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    kPrintf( "Model Number:\t %s\n", vcBuffer );
    
    // �ø��� ��ȣ ���
    kMemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    kPrintf( "Serial Number:\t %s\n", vcBuffer );

    // ���, �Ǹ���, �Ǹ��� �� ���� ���� ���
    kPrintf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    kPrintf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    kPrintf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // �� ���� �� ���
    kPrintf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );
}

/**
 *  �ϵ� ��ũ�� �Ķ���ͷ� �Ѿ�� LBA ��巹������ ���� �� ��ŭ ����
 */
static void kReadSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BYTE bData;
    BOOL bExit = FALSE;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹���� ���� �� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );
    
    // ���� ����ŭ �޸𸮸� �Ҵ� �޾� �б� ����
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    if( kReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        kPrintf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // ������ ������ ������ ���
        for( j = 0 ; j < iSectorCount ; j++ )
        {
            for( i = 0 ; i < 512 ; i++ )
            {
                if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
                {
                    kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                    if( kGetCh() == 'q' )
                    {
                        bExit = TRUE;
                        break;
                    }
                }                

                if( ( i % 16 ) == 0 )
                {
                    kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
                }

                // ��� �� �ڸ��� ǥ���Ϸ��� 16���� ���� ��� 0�� �߰�����
                bData = pcBuffer[ j * 512 + i ] & 0xFF;
                if( bData < 16 )
                {
                    kPrintf( "0" );
                }
                kPrintf( "%X ", bData );
            }
            
            if( bExit == TRUE )
            {
                break;
            }
        }
        kPrintf( "\n" );
    }
    else
    {
        kPrintf( "Read Fail\n" );
    }
    
    kFreeMemory( pcBuffer );
}

/**
 *  �ϵ� ��ũ�� �Ķ���ͷ� �Ѿ�� LBA ��巹������ ���� �� ��ŭ ��
 */
static void kWriteSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BOOL bExit = FALSE;
    BYTE bData;
    static DWORD s_dwWriteCount = 0;

    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹���� ���� �� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) writesector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );

    s_dwWriteCount++;
    
    // ���۸� �Ҵ� �޾� �����͸� ä��. 
    // ������ 4 ����Ʈ�� LBA ��巹���� 4 ����Ʈ�� ���Ⱑ ����� Ƚ���� ����
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // ���� ����
    if( kWriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        kPrintf( "Write Fail\n" );
        return ;
    }
    kPrintf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // ������ ������ ������ ���
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i++ )
        {
            if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
            {
                kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    bExit = TRUE;
                    break;
                }
            }                

            if( ( i % 16 ) == 0 )
            {
                kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
            }

            // ��� �� �ڸ��� ǥ���Ϸ��� 16���� ���� ��� 0�� �߰�����
            bData = pcBuffer[ j * 512 + i ] & 0xFF;
            if( bData < 16 )
            {
                kPrintf( "0" );
            }
            kPrintf( "%X ", bData );
        }
        
        if( bExit == TRUE )
        {
            break;
        }
    }
    kPrintf( "\n" );    
    kFreeMemory( pcBuffer );    
}

/**
 *  �ϵ� ��ũ�� ����
 */
static void kMountHDD( const char* pcParameterBuffer )
{
    if( kMount() == FALSE )
    {
        kPrintf( "HDD Mount Fail\n" );
        return ;
    }
    kPrintf( "HDD Mount Success\n" );
}

/**
 *  �ϵ� ��ũ�� ���� �ý����� ����(����)
 */
static void kFormatHDD( const char* pcParameterBuffer )
{
    if( kFormat() == FALSE )
    {
        kPrintf( "HDD Format Fail\n" );
        return ;
    }
    kPrintf( "HDD Format Success\n" );
}

/**
 *  ���� �ý��� ������ ǥ��
 */
static void kShowFileSystemInformation( const char* pcParameterBuffer )
{
    FILESYSTEMMANAGER stManager;
    
    kGetFileSystemInformation( &stManager );
    
    kPrintf( "================== File System Information ==================\n" );
    kPrintf( "Mouted:\t\t\t\t\t %d\n", stManager.bMounted );
    kPrintf( "Reserved Sector Count:\t\t\t %d Sector\n", stManager.dwReservedSectorCount );
    kPrintf( "Cluster Link Table Start Address:\t %d Sector\n", 
            stManager.dwClusterLinkAreaStartAddress );
    kPrintf( "Cluster Link Table Size:\t\t %d Sector\n", stManager.dwClusterLinkAreaSize );
    kPrintf( "Data Area Start Address:\t\t %d Sector\n", stManager.dwDataAreaStartAddress );
    kPrintf( "Total Cluster Count:\t\t\t %d Cluster\n", stManager.dwTotalClusterCount );
}

/**
 *  ��Ʈ ���͸��� �� ������ ����
 */
static void kCreateFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;

    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    pstFile = fopen( vcFileName, "w" );
   
    if( pstFile == NULL )
    {
        kPrintf( "File Create Fail\n" );
        return;
    }
    fclose( pstFile );
    kPrintf( "File Create Success\n" );
}

/**
 *  ��Ʈ ���͸����� ������ ����
 */
static void kDeleteFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    if( remove( vcFileName ) != 0 )
    {
        kPrintf( "File Not Found or File Opened\n" );
        return ;
    }
    
    kPrintf( "File Delete Success\n" );
}

/**
 *  ��Ʈ ���͸��� ���� ����� ǥ��
 */
static void kShowRootDirectory( const char* pcParameterBuffer )
{
    DIR* pstDirectory;
    int i, iCount, iTotalCount;
    struct dirent* pstEntry;
    char vcBuffer[ 400 ];
    char vcTempValue[ 50 ];
    DWORD dwTotalByte;
    DWORD dwUsedClusterCount;
    FILESYSTEMMANAGER stManager;
    
    // ���� �ý��� ������ ����
    kGetFileSystemInformation( &stManager );
     
    // ��Ʈ ���͸��� ��
    pstDirectory = opendir( "/" );
    if( pstDirectory == NULL )
    {
        kPrintf( "Root Directory Open Fail\n" );
        return ;
    }
    
    // ���� ������ ���鼭 ���͸��� �ִ� ������ ������ ��ü ������ ����� ũ�⸦ ���
    iTotalCount = 0;
    dwTotalByte = 0;
    dwUsedClusterCount = 0;
    while( 1 )
    {
        // ���͸����� ��Ʈ�� �ϳ��� ����
        pstEntry = readdir( pstDirectory );
        // ���̻� ������ ������ ����
        if( pstEntry == NULL )
        {
            break;
        }
        iTotalCount++;
        dwTotalByte += pstEntry->dwFileSize;

        // ������ ���� Ŭ�������� ������ ���
        if( pstEntry->dwFileSize == 0 )
        {
            // ũ�Ⱑ 0�̶� Ŭ������ 1���� �Ҵ�Ǿ� ����
            dwUsedClusterCount++;
        }
        else
        {
            // Ŭ������ ������ �ø��Ͽ� ����
            dwUsedClusterCount += ( pstEntry->dwFileSize + 
                ( FILESYSTEM_CLUSTERSIZE - 1 ) ) / FILESYSTEM_CLUSTERSIZE;
        }
    }
    
    // ���� ������ ������ ǥ���ϴ� ����
    rewinddir( pstDirectory );
    iCount = 0;
    while( 1 )
    {
        // ���͸����� ��Ʈ�� �ϳ��� ����
        pstEntry = readdir( pstDirectory );
        // ���̻� ������ ������ ����
        if( pstEntry == NULL )
        {
            break;
        }
        
        // ���� �������� �ʱ�ȭ �� �� �� ��ġ�� ���� ����
        kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
        vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';
        
        // ���� �̸� ����
        kMemCpy( vcBuffer, pstEntry->d_name, 
                 kStrLen( pstEntry->d_name ) );

        // ���� ���� ����
        kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
        kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue ) );

        // ������ ���� Ŭ������ ����
        kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
        kMemCpy( vcBuffer + 55, vcTempValue, kStrLen( vcTempValue ) + 1 );
        kPrintf( "    %s\n", vcBuffer );

        if( ( iCount != 0 ) && ( ( iCount % 20 ) == 0 ) )
        {
            kPrintf( "Press any key to continue... ('q' is exit) : " );
            if( kGetCh() == 'q' )
            {
                kPrintf( "\n" );
                break;
            }        
        }
        iCount++;
    }
    
    // �� ������ ������ ������ �� ũ�⸦ ���
    kPrintf( "\t\tTotal File Count: %d\n", iTotalCount );
    kPrintf( "\t\tTotal File Size: %d KByte (%d Cluster)\n", dwTotalByte, 
             dwUsedClusterCount );
    
    // ���� Ŭ������ ���� �̿��ؼ� ���� ������ ���
    kPrintf( "\t\tFree Space: %d KByte (%d Cluster)\n", 
             ( stManager.dwTotalClusterCount - dwUsedClusterCount ) * 
             FILESYSTEM_CLUSTERSIZE / 1024, stManager.dwTotalClusterCount - 
             dwUsedClusterCount );
    
    // ���͸��� ����
    closedir( pstDirectory );
}

/**
 *  ������ �����Ͽ� Ű����� �Էµ� �����͸� ��
 */
static void kWriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // ���� ����
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // ���� Ű�� �������� 3�� ������ ������ ������ ���Ͽ� ��
    iEnterCount = 0;
    while( 1 )
    {
        bKey = kGetCh();
        // ���� Ű�̸� ���� 3�� �������°� Ȯ���Ͽ� ������ ���� ����
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // ���� Ű�� �ƴ϶�� ���� Ű �Է� Ƚ���� �ʱ�ȭ
        else
        {
            iEnterCount = 0;
        }
        
        kPrintf( "%c", bKey );
        if( fwrite( &bKey, 1, 1, fp ) != 1 )
        {
            kPrintf( "File Wirte Fail\n" );
            break;
        }
    }
    
    kPrintf( "File Create Success\n" );
    fclose( fp );
}

/**
 *  ������ ��� �����͸� ����
 */
static void kReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // ���� ����
    fp = fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // ������ ������ ����ϴ� ���� �ݺ�
    iEnterCount = 0;
    while( 1 )
    {
        if( fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        kPrintf( "%c", bKey );
        
        // ���� ���� Ű�̸� ���� Ű Ƚ���� ������Ű�� 20���α��� ����ߴٸ� 
        // �� ������� ���θ� ���
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            
            if( ( iEnterCount != 0 ) && ( ( iEnterCount % 20 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
                iEnterCount = 0;
            }
        }
    }
    fclose( fp );
    kPrintf("\n");
}

/**
 *  ���� I/O�� ���õ� ����� �׽�Ʈ
 */
static void kTestFileIO( const char* pcParameterBuffer )
{
    FILE* pstFile;
    BYTE* pbBuffer;
    int i;
    int j;
    DWORD dwRandomOffset;
    DWORD dwByteCount;
    BYTE vbTempBuffer[ 1024 ];
    DWORD dwMaxFileSize;
    
    kPrintf( "================== File I/O Function Test ==================\n" );
    
    // 4Mbyte�� ���� �Ҵ�
    dwMaxFileSize = 4 * 1024 * 1024;
    pbBuffer = kAllocateMemory( dwMaxFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocation Fail\n" );
        return ;
    }
    // �׽�Ʈ�� ������ ����
    remove( "testfileio.bin" );

    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "1. File Open Fail Test..." );
    // r �ɼ��� ������ �������� �����Ƿ�, �׽�Ʈ ������ ���� ��� NULL�� �Ǿ�� ��
    pstFile = fopen( "testfileio.bin", "r" );
    if( pstFile == NULL )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
        fclose( pstFile );
    }
    
    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "2. File Create Test..." );
    // w �ɼ��� ������ �����ϹǷ�, ���������� �ڵ��� ��ȯ�Ǿ����
    pstFile = fopen( "testfileio.bin", "w" );
    if( pstFile != NULL )
    {
        kPrintf( "[Pass]\n" );
        kPrintf( "    File Handle [0x%Q]\n", pstFile );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    //==========================================================================
    // �������� ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "3. Sequential Write Test(Cluster Size)..." );
    // ���� �ڵ��� ������ ���� ����
    for( i = 0 ; i < 100 ; i++ )
    {
        kMemSet( pbBuffer, i, FILESYSTEM_CLUSTERSIZE );
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    %d Cluster Error\n", i );
            break;
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }
    
    //==========================================================================
    // �������� ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "4. Sequential Read And Verify Test(Cluster Size)..." );
    // ������ ó������ �̵�
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_END );
    
    // ���� �ڵ��� ������ �б� ���� ��, ������ ����
    for( i = 0 ; i < 100 ; i++ )
    {
        // ������ ����
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        
        // ������ �˻�
        for( j = 0 ; j < FILESYSTEM_CLUSTERSIZE ; j++ )
        {
            if( pbBuffer[ j ] != ( BYTE ) i )
            {
                kPrintf( "[Fail]\n" );
                kPrintf( "    %d Cluster Error. [%X] != [%X]\n", i, pbBuffer[ j ], 
                         ( BYTE ) i );
                break;
            }
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }

    //==========================================================================
    // ������ ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "5. Random Write Test...\n" );
    
    // ���۸� ��� 0���� ä��
    kMemSet( pbBuffer, 0, dwMaxFileSize );
    // ���� ���⿡ �Űܴٴϸ鼭 �����͸� ���� ����
    // ������ ������ �о ���۷� ����
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_CUR );
    fread( pbBuffer, 1, dwMaxFileSize, pstFile );
    
    // ������ ��ġ�� �ű�鼭 �����͸� ���ϰ� ���ۿ� ���ÿ� ��
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( kRandom() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = kRandom() % ( dwMaxFileSize - dwByteCount );
        
        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset, 
                dwByteCount );

        // ���� �����͸� �̵�
        fseek( pstFile, dwRandomOffset, SEEK_SET );
        kMemSet( vbTempBuffer, i, dwByteCount );
              
        // �����͸� ��
        if( fwrite( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
        
        kMemSet( pbBuffer + dwRandomOffset, i, dwByteCount );
    }
    
    // �� ���������� �̵��Ͽ� 1����Ʈ�� �Ἥ ������ ũ�⸦ 4Mbyte�� ����
    fseek( pstFile, dwMaxFileSize - 1, SEEK_SET );
    fwrite( &i, 1, 1, pstFile );
    pbBuffer[ dwMaxFileSize - 1 ] = ( BYTE ) i;

    //==========================================================================
    // ������ ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "6. Random Read And Verify Test...\n" );
    // ������ ��ġ�� �ű�鼭 ���Ͽ��� �����͸� �о� ������ ����� ��
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( kRandom() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = kRandom() % ( ( dwMaxFileSize ) - dwByteCount );

        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset, 
                dwByteCount );
        
        // ���� �����͸� �̵�
        fseek( pstFile, dwRandomOffset, SEEK_SET );
        
        // ������ ����
        if( fread( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Read Fail\n", dwRandomOffset ); 
            break;
        }
        
        // ���ۿ� ��
        if( kMemCmp( pbBuffer + dwRandomOffset, vbTempBuffer, dwByteCount ) 
                != 0 )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Compare Fail\n", dwRandomOffset ); 
            break;
        }
        
        kPrintf( "[Pass]\n" );
    }
    
    //==========================================================================
    // �ٽ� �������� ���� �б� �׽�Ʈ
    //==========================================================================
    kPrintf( "7. Sequential Write, Read And Verify Test(1024 Byte)...\n" );
    // ������ ó������ �̵�
    fseek( pstFile, -dwMaxFileSize, SEEK_CUR );
    
    // ���� �ڵ��� ������ ���� ����. �պκп��� 2Mbyte�� ��
    for( i = 0 ; i < ( 2 * 1024 * 1024 / 1024 ) ; i++ )
    {
        kPrintf( "    [%d] Offset [%d] Byte [%d] Write...", i, i * 1024, 1024 );

        // 1024 ����Ʈ�� ������ ��
        if( fwrite( pbBuffer + ( i * 1024 ), 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }

    // ������ ó������ �̵�
    fseek( pstFile, -dwMaxFileSize, SEEK_SET );
    
    // ���� �ڵ��� ������ �б� ���� �� ������ ����. Random Write�� �����Ͱ� �߸� 
    // ����� �� �����Ƿ� ������ 4Mbyte ��ü�� ������� ��
    for( i = 0 ; i < ( dwMaxFileSize / 1024 )  ; i++ )
    {
        // ������ �˻�
        kPrintf( "    [%d] Offset [%d] Byte [%d] Read And Verify...", i, 
                i * 1024, 1024 );
        
        // 1024 ����Ʈ�� ������ ����
        if( fread( vbTempBuffer, 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        
        if( kMemCmp( pbBuffer + ( i * 1024 ), vbTempBuffer, 1024 ) != 0 )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }
        
    //==========================================================================
    // ���� ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "8. File Delete Fail Test..." );
    // ������ �����ִ� �����̹Ƿ� ������ ������� �ϸ� �����ؾ� ��
    if( remove( "testfileio.bin" ) != 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    //==========================================================================
    // ���� �ݱ� �׽�Ʈ
    //==========================================================================
    kPrintf( "9. File Close Test..." );
    // ������ ���������� ������ ��
    if( fclose( pstFile ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    //==========================================================================
    // ���� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "10. File Delete Test..." );
    // ������ �������Ƿ� ���������� �������� �� 
    if( remove( "testfileio.bin" ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    // �޸𸮸� ����
    kFreeMemory( pbBuffer );    
}

/**
 *  ������ �а� ���� �ӵ��� ����
 */
static void kTestPerformance( const char* pcParameterBuffer )
{
    FILE* pstFile;
    DWORD dwClusterTestFileSize;
    DWORD dwOneByteTestFileSize;
    QWORD qwLastTickCount;
    DWORD i;
    BYTE* pbBuffer;
    
    // Ŭ�����ʹ� 1Mbyte���� ������ �׽�Ʈ
    dwClusterTestFileSize = 1024 * 1024;
    // 1����Ʈ�� �а� ���� �׽�Ʈ�� �ð��� ���� �ɸ��Ƿ� 16Kbyte�� �׽�Ʈ
    dwOneByteTestFileSize = 16 * 1024;
    
    // �׽�Ʈ�� ���� �޸� �Ҵ�
    pbBuffer = kAllocateMemory( dwClusterTestFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocate Fail\n" );
        return ;
    }
    
    // ���۸� �ʱ�ȭ
    kMemSet( pbBuffer, 0, FILESYSTEM_CLUSTERSIZE );
    
    kPrintf( "================== File I/O Performance Test ==================\n" );

    //==========================================================================
    // Ŭ������ ������ ������ ���������� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "1.Sequential Read/Write Test(Cluster Size)\n" );

    // ������ �׽�Ʈ ������ �����ϰ� ���� ����
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w" );
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }
    
    qwLastTickCount = kGetTickCount();
    // Ŭ������ ������ ���� �׽�Ʈ
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) != 
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Write Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ���
    kPrintf( "   Sequential Write(Cluster Size): %d ms\n", kGetTickCount() - 
             qwLastTickCount );

    //==========================================================================
    // Ŭ������ ������ ������ ���������� �д� �׽�Ʈ
    //==========================================================================
    // ������ ó������ �̵�
    fseek( pstFile, 0, SEEK_SET );
    
    qwLastTickCount = kGetTickCount();
    // Ŭ������ ������ �д� �׽�Ʈ
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) != 
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Read Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ���
    kPrintf( "   Sequential Read(Cluster Size): %d ms\n", kGetTickCount() - 
             qwLastTickCount );
    
    //==========================================================================
    // 1 ����Ʈ ������ ������ ���������� ���� �׽�Ʈ
    //==========================================================================
    kPrintf( "2.Sequential Read/Write Test(1 Byte)\n" );
    
    // ������ �׽�Ʈ ������ �����ϰ� ���� ����
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w" );
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }
    
    qwLastTickCount = kGetTickCount();
    // 1 ����Ʈ ������ ���� �׽�Ʈ
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fwrite( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Write Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ���
    kPrintf( "   Sequential Write(1 Byte): %d ms\n", kGetTickCount() - 
             qwLastTickCount );

    //==========================================================================
    // 1 ����Ʈ ������ ������ ���������� �д� �׽�Ʈ
    //==========================================================================
    // ������ ó������ �̵�
    fseek( pstFile, 0, SEEK_SET );
    
    qwLastTickCount = kGetTickCount();
    // 1 ����Ʈ ������ �д� �׽�Ʈ
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fread( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Read Fail\n" );
            // ������ �ݰ� �޸𸮸� ������
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // �ð� ���
    kPrintf( "   Sequential Read(1 Byte): %d ms\n", kGetTickCount() - 
             qwLastTickCount );
    
    // ������ �ݰ� �޸𸮸� ������
    fclose( pstFile );
    kFreeMemory( pbBuffer );
}

/**
 *  ���� �ý����� ĳ�� ���ۿ� �ִ� �����͸� ��� �ϵ� ��ũ�� �� 
 */
static void kFlushCache( const char* pcParameterBuffer )
{
    QWORD qwTickCount;
    
    qwTickCount = kGetTickCount();
    kPrintf( "Cache Flush... ");
    if( kFlushFileSystemCache() == TRUE )
    {
        kPrintf( "Pass\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
    kPrintf( "Total Time = %d ms\n", kGetTickCount() - qwTickCount );
}


static void kEditor( const char* pcParameterBuffer )
{
    CHARACTER prev[CONSOLE_HEIGHT*CONSOLE_WIDTH];
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int x, y;

    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;

    FILE* fp;
    int iEnterCount;
    BYTE bKey;

    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';

    kGetCursor(&x, &y);

    for(int i=0; i<y*CONSOLE_WIDTH+x; i++)
    {
        prev[i].bCharactor = pstScreen[i].bCharactor;
        prev[i].bAttribute = pstScreen[i].bAttribute;
    }
    // kViEditor(x, y);
    kTest(x,y,vcFileName);

    for(int i=0; i<y*CONSOLE_WIDTH+x; i++)
        pstScreen[i].bCharactor = prev[i].bCharactor;
	for(int i=y*CONSOLE_WIDTH+x; i<CONSOLE_WIDTH*CONSOLE_HEIGHT; i++)
		pstScreen[i].bCharactor = ' ';

}

/**
 *  ���丮 ����
 */
static void kMakeDirectory( const char* pcParamegerBuffer ){
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParamegerBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    pstFile = opendir( vcFileName);

    if( pstFile == NULL )
    {
        kPrintf( "Directory Create Fail\n" );
        return;
    }
    fclose( pstFile );
    kPrintf( "Directory Create Success\n" );
}


static void kShowDirectory( const char* pcParameterBuffer )
{
    DIR* pstDirectory;
    int i, iCount, iTotalCount;
    struct dirent* pstEntry;
    char vcBuffer[ 400 ];
    char vcTempValue[ 500 ];
    DWORD dwTotalByte;
    DWORD dwUsedClusterCount;
    FILESYSTEMMANAGER stManager;
    DIRECTORYENTRY* directoryInfo;
    
    // ���� �ý��� ������ ����
    kGetFileSystemInformation( &stManager );
     
    iCount = 0;

    directoryInfo = kFindDirectory(currentDirectoryClusterIndex);
    
    for( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if( directoryInfo[ i ].dwStartClusterIndex != 0 )   //directory 
        {
            
        pstEntry = &directoryInfo[i];
        // ���� �������� �ʱ�ȭ �� �� �� ��ġ�� ���� ����
        kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
        vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';
       
        if(pstEntry->flag == 1){	//directory
            // ���� �̸� ����
            kMemCpy( vcBuffer, pstEntry->d_name, kStrLen( pstEntry->d_name ) );
	
            // ���� ���� ����
            kSPrintf( vcTempValue, "Directory", 10 );
            kMemCpy( vcBuffer + 10, vcTempValue, kStrLen( vcTempValue ) +1);
    

            kMemCpy(vcBuffer+19," ", 1);

            kIToA(pstEntry->wYear, vcTempValue, 10);
            kMemCpy(vcBuffer+21,vcTempValue, kStrLen(vcTempValue));

            kMemCpy(vcBuffer+25,"/", 1);

            kIToA(pstEntry->bMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+26,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+28,"/", 1);

            kIToA(pstEntry->bDayOfMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+29,vcTempValue, kStrLen(vcTempValue)+1);


            kMemCpy(vcBuffer+30,"  ", 1);

            kIToA(pstEntry->bHour, vcTempValue, 10);
            kMemCpy(vcBuffer+32,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+34,":", 1);


            kIToA(pstEntry->bMinute, vcTempValue, 10);
            kMemCpy(vcBuffer+35,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+37,":", 1);

            kIToA(pstEntry->bSecond, vcTempValue, 10);
            kMemCpy(vcBuffer+38,vcTempValue, kStrLen(vcTempValue)+1);
            kPrintf( " %s \n", vcBuffer );     
        
        }
        else if(pstEntry->flag == 0){		//file
        // ���� �̸� ����
        kMemCpy( vcBuffer, pstEntry->d_name, 
                 kStrLen( pstEntry->d_name ) );

        // ���� ���� ����
        kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
        kMemCpy( vcBuffer + 10, vcTempValue, kStrLen( vcTempValue )  );

        // ������ ���� Ŭ������ ����
        kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
        kMemCpy( vcBuffer + 20, vcTempValue, kStrLen( vcTempValue ) + 1 );
             kMemCpy(vcBuffer+31,"  ", 1);

            kIToA(pstEntry->wYear, vcTempValue, 10);
            kMemCpy(vcBuffer+33,vcTempValue, kStrLen(vcTempValue));

            kMemCpy(vcBuffer+37,"/", 1);

            kIToA(pstEntry->bMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+38,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+ 40,"/", 1);

            kIToA(pstEntry->bDayOfMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+41,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+42,"  ", 1);

            kIToA(pstEntry->bHour, vcTempValue, 10);
            kMemCpy(vcBuffer+44,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+46,":", 1);

            kIToA(pstEntry->bMinute, vcTempValue, 10);
            kMemCpy(vcBuffer+47,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+49,":", 1);

            kIToA(pstEntry->bSecond, vcTempValue, 10);
            kMemCpy(vcBuffer+50,vcTempValue, kStrLen(vcTempValue)+1);
   
        kPrintf( " %s\n", vcBuffer );
        }
        }
    }
}


static void kShowAllFile(void){
DIR* pstDirectory;
    int i, iCount, iTotalCount;
    struct dirent* pstEntry;
    char vcBuffer[ 400 ];
    char vcTempValue[ 500 ];
    DWORD dwTotalByte;
    DWORD dwUsedClusterCount;
    FILESYSTEMMANAGER stManager;
    DIRECTORYENTRY* directoryInfo;

    
    // ���� �ý��� ������ ����
    kGetFileSystemInformation( &stManager );
     
    iCount = 0;

    directoryInfo = kFindDirectory(currentDirectoryClusterIndex);
    
    for( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if( directoryInfo[ i ].dwStartClusterIndex != 0 )   //directory 
        {
            
        pstEntry = &directoryInfo[i];
        // ���� �������� �ʱ�ȭ �� �� �� ��ġ�� ���� ����
        kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
        vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';
       
        if(pstEntry->flag == 1){	//directory
            // ���� �̸� ����
            kSPrintf(vcTempValue,"d",1);
            kMemCpy( vcBuffer + 1, vcTempValue, kStrLen(vcTempValue)); 

            switch(pstEntry->user){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }
            switch(pstEntry->group){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer +5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }

            switch(pstEntry->other){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer +8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }
             kSPrintf(vcTempValue," ",1);
            kMemCpy( vcBuffer + 11, vcTempValue, kStrLen(vcTempValue)); 

            kMemCpy( vcBuffer+12, pstEntry->d_name, kStrLen( pstEntry->d_name ) );
	
            // ���� ���� ����
            kSPrintf( vcTempValue, "Directory", 10 );
            kMemCpy( vcBuffer + 22, vcTempValue, kStrLen( vcTempValue ) +1);

            kMemCpy(vcBuffer+31," ", 1);

            kIToA(pstEntry->wYear, vcTempValue, 10);
            kMemCpy(vcBuffer+33,vcTempValue, kStrLen(vcTempValue));

            kMemCpy(vcBuffer+37,"/", 1);

            kIToA(pstEntry->bMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+38,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+40,"/", 1);

            kIToA(pstEntry->bDayOfMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+41,vcTempValue, kStrLen(vcTempValue)+1);


            kMemCpy(vcBuffer+42,"  ", 1);

            kIToA(pstEntry->bHour, vcTempValue, 10);
            kMemCpy(vcBuffer+44,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+46,":", 1);


            kIToA(pstEntry->bMinute, vcTempValue, 10);
            kMemCpy(vcBuffer+48,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+50,":", 1);

            kIToA(pstEntry->bSecond, vcTempValue, 10);
            kMemCpy(vcBuffer+51,vcTempValue, kStrLen(vcTempValue)+1);
            kPrintf( " %s \n", vcBuffer );     
        
        }
        else if(pstEntry->flag == 0){		//file
        // ���� �̸� ����

        kSPrintf(vcTempValue,"-",1);    //print '-' because it is file
            kMemCpy( vcBuffer + 1, vcTempValue, kStrLen(vcTempValue)); 

            switch(pstEntry->user){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 2, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }
            switch(pstEntry->group){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer +5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 5, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }

            switch(pstEntry->other){
                case 1:
                    kSPrintf( vcTempValue, "--x", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 2:
                    kSPrintf( vcTempValue, "-w-", 3 );
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 3:
                    kSPrintf( vcTempValue, "-wx", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 4:
                    kSPrintf( vcTempValue, "r--", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 5:
                    kSPrintf( vcTempValue, "r-x",3 );
                    kMemCpy( vcBuffer +8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 6:
                    kSPrintf( vcTempValue, "rw-", 3);
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;
                case 7:
                    kSPrintf( vcTempValue, "rwx", 3 );
                    kMemCpy( vcBuffer + 8, vcTempValue, kStrLen( vcTempValue ));
                    break;

            }



        kSPrintf(vcTempValue," ",1);
        kMemCpy( vcBuffer + 11, vcTempValue, kStrLen(vcTempValue)); 

        kMemCpy( vcBuffer+12, pstEntry->d_name, kStrLen( pstEntry->d_name ) );


        // ���� ���� ����
        kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
        kMemCpy( vcBuffer + 20, vcTempValue, kStrLen( vcTempValue )  );

        // ������ ���� Ŭ������ ����
        kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
        kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue ) + 1 );

             kMemCpy(vcBuffer+41,"  ", 1);

            kIToA(pstEntry->wYear, vcTempValue, 10);
            kMemCpy(vcBuffer+43,vcTempValue, kStrLen(vcTempValue));

            kMemCpy(vcBuffer+47,"/", 1);

            kIToA(pstEntry->bMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+48,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+ 50,"/", 1);

            kIToA(pstEntry->bDayOfMonth, vcTempValue, 10);
            kMemCpy(vcBuffer+51,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+52,"  ", 1);

            kIToA(pstEntry->bHour, vcTempValue, 10);
            kMemCpy(vcBuffer+54,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+56,":", 1);

            kIToA(pstEntry->bMinute, vcTempValue, 10);
            kMemCpy(vcBuffer+57,vcTempValue, kStrLen(vcTempValue)+1);

            kMemCpy(vcBuffer+59,":", 1);

            kIToA(pstEntry->bSecond, vcTempValue, 10);
            kMemCpy(vcBuffer+60,vcTempValue, kStrLen(vcTempValue)+1);
   
        kPrintf( " %s\n", vcBuffer );

        }

        }
        
    }
   



    
}

/**
 * ���丮 �̵�
 */
static void kMoveDirectory( const char* pcParamegerBuffer){
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParamegerBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    DIR* pstDirectory;
    
    FILESYSTEMMANAGER stManager;
    DIRECTORYENTRY* directoryInfo;
    char temp_path[100] = "\0";
    DWORD temp_index = 0;
    
    // ���� �ý��� ������ ����
    kGetFileSystemInformation( &stManager );
   
   
    directoryInfo = kFindDirectory(currentDirectoryClusterIndex);

    if(kMemCmp(vcFileName,".",2)==0){
        currentDirectoryClusterIndex = directoryInfo[0].ParentDirectoryCluserIndex;
        kPrintf("%d", currentDirectoryClusterIndex);
        kSetClusterIndex(currentDirectoryClusterIndex);
        kMemCpy(path,directoryInfo[0].ParentDirectoryPath,kStrLen(directoryInfo[0].ParentDirectoryPath)+1);
     }
    else if(kMemCmp(vcFileName,"..",3)==0){
       
        currentDirectoryClusterIndex = directoryInfo[1].ParentDirectoryCluserIndex;
        kSetClusterIndex(currentDirectoryClusterIndex);
        
        kMemCpy(path,directoryInfo[1].ParentDirectoryPath,kStrLen(directoryInfo[1].ParentDirectoryPath)+1);
     }
     
    else{
        for( int j = 2 ; j < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; j++ )
        {
            if( directoryInfo[ j ].dwStartClusterIndex != 0 && kStrLen(directoryInfo[j].vcFileName) == kStrLen(vcFileName)
         && kMemCmp(directoryInfo[ j ].vcFileName,vcFileName,kStrLen(vcFileName)+1)==0 && directoryInfo[j].flag ==1)
            {//kch path sujeong
                kMemCpy(temp_path,path,kStrLen(path)+1);   
                temp_index = currentDirectoryClusterIndex;
                
                if(kMemCmp(path,"/",2)==0)
                    kMemCpy(path + kStrLen(path),vcFileName,kStrLen(vcFileName)+1);
                else{
                    kMemCpy(path + kStrLen(path),"/",1);
                    kMemCpy(path + kStrLen(path),vcFileName,kStrLen(vcFileName)+1);
                }

                currentDirectoryClusterIndex = directoryInfo[ j ].dwStartClusterIndex;
                kSetClusterIndex(currentDirectoryClusterIndex);
                directoryInfo = NULL;
                directoryInfo = kFindDirectory(currentDirectoryClusterIndex);

            
                break;
            
            }
            
        }
    }

    
}
/**
 *  ���͸��� ����
 */
static void kRemoveDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    if( remove( vcFileName ) != 0 )
    {
        kPrintf( "File Not Found or File Opened\n" );
        return ;
    }
    
    kPrintf( "File Delete Success\n" );
}

static void kReservejob( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcHour );
    kGetNextParameter( &stList, vcMinute );
    kGetNextParameter( &stList, vcSecond );

 
    kCreateTask( 3 | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kRunreservedtask );
}

void kRunreservedtask(void){
    BYTE bSecond, bMinute, bHour;
    while(1){
        kReadRTCTime( &bHour, &bMinute, &bSecond );
        if(kAToI(vcHour, 10) == bHour && kAToI(vcMinute, 10) == bMinute && kAToI(vcSecond, 10) == bSecond){
            for(int i = 0; i < 10; i++){
                kCreateTask( 3 | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 );
            }
            break;
        }
    }
}

static void kChFileMod(const char* pcParameterBuffer){

	DIRECTORYENTRY* directoryInfo;
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    char mode;
    int i_mode, u_mod, g_mod, o_mod, iLength, iCount, iTotalCount;
    
	kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, mode );
    iLength = kGetNextParameter(&stList, vcFileName);
	vcFileName[ iLength ] = '\0';
	
	i_mode= kAToI(mode,10);
    u_mod = (i_mode/100);
    g_mod = (i_mode - (100*u_mod))/10;
    o_mod = (i_mode-((100*u_mod)+(10*g_mod)))/1;
	
    directoryInfo = kFindDirectory(0);

	for(int i = 0 ; i<FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
		if (kMemCmp(directoryInfo[ i ].vcFileName, vcFileName, kStrLen(vcFileName)) == 0 )
		{
			kUpdateFileMod(directoryInfo[i], u_mod, g_mod, o_mod, i);
            kPrintf("Change File Mode complete. \n");
            return ;
        }
    }
	kPrintf("There is no such file!\n");
	return;
}
void kScreenSaver(void)
{
    kCreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, 
                         ( QWORD ) kDropScreensSaver );

}
static void kDropScreensSaver( void )
{
    int iX, iY;
    int i;
  
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int x, y;
    int temp = 0;
    BYTE bData = 0;
    int iMargin = 0;
    
    while( 1 )
    {
        if(screensaver.run == 1){
            if(temp == 0){
                kGetCursor(&x, &y);
                for(int i=0; i<CONSOLE_WIDTH*CONSOLE_HEIGHT; i++)
                {
                    screensaver.prev[i] = pstScreen[i];
                    screensaver.prev[i].bAttribute = pstScreen[i].bAttribute;
                }
                screensaver.x = x;
                screensaver.y = y;
                temp = 1;
                kCls(NULL);
            }
            bData = kRandom();
            pstScreen[iMargin].bCharactor = bData;
            pstScreen[iMargin].bAttribute = bData & 0x0F;
            iMargin = (iMargin + 1) % 20000;
            // kSleep(1);
        }
        else
        {
            if(temp == 1){
                for(int i=0; i<CONSOLE_WIDTH*CONSOLE_HEIGHT; i++)
                {
                    pstScreen[i].bCharactor = screensaver.prev[i].bCharactor;
                    pstScreen[i].bAttribute = screensaver.prev[i].bAttribute;

                }
                kSetCursor(screensaver.x, screensaver.y); 
                temp = 0;
            }
        }
        
    }
}



void kSetScreenSaver(void){
    int temp = g_qwTickCount / 1000;
    screensaver.run = 0;
    screensaver.time = temp;
}