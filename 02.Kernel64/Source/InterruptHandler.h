/**
 *  file    InterruptHandler.h
 *  date    2009/01/24
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� ��� ����
 */

#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "Types.h"
#include "Page.h"
////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode );
void kCommonInterruptHandler( int iVectorNumber );
void kKeyboardHandler( int iVectorNumber );

void kPagingInterruptHandler(int faultAdress, QWORD qwErrorCode);
void kPageFaultExceptionHandler( int faultAdress);
void kProtectionFaultExceptionHandler( int faultAdress);
void kRemainingInterruptHandler( int faultAdress);
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags );
void kTimerHandler( int iVectorNumber );
void kHDDHandler( int iVectorNumber );
static inline void invlpg(void* m);
#endif /*__INTERRUPTHANDLER_H__*/
