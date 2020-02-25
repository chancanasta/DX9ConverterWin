#ifndef __DX9CONVERTERWIN_H
#define __DX9CONVERTERWIN_H

#include <shobjidl.h> 

#include "resource.h"

//uncomment for more debug output
//#define _DX9_DEBUG

void GetFileName(HWND hWnd);
void ConvertFiles(HWND hWnd);

HRESULT AddSelections(IFileOpenDialog *pfd, HWND hWnd);

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//'console' output
void ClearOutput();
void ConsoleOut(TCHAR *output);

//file list functions
void ClearList(HWND hWnd);
void SelectAll(HWND hWnd);
void SelectNone(HWND hWnd);
void SetListButtons();
void RemoveItems();

//log file

void OpenLogFile();
void CloseLogFile();

#define MAX_CONSOLE_LINES	512
#endif