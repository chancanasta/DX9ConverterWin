// DX9ConverterWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DX9ConverterWin.h"
#include "DX9Converter.h"
#include "FileConstants.h"
#include <stdio.h>


#pragma comment(lib, "comctl32.lib")


#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HMENU hMenu;
HWND hMainWindow = NULL;
HWND hFileList = NULL;
BOOL bLogFile = FALSE;
HANDLE hLogFile = NULL;
char workBuf[BUFSIZE];
SYSTEMTIME sysTime;
DWORD bytesWritten;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	(lpCmdLine);

	MSG msg;
	BOOL ret;

	InitCommonControls();
	hMainWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, DialogProc, 0);
	ShowWindow(hMainWindow, nCmdShow);

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(hMainWindow, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

//Callback for the dialog
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD wmID = LOWORD(wParam);
	DWORD wmEvent = HIWORD(wParam);

	switch (uMsg)
	{
	case WM_INITDIALOG:
//set our menu
		hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDC_DX9CONVERTERWIN));
		SetMenu(hDlg, hMenu);		
		EnableMenuItem(hMenu, IDM_CONVERT, MF_DISABLED|MF_GRAYED);
		OpenLogFile();
//get our file list
		hFileList = GetDlgItem(hDlg, IDC_FILELIST);
		break;
	case WM_COMMAND:
		switch (wmID)
		{
		case IDC_FILELIST:
			switch (wmEvent)
			{
			case LBN_SELCHANGE:			
				SetListButtons();
				break;
			case LBN_DBLCLK:
				ConvertFiles(hDlg);
				break;
			}
			break;
		case IDM_EXIT:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDM_OPEN:
		case IDC_BROWSEBUTTON:
			GetFileName(hDlg);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, About);
			break;
		case IDC_CLEARLIST:
			ClearList(hDlg);
			break;
		case IDC_SELECTALL:
			SelectAll(hDlg);
			break;
		case IDC_SELECTNONE:
			SelectNone(hDlg);
			break;
		case IDM_CONVERT:
		case IDC_CONVERTBUTTON:
			ConvertFiles(hDlg);
			break;
		case IDC_CLEAROUTPUT:
			ClearOutput();
			break;
		case IDC_REMOVE:
			RemoveItems();
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hDlg);
		return TRUE;

	case WM_DESTROY:
		DestroyMenu(hMenu);
		CloseLogFile();
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

void ConsoleOut(TCHAR *output)
{
	HWND hOutput = GetDlgItem(hMainWindow, IDC_OUTPUT);
	
	DWORD noLines=(DWORD)SendMessage(hOutput, EM_GETLINECOUNT, 0, 0);
	if (noLines > MAX_CONSOLE_LINES)	
		SetDlgItemText(hMainWindow, IDC_OUTPUT, L"");

	
	if (bLogFile)
	{
		DWORD bytesWritten;
		int byteLen;
		byteLen = WideCharToMultiByte(CP_UTF8, 0, output, (int)_tcslen(output), workBuf, BUFSIZE, NULL, NULL);
		if (byteLen)
		{
			workBuf[byteLen] = 0;
			if (!WriteFile(hLogFile, workBuf, (DWORD)byteLen+1, &bytesWritten, NULL))
			{
				SendMessage(hOutput, EM_SETSEL, -1, -1);
				SendMessage(hOutput, EM_REPLACESEL, FALSE, (LPARAM)L"** Unable to write to log file");
			}
		}
	}
	SendMessage(hOutput, EM_SETSEL, -1, -1);
	SendMessage(hOutput, EM_REPLACESEL, FALSE, (LPARAM)output);
}

void ClearOutput()
{
	HWND hOutput = GetDlgItem(hMainWindow, IDC_OUTPUT);
	SetDlgItemText(hMainWindow, IDC_OUTPUT, L"");
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX9CONVERTERWIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
      return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void SelectAll(HWND hWnd)
{
	SendMessage(hFileList, LB_SELITEMRANGE, (WPARAM)TRUE, (LPARAM)(LOWORD(0) | HIWORD(-1)));
	SetListButtons();
}

void SelectNone(HWND hWnd)
{
	SendMessage(hFileList, LB_SELITEMRANGE, (WPARAM)FALSE, (LPARAM)(LOWORD(0)|HIWORD(-1)));
	SetListButtons();
}

void ClearList(HWND hWnd)
{
	if (MessageBox(hWnd, TEXT("Clear the file list ?"), TEXT("Clear"),
		MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		SendMessage(hFileList, LB_RESETCONTENT, 0, 0);
		SetListButtons();
	}
}

void ConvertFiles(HWND hWnd)
{
	TCHAR *inBuffer = NULL;
	DWORD textLen;
	DWORD maxTextLen = 0;
	DWORD fileItems;
	int selItems[BUFSIZE];
//get the array of selected items
	fileItems= (DWORD)SendMessage(hFileList, LB_GETSELITEMS, BUFSIZE, (LPARAM)selItems);
	
	if (fileItems > 0)
	{
		//loop through
		for (DWORD i = 0; i < fileItems; i++)
		{
			textLen = (DWORD)SendMessage(hFileList, LB_GETTEXTLEN, selItems[i], 0);
			if (textLen > maxTextLen)
			{
				inBuffer = new TCHAR[textLen + 1];
				maxTextLen = textLen;
			}
//get the selected item text
			SendMessage(hFileList, LB_GETTEXT, selItems[i], (LPARAM)inBuffer);
//unselect
			SendMessage(hFileList, LB_SETSEL, FALSE, (LPARAM)selItems[i]);
//convert the file
			DX9Convert(inBuffer, hWnd);
			Yield();
		}
		//delete out buffer if we allocated anything	
		if (inBuffer)
			delete inBuffer;
	}
//set the button states
	SetListButtons();
	
}

void GetFileName(HWND hWnd)
{	
	SetDlgItemText(hWnd, IDC_OUTPUT, L"");	
	DWORD index;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			const COMDLG_FILTERSPEC c_rgSaveTypes[] =
			{
				{ L"Sysex Files",       L"*.syx" },
				{ L"All Files",         L"*.*" }
			};


			pFileOpen->SetFileTypes(2, c_rgSaveTypes);
			FILEOPENDIALOGOPTIONS fodo;			
			pFileOpen->GetOptions(&fodo);
			pFileOpen->SetOptions(fodo | FOS_ALLOWMULTISELECT);
			hr = pFileOpen->Show(NULL);
			IShellItem *pItem;
			hr = pFileOpen->GetResult(&pItem);	
			if (SUCCEEDED(hr))
			{				
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				//Add single file name to list box
				if (SUCCEEDED(hr))
				{	

					index=(DWORD)SendMessage(hFileList, LB_ADDSTRING,0, (LPARAM)pszFilePath);					
					if (index != LB_ERR)
						SendMessage(hFileList, LB_SELITEMRANGEEX, index, index + 1);
					
					CoTaskMemFree(pszFilePath);
				}
				pItem->Release();
			}
			else
//check for multiple selections
				AddSelections(pFileOpen,hWnd);
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	SetListButtons();
}

HRESULT AddSelections(IFileOpenDialog *pfd,HWND hWnd)
{
	HRESULT hr;
	DWORD index;

	// To Allow for multiple selections the code called IFileOpenDialog::GetResults
	// which returns an IShellItemArray object when the 'Open' button is clicked

	IShellItemArray *psiaResult;

	hr = pfd->GetResults(&psiaResult);	
	if (SUCCEEDED(hr))
	{
		PWSTR pszFilePath = NULL;
		DWORD dwNumItems = 0; // number of items in multiple selection		

		hr = psiaResult->GetCount(&dwNumItems);  // get number of selected items		

// Loop through IShellItemArray and add the items
		for (DWORD i = 0; i < dwNumItems; i++)
		{
			IShellItem *psi = NULL;

			hr = psiaResult->GetItemAt(i, &psi); // get a selected item from the IShellItemArray

			if (SUCCEEDED(hr))
			{
				hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				if (SUCCEEDED(hr))
				{
					index=(DWORD)SendMessage(hFileList, LB_ADDSTRING, 0, (LPARAM)pszFilePath);
					if (index != LB_ERR)
						SendMessage(hFileList, LB_SELITEMRANGEEX, index, index + 1);

					CoTaskMemFree(pszFilePath);
				}

				psi->Release();
			}
		}		
		psiaResult->Release();

	}

	return hr;
}
//set the enable/disable states of the buttons
void SetListButtons()
{
	int noSelItems;
	int noItems;
	HWND butSelectAll,butSelectNone,butRemove,butClear,butConvert;
//get all the buttons
	butSelectAll = GetDlgItem(hMainWindow, IDC_SELECTALL);
	butSelectNone = GetDlgItem(hMainWindow, IDC_SELECTNONE);
	butRemove= GetDlgItem(hMainWindow, IDC_REMOVE);
	butClear = GetDlgItem(hMainWindow, IDC_CLEARLIST);
	butConvert = GetDlgItem(hMainWindow, IDC_CONVERTBUTTON);

	
//get number of list items and number of selected items
	noItems = (int)SendMessage(hFileList, LB_GETCOUNT, 0, 0);
	noSelItems = (int)SendMessage(hFileList, LB_GETSELCOUNT, 0, 0);
	if (noItems > 0)
	{
//some items
		EnableWindow(butClear, TRUE);
		if (noSelItems > 0)
		{
			EnableWindow(butRemove,  TRUE);			
			EnableWindow(butConvert, TRUE);
			EnableMenuItem(hMenu, IDM_CONVERT, MF_ENABLED);
		}
		else
		{
			EnableWindow(butRemove, FALSE);
			EnableWindow(butConvert,  FALSE);
			EnableMenuItem(hMenu, IDM_CONVERT, MF_DISABLED | MF_GRAYED);
		}
		if (noSelItems != noItems)
			EnableWindow(butSelectAll, TRUE);
		else
			EnableWindow(butSelectAll, FALSE);

		if (noSelItems == 0)
			EnableWindow(butSelectNone, FALSE);
		else
			EnableWindow(butSelectNone, TRUE);
	}
	else
	{
//no items
		EnableWindow(butSelectAll, FALSE);
		EnableWindow(butSelectNone,  FALSE);
		EnableWindow(butRemove, FALSE);
		EnableWindow(butClear, FALSE);
		EnableWindow(butConvert, FALSE);
		EnableMenuItem(hMenu, IDM_CONVERT, MF_DISABLED | MF_GRAYED);
	}
		
}

void RemoveItems()
{
	int fileItems;	

	int selItems[BUFSIZE];
	//get the array of selected items
	fileItems = (DWORD)SendMessage(hFileList, LB_GETSELITEMS, BUFSIZE, (LPARAM)selItems);

	if (fileItems > 0)
	{
		if (MessageBox(hMainWindow, TEXT("Remove Selected Items from List ?"), TEXT("Remove Items"),
			MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
//need to go through the list backwards			
			for (int i = fileItems-1; i >=0; i--)
				SendMessage(hFileList, LB_DELETESTRING, selItems[i], 0);
		}
	}
	SetListButtons();
}

void OpenLogFile()
{
	hLogFile = CreateFile(L"DX9ConverterWin.log",                // name of the file
		GENERIC_WRITE,          // open for writing
		FILE_SHARE_READ,	// log can be read before it's closed
		NULL,                   // default security
		CREATE_ALWAYS,             // create file always, even if it already exists
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hLogFile != INVALID_HANDLE_VALUE)
	{
		GetSystemTime(&sysTime);
		sprintf_s(workBuf, BUFSIZE, "------- Log Opened  %02d/%02d/%04d %02d:%02d:%02d  ------- \r\n",		
			sysTime.wDay, sysTime.wMonth, sysTime.wYear,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		WriteFile(hLogFile, workBuf, (DWORD)strlen(workBuf) + 1, &bytesWritten, NULL);
		bLogFile = TRUE;
	}
}

void CloseLogFile()
{
	if (bLogFile)
	{
		GetSystemTime(&sysTime);
		sprintf_s(workBuf, BUFSIZE, "\r\n=========== Log Closed %02d/%02d/%04d %02d:%02d:%02d ===========\r\n",
			sysTime.wDay, sysTime.wMonth, sysTime.wYear,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		WriteFile(hLogFile, workBuf, (DWORD)strlen(workBuf) + 1, &bytesWritten, NULL);

		CloseHandle(hLogFile);
	}
	bLogFile = FALSE;
}