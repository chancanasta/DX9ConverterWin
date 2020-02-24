#ifndef __DX9CONVERTER_H
#define __DX9CONVERTER_H

BOOL AllocateBuffers(VOID);
VOID FreeBuffers(VOID);

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped
);

int DX9Convert(TCHAR *inFile,HWND hWnd);

#endif
