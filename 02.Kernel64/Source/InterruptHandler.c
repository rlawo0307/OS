/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� �ҽ� ����
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "Page.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"
#include "HardDisk.h"
#include "ConsoleShell.h"

/**
 *  �������� ����ϴ� ���� �ڵ鷯
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    int iCursorX, iCursorY;
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
    
    // kPrintStringXY( 0, 0, "====================================================" );
    // kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    // kPrintStringXY( 0, 2, "                    Vector:                         " );
    // kPrintStringXY( 27, 2, vcBuffer );
    // kPrintStringXY( 0, 3, "====================================================" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("====================================================\n" );
    kPrintf("                 Exception Occur~!!!!               \n" );
    kPrintf("                    Vector: %c%c                    \n", vcBuffer[0], vcBuffer[1] );
    kPrintf("====================================================" );
    while( 1 ) ;
}
/**
 *  for paging exception ���ͷ�Ʈ �ڵ鷯
 */

void kPagingInterruptHandler(int faultAdress, QWORD qwErrorCode)
{
	int page_mask = 0x00000001;
	int protection_mask = 0x00000002;
	if((qwErrorCode & page_mask) == 0 )
	{
		kPageFaultExceptionHandler(faultAdress);
	}
	else if((qwErrorCode & protection_mask) == 2)
	{
		kProtectionFaultExceptionHandler(faultAdress);
	}
    else
    {
        kRemainingInterruptHandler(faultAdress);
    }
}
void kPageFaultExceptionHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    kPrintf("====================================================\n" );
    kPrintf("                 Page Fault Occur~!!!!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

	pstPTEntry = (PTENTRY*)0x142000;
	kSetPageEntryData(&(pstPTEntry[511]),0,0x1FF000,0x00000003,0);
	invlpg(&faultAdress);
    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kProtectionFaultExceptionHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    
    kPrintf("====================================================\n" );
    kPrintf("                 Protection Fault Occur~!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

	pstPTEntry = (PTENTRY*)0x142000;
	kSetPageEntryData(&(pstPTEntry[511]),0,0x1FF000,0x00000003,0);
	invlpg(&faultAdress);
    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kRemainingInterruptHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    kPrintf("====================================================\n" );
    kPrintf("                 Fault Occur~!!!!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress, 
        DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
    pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) |
        dwUpperFlags;
}
/**
 *  �������� ����ϴ� ���ͷ�Ʈ �ڵ鷯
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  Ű���� ���ͷ�Ʈ�� �ڵ鷯
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ���� ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // Ű���� ��Ʈ�ѷ����� �����͸� �о ASCII�� ��ȯ�Ͽ� ť�� ����
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
    screensaver.run = 0;
    screensaver.time = g_qwTickCount / 1000;
}

/**
 *  Ÿ�̸� ���ͷ�Ʈ�� �ڵ鷯
 */
void kTimerHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iTimerInterruptCount = 0;
    int temp;
    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iTimerInterruptCount;
    g_iTimerInterruptCount = ( g_iTimerInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );

    // Ÿ�̸� �߻� Ƚ���� ����
    g_qwTickCount++;

    // �½�ũ�� ����� ���μ����� �ð��� ����
    kDecreaseProcessorTime();
    // ���μ����� ����� �� �ִ� �ð��� �� ��ٸ� �½�ũ ��ȯ ����
    if( kIsProcessorTimeExpired() == TRUE )
    {
        kScheduleInInterrupt();
    }
    temp = g_qwTickCount / 1000;
    if((temp - screensaver.time) >= 15){
        screensaver.run = 1;
    }
}
/**
 *  �ϵ� ��ũ���� �߻��ϴ� ���ͷ�Ʈ�� �ڵ鷯
 */
void kHDDHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iHDDInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ���� ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iHDDInterruptCount;
    g_iHDDInterruptCount = ( g_iHDDInterruptCount + 1 ) % 10;
    // ���� ���� �ִ� �޽����� ��ġ�� �ʵ��� (10, 0)�� ���
    kPrintStringXY( 10, 0, vcBuffer );
    //=========================================================================

    // ù ��° PATA ��Ʈ�� ���ͷ�Ʈ ����(IRQ 14) ó��
    if( iVectorNumber - PIC_IRQSTARTVECTOR == 14 )
    {
        // ù ��° PATA ��Ʈ�� ���ͷ�Ʈ �߻� ���θ� TRUE�� ����
        kSetHDDInterruptFlag( TRUE, TRUE );
    }
    // �� ��° PATA ��Ʈ�� ���ͷ�Ʈ ����(IRQ 15) ó��
    else
    {
        // �� ��° PATA ��Ʈ�� ���ͷ�Ʈ �߻� ���θ� TRUE�� ����
        kSetHDDInterruptFlag( FALSE, TRUE );
    }
    
    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}


static inline void invlpg(void *m)
{   
	asm volatile("invlpg (%0)" :  : "b"(m) : "memory");
}
