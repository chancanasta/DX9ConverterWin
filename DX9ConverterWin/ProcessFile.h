#ifndef __PROCESSFILE_H
#define __PROCESSFILE_H


//main processing
BOOL ProcessFile(HANDLE hFile, PHANDLE phInputBuffer, PHANDLE phOutputBuffer, PHANDLE phNameBuffer, int midiChannel, TCHAR *fileName);
//format of data dump
WORD GetDumpFormat(PUCHAR readBuffer);
//initial validation checks
BOOL ValidDXSysEx(PUCHAR readBuffer, DWORD  dwBytesRead);
//convert patch
BOOL ConvertVoice(lpFM_BULK_NEW_PATCH lpNewPatch, int PatchNo, lpFM_BULK_OLD_PATCH lpOldPatch);

//Which calls these...
//convert operator
void OperatorConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch);
//convert from 'new' envelope to 'old' one
void EGConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld);
//convert the frequency params
void FreqConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld);
//convert the alogrithm params
void AlgorithmConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch);
//convert the LFO params
void LFOConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch);
//default out all the easy stuff in the patch
void SetDefaults(lpFM_BULK_OLD_PATCH lpOldPatch, int PatchNo);
//set the check byte
void SetCheckSum(UCHAR *writeBuffer,BOOL display);
//calculate the coarse, fine and detune frequencies for the DX9 new style operator
void CalcFreqDetune(lpFM_BULK_OPERATOR_NEW lpOperatorNew, UCHAR *pCoarseOut, UCHAR *pFineOut, UCHAR *pDetuneOut);

//Single voice dump
void SetSingleDefaults(lpFM_SINGLE_OLD_PATCH lpOldPatch);

//write out the buffers for the 2 files (sysex and text file of patch names)
BOOL WriteDX9BulkFile(LPCWSTR fileName, UCHAR *outBuffer);
BOOL WriteDX9PatchNames(LPCWSTR fileName, LPCSTR outBuffer);

//convert bulk voices
void ConvertBulkVoices(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer,int midiChannel);
void ConvertSingleVoice(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer, int midiChannel);

//handle the patch names
void getPatchName(char *outName, char *patchName);
void getStrName(lpFM_BULK_NEW_PATCH lpNewPatch, char *dispName);

#endif

