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
void SingleOperatorConvert(lpFM_SINGLE_NEW_PATCH lpNewPatch, lpFM_SINGLE_OLD_PATCH lpOldPatch, lpFM_SINGLE_ADD_PARAMS lpFMAdditional = NULL);

//convert from 'new' envelope to 'old' one
void EGConvert(lpFM_EG_NEW lpEgNew, lpFM_EG_OLD lpEgOld);
//convert the frequency params
void FreqConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld, lpFM_BULK_ADD_OPERATOR lpAddOperator);
void SingleFreqConvert(lpFM_SINGLE_OPERATOR_NEW lpOperatorNew, lpFM_SINGLE_OPERATOR_OLD lpOperatorOld, lpFM_SINGLE_ADD_OP lpFMAddOperator = NULL);

//convert the alogrithm params
void AlgorithmConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch);
//convert the LFO params
void LFOConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch);
void SingleLFOConvert(lpFM_SINGLE_NEW_PATCH lpNewPatch, lpFM_SINGLE_OLD_PATCH lpOldPatch);
//default out all the easy stuff in the patch
void SetDefaults(lpFM_BULK_OLD_PATCH lpOldPatch, int PatchNo, char *dispName, BOOL oldName);
void SetSingleDefaults(lpFM_SINGLE_OLD_PATCH lpOldPatch);
//the checksum
UCHAR CalcChecksum(UCHAR *writeBuffer, int startPos, int dataLen);
//calculate the coarse, fine and detune frequencies for the DX9 new style operator
void CalcFreqDetune(UCHAR freqIn, UCHAR fineIn, UCHAR detuneIn, UCHAR *pCoarseOut, UCHAR *pFineOut, UCHAR *pDetuneOut);



//write out the buffers for the 2 files (sysex and text file of patch names)
BOOL WriteDX9BulkFile(LPCWSTR fileName, UCHAR *outBuffer, int outLen);
BOOL WriteDX9PatchNames(LPCWSTR fileName, LPCSTR outBuffer);

//convert bulk voices
void ConvertBulkVoices(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer,int midiChannel);
void ConvertSingleVoice(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer, int midiChannel, lpFM_SINGLE_ADD_PARAMS lpFMAdditional = NULL);
void ConvertAdditionalVoice(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer, int midiChannel, DWORD dwBytesRead);

//handle the patch names
void getPatchName(char *outName, char *patchName);
void getStrName(lpFM_BULK_NEW_PATCH lpNewPatch, char *dispName);
void getStrSingleName(lpFM_SINGLE_NEW_PATCH lpNewPatch, char *dispName);

#endif

