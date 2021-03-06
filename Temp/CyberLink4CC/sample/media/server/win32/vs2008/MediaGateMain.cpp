/******************************************************************
*
*	CyberMediaGate for C++
*
*	Copyright (C) Satoshi Konno 2003-2004
*
*	File: MediaGateMain.cpp
*
******************************************************************/
#include <winsock2.h>   
#include <windowsx.h>   
#include <commdlg.h>   
#include <commctrl.h>
#include <shlobj.h>
#include <uhttp/util/Logger.h>
#include <uhttp/util/LoggerOutputDebugString.h>
#include <uhttp/util/Debug.h>

#include "resource.h"
#include "MediaGate.h"

#include <atlstr.h>

#include <ftlBase.h>
#include <ftlConversion.h>

using namespace std;
using namespace FTL;

const TCHAR WINDOW_TITLE[] = TEXT("Cyber Media Gate");
const TCHAR WINDOW_CLASS[] = TEXT("CyberMediaGate");

const int SPLIT_BORDER_WIDTH = 2;
const int DEFAULT_BUTTON_WIDTH = 50;
const int BUTTON_HEIGHT = 20;
const int DEFAULT_DIRECTORY_WINDOW_SIZE = (DEFAULT_BUTTON_WIDTH*2);
const int DEFAULT_WINDOW_SIZE = 400;
const int DEFAULT_WINDOW_HEIGHT = 300;

HINSTANCE ghInstance;

HWND ghMainWindow;
HWND ghDirectoryListWindow;
HWND ghItemListWindow;
HWND ghAddWindow;
HWND ghDelWindow;

int gDirectoryWindowWidth;
int gMouseOffset;

MediaGate *gMediaGate;
string gPreferenceFileName;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resize Window
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResetWindowSize()
{
	RECT mainWinRect;
	GetClientRect(ghMainWindow,&mainWinRect);

	int buttonWidth = gDirectoryWindowWidth / 2;
	MoveWindow(
		ghAddWindow,
		0,mainWinRect.bottom - BUTTON_HEIGHT,
		buttonWidth, BUTTON_HEIGHT,
		TRUE);

	MoveWindow(
		ghDelWindow,
		buttonWidth,mainWinRect.bottom - BUTTON_HEIGHT,
		buttonWidth, BUTTON_HEIGHT,
		TRUE);

	MoveWindow(
		ghDirectoryListWindow,
		0,0,
		gDirectoryWindowWidth, mainWinRect.bottom - BUTTON_HEIGHT,
		TRUE);

	int itemWindowWidth = mainWinRect.right - gDirectoryWindowWidth - SPLIT_BORDER_WIDTH;
	MoveWindow(
		ghItemListWindow,
		gDirectoryWindowWidth + SPLIT_BORDER_WIDTH, 0,
		itemWindowWidth, mainWinRect.bottom,
		TRUE);
}

void SetDirectoryWindowWidth(int width)
{
	RECT mainWinRect;
	GetClientRect(ghMainWindow,&mainWinRect);
	if(width<1)
		width = 1;
	if(mainWinRect.right < (width + SPLIT_BORDER_WIDTH + 1))
		width = mainWinRect.right - SPLIT_BORDER_WIDTH - 1;
	gDirectoryWindowWidth = width;
	ResetWindowSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Item Node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HFONT CreateWindowFont()
{
	return CreateFont(
			16, 0, 
			0, 0, FW_NORMAL,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, 
			VARIABLE_PITCH | FF_DONTCARE,
			TEXT("Lucida Sans Unicode")
		);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Item Node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND CreateItemListWindow(HWND mainWnd, HINSTANCE mainInstance)
{
	HWND listWnd =CreateWindowEx(
		0,
		WC_LISTVIEW,
		0,
		WS_CHILD | LVS_REPORT | WS_VISIBLE,
		0,0,0,0,
		ghMainWindow,
		(HMENU)1,
		mainInstance,
		NULL);

	TCHAR *columTitle[] = {TEXT("title"), TEXT("creator"), TEXT("date"), TEXT("size")};

	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = ((DEFAULT_WINDOW_SIZE - DEFAULT_DIRECTORY_WINDOW_SIZE - SPLIT_BORDER_WIDTH)/4)-2;
	for (int n=0; n<4; n++) {
		col.pszText = columTitle[n];
		ListView_InsertColumn(listWnd , n , &col);
	}

	return listWnd;
}

void UpdateItemList(int dirNum)
{
	ListView_DeleteAllItems(ghItemListWindow);

	MediaServer *mserver = gMediaGate->getMediaServer();
	Directory *dir = mserver->getContentDirectory(dirNum);
	if (dir == NULL)
		return;

	LVITEM item = {0};
	item.mask = 0;
	item.mask = LVIF_TEXT;
    TCHAR buf[512] = { 0 } ;
	int itemCnt = dir->getNContentNodes();
	for (int n=0 ; n<itemCnt ; n++) {
		ContentNode *conNode = dir->getContentNode(n);
		if (conNode->isItemNode() == false)
			continue;
		
		ItemNode *itemNode = (ItemNode *)conNode;

        FTL::CFConversion conv;
		item.pszText = (LPTSTR)conv.MBCS_TO_TCHAR(itemNode->getTitle());
		item.iItem = n;
		item.iSubItem = 0;
		ListView_InsertItem(ghItemListWindow , &item);

		item.pszText = (LPTSTR)conv.MBCS_TO_TCHAR(itemNode->getCreator());
		item.iSubItem = 1;
		ListView_SetItem(ghItemListWindow , &item);

		item.pszText = (LPTSTR)conv.MBCS_TO_TCHAR(itemNode->getDate());
		item.iSubItem = 2;
		ListView_SetItem(ghItemListWindow , &item);

		_stprintf(buf, TEXT("%ld"), itemNode->getStorageUsed());
		item.pszText = buf;
		item.iSubItem = 3;
		ListView_SetItem(ghItemListWindow , &item);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Content Directory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND CreateDirectoryListWindow(HWND mainWnd, HINSTANCE mainInstance)
{
	HWND listWnd = CreateWindow(
		TEXT("LISTBOX"),
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
		0,0,0,0,
		ghMainWindow,
		(HMENU)1,
		mainInstance,
		NULL);

	return listWnd;
}

void UpdateDirectoryList()
{
	SendMessage(ghDirectoryListWindow, LB_RESETCONTENT, 0, 0);

	MediaServer *mserver = gMediaGate->getMediaServer();
	int dirCnt = mserver->getNContentDirectories();
	for (int n=0; n<dirCnt; n++) {
		Directory *dir = mserver->getContentDirectory(n);
		const char *name = dir->getFriendlyName();
        FTL::CFConversion conv;
		SendMessage(ghDirectoryListWindow, LB_ADDSTRING, 0, (LPARAM)conv.MBCS_TO_TCHAR(name));
	}
}

void DeleteDirectoryList(int selID)
{
	MediaServer *mserver = gMediaGate->getMediaServer();
	Directory *dir = mserver->getContentDirectory(selID);
	if (dir == NULL)
		return;
    
	
    if (FTL::FormatMessageBox(ghMainWindow, WINDOW_TITLE, MB_OKCANCEL, 
        TEXT("Are you delete the '%s' directory ?"),  dir->getFriendlyName()) != IDOK)
		return;
	mserver->removeContentDirectory(selID);
	UpdateDirectoryList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Content Directory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND CreateButton(HWND mainWnd, HINSTANCE mainInstance, const TCHAR *name)
{
	HWND buttonWnd =CreateWindow(
		TEXT("BUTTON"),
		name,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0,0,DEFAULT_BUTTON_WIDTH,BUTTON_HEIGHT,
		ghMainWindow,
		NULL,
		mainInstance,
		NULL);

	return buttonWnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Add Dialog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TCHAR gDirName[MAX_PATH+1];
TCHAR gDirPath[MAX_PATH+1];

void InitBrowseInfo(HWND hWnd, BROWSEINFO &bInfo)
{
	bInfo.hwndOwner = hWnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = gDirPath;
	bInfo.lpszTitle = _T("");
	bInfo.ulFlags = 0;
	bInfo.lpfn = NULL;
	bInfo.lParam = 0;
	bInfo.iImage = 0;
}

BOOL CALLBACK AddDialogProc(HWND hWnd, UINT msg , WPARAM wParam , LPARAM lParam)
{
	switch (msg) {
	case WM_COMMAND:
		{
			int comID = (int)LOWORD(wParam);
			if (comID == IDC_OPEN_BUTTON) {
				BROWSEINFO bInfo;
				InitBrowseInfo(hWnd, bInfo);
				LPITEMIDLIST itemList = SHBrowseForFolder(&bInfo);
				if (itemList != NULL) {
					SHGetPathFromIDList(itemList, gDirPath);
					SetWindowText(GetDlgItem(hWnd, IDC_DIR_PATH), gDirPath);
				}
			}
			else if (comID == IDOK) {
				GetWindowText(GetDlgItem(hWnd, IDC_DIR_NAME), gDirName, MAX_PATH);
				GetWindowText(GetDlgItem(hWnd, IDC_DIR_PATH), gDirPath, MAX_PATH);
				if (_tcslen(gDirName) <= 0 || _tcslen(gDirPath) <= 0)
					return FALSE;
				EndDialog(hWnd , IDOK);
			}
			else if (comID == IDCANCEL) {
				EndDialog(hWnd , IDCANCEL);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void OpenAddDialog(HWND hWnd)
{
	gDirName[0] = '\0';
	gDirPath[0] = '\0';
	int ret = DialogBox(
		(HINSTANCE)GetWindowLong(hWnd , GWL_HINSTANCE),
		MAKEINTRESOURCE(IDD_ADD_DIALOG),
		hWnd ,
		AddDialogProc);
	if (ret != IDOK)
		return;
    std::string strUtf8Path;
    FTL::CFConversion convName;
    FTL::CFConversion convPath;

	FileDirectory *fileDir = new FileDirectory(convName.TCHAR_TO_MBCS(gDirName), 
        convPath.TCHAR_TO_MBCS(gDirPath));
	MediaServer *mserver = gMediaGate->getMediaServer();
	mserver->addContentDirectory(fileDir);
	UpdateDirectoryList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  InitApp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void InitApp()
{
    FUNCTION_BLOCK_NAME_TRACE( TEXT("InitApp"),100);
	gMediaGate = NULL;
    {
        FUNCTION_BLOCK_NAME_TRACE(TEXT("new MediaGate"), 100);
        gMediaGate = new MediaGate(MediaGate::FILESYS_MODE);
    }
    
    {
        FUNCTION_BLOCK_NAME_TRACE(TEXT("loadPreferences"), 100);
	    gMediaGate->loadPreferences(gPreferenceFileName.c_str());
    }
    {
        FUNCTION_BLOCK_NAME_TRACE(TEXT("UpdateDirectoryList"), 100);
        UpdateDirectoryList();
    }
	
    {
        FUNCTION_BLOCK_NAME_TRACE(TEXT("start"), 100);
	    gMediaGate->start();
    }
}

void ExitApp()
{
	gMediaGate->stop();
	gMediaGate->savePreferences(gPreferenceFileName.c_str());
	delete gMediaGate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MainWndProc
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Message,WPARAM wParam,LPARAM lParam)
{
	static HFONT font;

	switch(Message){
	case WM_CREATE:
		{
			InitCommonControls();
			ghMainWindow=hWnd;
			gDirectoryWindowWidth = DEFAULT_DIRECTORY_WINDOW_SIZE;
			font = CreateWindowFont();

			ghDirectoryListWindow = CreateDirectoryListWindow(ghMainWindow, ghInstance);
			SendMessage(ghDirectoryListWindow, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

			ghItemListWindow = CreateItemListWindow(ghMainWindow, ghInstance);
			SendMessage(ghItemListWindow, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

			ghAddWindow = CreateButton(ghMainWindow, ghInstance, TEXT("Add"));
			SendMessage(ghAddWindow, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

			ghDelWindow = CreateButton(ghMainWindow, ghInstance, TEXT("Del"));
			SendMessage(ghDelWindow, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

			InitApp();
		}
		return TRUE;
	case WM_DESTROY:
		{
			ExitApp();
			DeleteObject(font);
			PostQuitMessage(0);
		}
		return TRUE;
	case WM_SIZE:
		{
			ResetWindowSize();
		}
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			RECT rect;
			GetClientRect(hWnd,&rect);
			rect.left = gDirectoryWindowWidth;
			rect.right = gDirectoryWindowWidth + SPLIT_BORDER_WIDTH;
			HDC hdc = BeginPaint(hWnd , &ps);
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
			EndPaint(hWnd , &ps);
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		{
			RECT LeftWindowRect;
			GetWindowRect(ghDirectoryListWindow,&LeftWindowRect);
			short xPos=(short)LOWORD(lParam);
			gMouseOffset=xPos-(LeftWindowRect.right-LeftWindowRect.left);
			SetCapture(ghMainWindow);
			SetDirectoryWindowWidth(xPos-gMouseOffset);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			if(GetCapture()==ghMainWindow){
				short xPos=(short)LOWORD(lParam);
				SetDirectoryWindowWidth(xPos-gMouseOffset);
			}
		}
		return TRUE;
	case WM_LBUTTONUP:
		{
			if(GetCapture()==ghMainWindow){
				short xPos=(short)LOWORD(lParam);
				SetDirectoryWindowWidth(xPos-gMouseOffset);
				ReleaseCapture();
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			HWND chWnd = (HWND)lParam;
			int comType = (int)HIWORD(wParam);
			int comID = (int)LOWORD(wParam);
			if (chWnd == ghDirectoryListWindow) {
				if (comType == LBN_SELCHANGE) {
					int selID = SendMessage(ghDirectoryListWindow, LB_GETCURSEL, 0, 0);
					UpdateItemList(selID);
				}
			}
			else if (chWnd == ghAddWindow) {
				if (comType == BN_CLICKED)
					OpenAddDialog(hWnd);
			}
			else if (chWnd == ghDelWindow) {
				if (comType == BN_CLICKED) {
					int selID = SendMessage(ghDirectoryListWindow, LB_GETCURSEL, 0, 0);
					DeleteDirectoryList(selID);
				}
			}
		}
		return TRUE;
	}
	return DefWindowProc(hWnd,Message,wParam,lParam);
}

int PASCAL WinMain(HINSTANCE hInst,HINSTANCE  hPrevInstance,LPSTR lpszCmdLine,int nCmdShow)
{
    FUNCTION_BLOCK_TRACE(0);
    uHTTP::Debug::on();

    uHTTP::Logger *pLogger = uHTTP::Logger::GetSharedInstance();
    pLogger->setLevel(uHTTP::LoggerLevel::TRACE);
    pLogger->addTarget(new uHTTP::LoggerOutputDebugString());

    //uHTTP::Debug::message("some information\n");

	ghInstance = hInst;

	gPreferenceFileName = MediaGate::DEFAULT_PREFERENCE_FILENAME;
	if (0 < strlen(lpszCmdLine))
		gPreferenceFileName = lpszCmdLine;

	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc=MainWndProc;
	wc.hInstance=ghInstance;
	wc.hIcon = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_ICON));  
	wc.hCursor=LoadCursor(NULL,IDC_SIZEWE);
	wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName=WINDOW_CLASS;
	wc.lpszMenuName= NULL;/*MAKEINTRESOURCE(IDR_MENU)*/;  
	RegisterClass(&wc);

	CreateWindow(
		WINDOW_CLASS,
		WINDOW_TITLE,
		WS_POPUPWINDOW | WS_CLIPCHILDREN | WS_CAPTION | WS_THICKFRAME | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		DEFAULT_WINDOW_SIZE, DEFAULT_WINDOW_HEIGHT,
		NULL,
		NULL,
		ghInstance,
		NULL);

	MSG msg;
	while(GetMessage(&msg,NULL,NULL,NULL)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

