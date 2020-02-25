#include "stdafx.h"
#include "DX9ConverterWin.h"
#include "FileConstants.h"
#include "FMStructs.h"
#include "DXDisplay.h"

TCHAR tWork[BUFSIZE];

//loop through and display a bulk voice data dump
void DisplayBulkVoices(UCHAR *readBuffer)
{
	DWORD inBufferPos = 6;

	//'old' format file - so just display it
	lpFM_BULK_OLD_PATCH lpOldPatch;
	//dump the voice data details 
	for (int i = 0; i < 32; i++, inBufferPos += FMDX9_PATCH_DATA_SIZE)
	{
		lpOldPatch = (lpFM_BULK_OLD_PATCH)&readBuffer[inBufferPos];
		DisplayVoice(lpOldPatch, i);
	}
}

//display some details of an old (DX9) voice
void DisplayVoice(lpFM_BULK_OLD_PATCH lpOldPatch, int patchNo)
{
	char dispName[12];
	dispName[10] = 0;
	memcpy(dispName, lpOldPatch->PatchName, 10);
	ConsoleOut(L"\r\n\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	swprintf(tWork, BUFSIZE, TEXT("[%02d] Name %S\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\nAlgo:%d , Trans:%d KeySync:%d FB:%d\r\n"),
		patchNo + 1, dispName, lpOldPatch->Algorithm.Algorithm, lpOldPatch->Lfo.Transpose, (lpOldPatch->Algorithm.OscKeySyncFeedback >> 3) & 1, lpOldPatch->Algorithm.OscKeySyncFeedback & 7);
	ConsoleOut(tWork);

	for (int i = 0; i < 4; i++)
	{
		int op = 6 - i;
		swprintf(tWork, BUFSIZE, TEXT("----------- OP %d --------------\r\n"), op);
		ConsoleOut(tWork);
		//operator ratio		
		swprintf(tWork, BUFSIZE, TEXT("Coarse:%02d Fine:%02d Det:%02d AModSen:%d KSRate:%d KSLevel:%d OutLevel:%02d\r\n"),
			(lpOldPatch->FMOp[i].FreqCMode >> 1),
			lpOldPatch->FMOp[i].FreqFine,
			(lpOldPatch->FMOp[i].OscDetuneRateScale >> 3) & 15,
			lpOldPatch->FMOp[i].KeyVelSensAModSens & 3,
			(lpOldPatch->FMOp[i].OscDetuneRateScale & 7),
			lpOldPatch->FMOp[i].ScaleRightDepth, //Where the DX9 stores Keyboard scaling level
			lpOldPatch->FMOp[i].OutputLevel);
		ConsoleOut(tWork);

		//envelope
		swprintf(tWork, BUFSIZE, L"L1:%02d L2:%02d L3:%02d L4:%02d\r\n", lpOldPatch->FMOp[i].Level1, lpOldPatch->FMOp[i].Level2, lpOldPatch->FMOp[i].Level3, lpOldPatch->FMOp[i].Level4);
		ConsoleOut(tWork);
		swprintf(tWork, BUFSIZE, L"R1:%02d R2:%02d R3:%02d R4:%02d\r\n", lpOldPatch->FMOp[i].Rate1, lpOldPatch->FMOp[i].Rate2, lpOldPatch->FMOp[i].Rate3, lpOldPatch->FMOp[i].Rate4);
		ConsoleOut(tWork);

		ConsoleOut(TEXT("\r\n"));

	}
	//LFO
	swprintf(tWork, BUFSIZE, L"LFO Delay:%02d Speed:%02d PMD:%02d AMD:%02d PMS:%d Wave:%d",
		lpOldPatch->Lfo.LFODelay,
		lpOldPatch->Lfo.LFOSpeed,
		lpOldPatch->Lfo.LFOPitchModDepth,
		lpOldPatch->Lfo.LFOAmpModDepth,
		//breakdown LFOPModWaveSync
		//b6 5 4 - LFO Pitch Mod Sensitivity (0-7)
		//b3 2 1 - LFO Wave (0-5)
		//b0	 - Sync (0-1)
		(lpOldPatch->Lfo.LFOPModWaveSync >> 4) & 7,
		(lpOldPatch->Lfo.LFOPModWaveSync >> 1) & 7
	);
	ConsoleOut(tWork);
}

//display details from a single voice data dump
void DisplaySingleVoice(UCHAR *readBuffer)
{

	lpFM_SINGLE_OLD_PATCH lpSingleOldPatch;
	DWORD inBufferPos = 6;
#ifdef _DX9_DEBUG
	swprintf_s(tWork, BUFSIZE, L"Size of single voice :%d\r\n", (int)sizeof(FM_SINGLE_OLD_PATCH));
	ConsoleOut(tWork);
#endif
	lpSingleOldPatch = (lpFM_SINGLE_OLD_PATCH)&readBuffer[inBufferPos];
	char dispName[12];
	dispName[10] = 0;
	memcpy(dispName, lpSingleOldPatch->PatchName, 10);
	swprintf_s(tWork, BUFSIZE, L"Patch Name %S\r\n", dispName);
	ConsoleOut(tWork);

	ConsoleOut(L"\r\n\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	swprintf(tWork, BUFSIZE, TEXT("Name %S\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\nAlgo:%d , Trans:%d KeySync:%d FB:%d\r\n"),
		dispName,
		lpSingleOldPatch->Alogrithm.Algorithm,
		lpSingleOldPatch->Lfo.Transpose,
		lpSingleOldPatch->Alogrithm.OscSync,
		lpSingleOldPatch->Alogrithm.Feedback);
		

	ConsoleOut(tWork);

	for (int i = 0; i < 4; i++)
	{
		int op = 6 - i;
		swprintf(tWork, BUFSIZE, TEXT("----------- OP %d --------------\r\n"), op);
		ConsoleOut(tWork);
		//operator ratio		
		swprintf(tWork, BUFSIZE, TEXT("Coarse:%02d Fine:%02d Det:%02d AModSen:%d KSRate:%d KSLevel:%d OutLevel:%02d\r\n"),
			lpSingleOldPatch->FMOp[i].FreqCoarse,
			lpSingleOldPatch->FMOp[i].FreqFine,
			lpSingleOldPatch->FMOp[i].OscDetune,
			lpSingleOldPatch->FMOp[i].AmpModSens,
			lpSingleOldPatch->FMOp[i].KeyRateScaling,
			lpSingleOldPatch->FMOp[i].ScaleRightDepth,
			lpSingleOldPatch->FMOp[i].OutputLevel);
		ConsoleOut(tWork);

		//envelope
		swprintf(tWork, BUFSIZE, L"L1:%02d L2:%02d L3:%02d L4:%02d\r\n", 
			lpSingleOldPatch->FMOp[i].Level1, lpSingleOldPatch->FMOp[i].Level2, lpSingleOldPatch->FMOp[i].Level3, lpSingleOldPatch->FMOp[i].Level4);
		ConsoleOut(tWork);
		swprintf(tWork, BUFSIZE, L"R1:%02d R2:%02d R3:%02d R4:%02d\r\n", 
			lpSingleOldPatch->FMOp[i].Rate1, lpSingleOldPatch->FMOp[i].Rate2, lpSingleOldPatch->FMOp[i].Rate3, lpSingleOldPatch->FMOp[i].Rate4);
		ConsoleOut(tWork);

		ConsoleOut(TEXT("\r\n"));

	}
	//LFO
	swprintf(tWork, BUFSIZE, L"LFO Delay:%02d Speed:%02d PMD:%02d AMD:%02d PMS:%d Wave:%d",
		lpSingleOldPatch->Lfo.LFODelay,
		lpSingleOldPatch->Lfo.LFOSpeed,
		lpSingleOldPatch->Lfo.LFOPitchModDepth,
		lpSingleOldPatch->Lfo.LFOAmpModDepth,
		lpSingleOldPatch->Lfo.PitchModSens,
		lpSingleOldPatch->Lfo.WaveForm);

	ConsoleOut(tWork);



}