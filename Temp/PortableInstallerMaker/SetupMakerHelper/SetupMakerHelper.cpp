// SetupMakerHelper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SetupMakerHelper.h"
#include <atlbase.h>
#include "ftlbase.h"

#define USE_INLINE_HOOK

#ifdef USE_INLINE_HOOK
#  include "InlineHook.h"
#else
#  include "detours.h"
#  pragma comment(lib, "detours.lib")
#endif 

#include "ProtectWndHookAPI.h"

HMODULE g_hModule = NULL;
BOOL    g_bNeedHook = FALSE;

#pragma data_seg("SHAREDMEM")
HHOOK g_hHookCallWndProc = NULL;
HHOOK g_hHookKeyboard = NULL;
HWND  g_hWndGetSetupResult = NULL;
DWORD g_dwSetupPID = 0;
#pragma data_seg()
#pragma comment(linker, "/Section:SHAREDMEM,rws")

volatile BOOL  g_bHooked = FALSE;

CProtectWndHookAPI g_ProtectWndHookApi;

LRESULT CALLBACK My_CallWndProc(
                                _In_  int nCode,
                                _In_  WPARAM wParam,
                                _In_  LPARAM lParam
                                )
{
    CWPSTRUCT * pWPStruct = (CWPSTRUCT*)lParam;
    if (g_bNeedHook && !g_bHooked && /*!g_bIsSelfProcess &&*/ pWPStruct)
    {
		g_bHooked = TRUE;
        //g_bIsSelfProcess = (g_ProtectWndInfo.curProcessId == GetCurrentProcessId());

#ifdef _DEBUG
        TCHAR szModuleName[MAX_PATH] = {0};
        GetModuleFileName(NULL, szModuleName, _countof(szModuleName));
        FTLTRACE(TEXT("Will Hook API in PID=%d(%s),TID=%d\n"), 
            GetCurrentProcessId(), PathFindFileName(szModuleName), GetCurrentThreadId());
#endif 
        BOOL bRet = FALSE;
        API_VERIFY(HookApi());
    }
    return CallNextHookEx(g_hHookCallWndProc, nCode, wParam, lParam);
}

LRESULT CALLBACK My_LowLevelKeyboardProc(int nCode,
                                         WPARAM wParam,
                                         LPARAM lParam
                                         )
{
    LRESULT nResult = 1;
    KBDLLHOOKSTRUCT* pKbdLlHookStruct = (KBDLLHOOKSTRUCT*)lParam;
    //FTLTRACE(TEXT("LowLevelKeyboardProc, vkCode=0x%x, ScanCode=0x%x, flags=0x%x, dwExtraInfo=0x%p\n"), 
    //    pKbdLlHookStruct->vkCode, pKbdLlHookStruct->scanCode, pKbdLlHookStruct->flags, pKbdLlHookStruct->dwExtraInfo);
    if (0x37 == pKbdLlHookStruct->scanCode     //PrtSc(55)
        || 0x54 == pKbdLlHookStruct->scanCode) //Alt+PrtSc(84)
    {
        //Skip Call Next Hook

        TCHAR szModuleName[MAX_PATH] = {0};
        GetModuleFileName(NULL, szModuleName, _countof(szModuleName));
        FTLTRACE(TEXT("LowLevelKeyboardProc In PID=%d(%s),TID=%d\n"), 
            GetCurrentProcessId(), PathFindFileName(szModuleName), 
            GetCurrentThreadId());
    }
    else
    {
        nResult = CallNextHookEx(g_hHookKeyboard,nCode,wParam,lParam);
    }
    return nResult;
}

SETUPMAKERHELPER_API BOOL EnableSetupMonitor(DWORD curProcessId, DWORD dwSetupPID, HWND hWndGetSetupResult)
{
    FTLTRACE(TEXT("Enter EnableSetupMonitor for dwSetupPID=0x%x(%d), hWndGetSetupResult=0x%x\n"), 
        dwSetupPID, dwSetupPID, hWndGetSetupResult);
    BOOL bRet = TRUE;
    if (NULL == g_hHookCallWndProc)
    {
        g_dwSetupPID = dwSetupPID;
        g_hWndGetSetupResult = hWndGetSetupResult;
        API_VERIFY((g_hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, My_CallWndProc, g_hModule, 0)) != NULL);
        //API_VERIFY((g_hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, My_LowLevelKeyboardProc, g_hModule, 0)) != NULL);

        //FTLTRACE(TEXT("Leave EnableWindowProtected g_hHook=0x%x, g_hHookKeyboard=0x%x, bRet=%d\n"), g_hHookCallWndProc, g_hHookKeyboard, bRet);
    }
    return bRet;
}

SETUPMAKERHELPER_API BOOL DisableSetupMonitor(DWORD dwSetupPID)
{
    BOOL bRet = TRUE;
    FTLTRACE(TEXT("DisableSetupMonitor, dwSetupPID=%d, g_hHookCallWndProc=0x%x\n"),
        dwSetupPID, g_hHookCallWndProc);
    //FUNCTION_BLOCK_MODULE_NAME_TRACE(TEXT("DisableWindowProtected"), 100);
    API_VERIFY(UnHookApi());
    if (g_hHookCallWndProc)
    {
        API_VERIFY(UnhookWindowsHookEx(g_hHookCallWndProc));
        g_hHookCallWndProc = NULL;
    }
    if (g_hHookKeyboard)
    {
        API_VERIFY(UnhookWindowsHookEx(g_hHookKeyboard));
        g_hHookKeyboard = NULL;
    }
    return bRet;
}

unsigned int __stdcall _AsyncHookControl(void * param)
{
	BOOL bRet = FALSE;
	BOOL bStart = (BOOL)param;
	if(bStart)
	{
		API_VERIFY(g_ProtectWndHookApi.StartHook());
	}
	else
	{
		API_VERIFY(g_ProtectWndHookApi.StopHook());
	}
	return 0;
}
SETUPMAKERHELPER_API BOOL HookApi()
{
	unsigned int nThreadId = 0;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _AsyncHookControl, (void*)TRUE, 0, &nThreadId);
	CloseHandle(hThread);

    return TRUE;
}

SETUPMAKERHELPER_API BOOL UnHookApi()
{
    FTLTRACE(TEXT("Enter UnHookAPi\n"));
    FUNCTION_BLOCK_NAME_TRACE_EX(TEXT("UnHookApi"), FTL::TraceDetailExeName, 100);
    //notify all the toplevel progress
    BroadcastSystemMessage(BSF_FORCEIFHUNG |BSF_POSTMESSAGE, NULL, WM_NULL, 0, 0);
    //Sync UnHook API
	BOOL bRet = FALSE;
    API_VERIFY(g_ProtectWndHookApi.StopHook());
    g_bHooked = FALSE;
    FTLTRACE(TEXT("Leave UnHookAPi\n"));
    return bRet;
}

