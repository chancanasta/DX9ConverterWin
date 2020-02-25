//where most of the file processing happens
#include "stdafx.h"
#include "DX9ConverterWin.h"
#include "FileConstants.h"
#include "SysExConstants.h"
#include "FMStructs.h"
#include "FMConversion.h"
#include "ProcessFile.h"
#include "DXDisplay.h"
#include <Shlwapi.h>
#include <math.h> 
#include <stdio.h>
OVERLAPPED ol = { 0 };
TCHAR genOut[BUFSIZE];

//main file processing funtiona
BOOL ProcessFile(HANDLE hFile, PHANDLE phInputBuffer, PHANDLE phOutputBuffer, PHANDLE phNameBuffer,int midiChannel,TCHAR *fileName)
{
	DWORD dwBytesRead = 0;	
//buffers
	UCHAR *readBuffer;
	UCHAR *writeBuffer;		
					
	WORD dumpFormat;
//try to read the file in	
	if (!ReadFile(hFile, phInputBuffer, INPUTBUFFERSIZE, &dwBytesRead, &ol))
	{
		wsprintf(genOut, L"Failed to read file - error %d\n", GetLastError());
		ConsoleOut(genOut);		
		CloseHandle(hFile);
		return FALSE;
	}

	readBuffer = (UCHAR *)phInputBuffer;
	writeBuffer = (UCHAR *)phOutputBuffer;
//check it's a valid SYSEX file
	if (!ValidDXSysEx(readBuffer, dwBytesRead))
	{
		//close the file
		CloseHandle(hFile);
		return FALSE;
	}
		
//figure out which format it is
	dumpFormat = GetDumpFormat(readBuffer);
	
	switch (dumpFormat)
	{
		case SINGLE_VOICE:
			ConsoleOut(L"Single voice data dump\r\n");
			break;
		case BULK_VOICE:
			ConsoleOut(L"Bulk voice data dump\r\n");
			ConvertBulkVoices(hFile,readBuffer, writeBuffer,midiChannel);
			break;
		case OLD_SINGLE_VOICE:			
			ConsoleOut(L"Original DX single voice dump (DX7/DX9)\r\n");
			DisplaySingleVoice(readBuffer);
			break;
		case OLD_BULK_VOICE:
			ConsoleOut(L"Origingal DX bulk voice dump (DX7/9)\r\nDisplaying settings\r\n");			
			DisplayBulkVoices(readBuffer);
			break;
		default:
			ConsoleOut(L"Invalid Voice Dump format\r\n");
//close the file
			CloseHandle(hFile);
			return FALSE;
	}
	
	
	
	return TRUE;
}




//get the data dump format
WORD GetDumpFormat(PUCHAR readBuffer)
{
	switch (readBuffer[3])
	{
		case 0x00:
			return OLD_SINGLE_VOICE;
		case 0x03:
			return SINGLE_VOICE;
		case 0x04:
			return BULK_VOICE;		
		case 0x09:
			return OLD_BULK_VOICE;
	}
	return FORMAT_ERROR;
}
//Check that it's a valid SysEx File
BOOL ValidDXSysEx(PUCHAR readBuffer,DWORD  dwBytesRead)
{
//check the easy bytes
//start
	if (readBuffer[0] != START_BYTE)
	{
		ConsoleOut(L"**** Not a MIDI Sysex file\r\n");
		return FALSE;
	}
//manufacturer's ID
	if (readBuffer[1] != ID_BYTE)
	{		
		ConsoleOut(L"**** Not a Yamaha SysEx File\r\n");

	}
//MIDI channel
	if (readBuffer[2] > 15)
	{
		ConsoleOut(L"**** Invalid Midi channel in Sysex file\r\n");
		return false;
	}
	else
	{
		swprintf(genOut, BUFSIZE, TEXT("SysEx MIDI Channel value (%d)\n"), (int)readBuffer[2] + 1);
		ConsoleOut(genOut);		
	}


//End byte
	if (readBuffer[dwBytesRead - 1] != END_BYTE)
	{
		ConsoleOut(L"Invalid Sysex File (incorrect last byte)\r\n");
		return FALSE;
	}
	
	return TRUE;
}

void OperatorConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch)
{
	lpFM_BULK_OPERATOR_NEW lpNewOperator;
	lpFM_BULK_OPERATOR_OLD lpOldOperator;
	UCHAR work;
	UCHAR algo;
	FM_DX9_CARRIERS OpsMap;

/*
loop through the 4 operators
Note that the operators oddly and helpfully go 4,2,3,1
*/

//get the algorithm, as we may need to remap the operators (algorithm 3 (7))
	algo = (lpNewPatch->LFOParams.SyncFBackAlgo)&0x07;
//get the map
	OpsMap = OPERATORMAP[algo];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("Algo :%d \r\n"), algo);
	ConsoleOut(genOut);
#endif
	for (int i = 0; i < 4; i++)
	{
//get pointers to the operators
		lpNewOperator = &lpNewPatch->FMOp[OpsMap.Operators[i]];
		lpOldOperator = &lpOldPatch->FMOp[i];
//Envlopes
#ifdef _DX9_DEBUG
		swprintf(genOut, BUFSIZE, TEXT("Ops i -%d from %d, EG %d - "), i, OpsMap.Operators[i], (i + 1));
		ConsoleOut(genOut);
#endif
		EGConvert(lpNewOperator, lpOldOperator);
//Frequency
		FreqConvert(lpNewOperator, lpOldOperator);
//General Operator values
//output level
		lpOldOperator->OutputLevel = FM_DX9_OUTLEVEL[lpNewOperator->OutputLevel];
//DX9 has simple keyboard scaling compared to DX7, just rate and level
//scaling rate
//breakdown RateScalingDetune
//b4 3 - Keyboard Scaling rate (0-3)
//b2 1 0 - Detune (0-6, centre is 3)
		work = (lpNewOperator->RateScalingDetune>>3)&0x03;
//rate scaling in bottom 3 bits
		lpOldOperator->OscDetuneRateScale |= SCALINGRATECONVERT[work];
//scaling level
//DX9 uses the ScaleRightDepth parameter for level scaling
		lpOldOperator->ScaleRightDepth = lpNewOperator->LevelScaling;
//default the other scaling parameters
		lpOldOperator->BreakPoint = 0x0F;
		lpOldOperator->ScaleLeftDepth = 0x00;
		lpOldOperator->ScaleLRCurve = 0x04;
				
//key velocity is always zero, just need amplitute mod sensitvity (comes from performance parameters)
		int amod = (lpNewPatch->PerfParameters.PmsAmsLFOWave >> 2) & 3;
//Amplitute mod enable is on/off per operator, so get flag
		int ame = (lpNewOperator->AmeEGBiasVel >> 6) & 1;
//then set amp mod for this operator		
		lpOldOperator->KeyVelSensAModSens = amod*ame;
	}
}

//calculate the coarse, fine and detune frequencies for the DX9 new style operator
void CalcFreqDetune(lpFM_BULK_OPERATOR_NEW lpOperatorNew, UCHAR *pCoarseOut, UCHAR *pFineOut, UCHAR *pDetuneOut)
{
	UCHAR newFreq;
	UCHAR coarse = 0;
	UCHAR fine = 0;
	UCHAR work = 0;
	UCHAR newDetune;
	UCHAR oldDetune;
	double newFreqVal;
	double oldFreqVal;
	double freqFine;
	double fineStep;
	double detuneStep;
	double fineWork;
	double calcFreq;
	int i;
	//get the frequency (0-63) from new 4 op
	newFreq = lpOperatorNew->OSCFreq;

	//get a double that represents that ratio
	newFreqVal = DX21FREQ[newFreq];
	//derive coarse and fine values
	i = 0;
	//find the nearest coarse freq that is less than or equal to the 4op freq
	while (i<32 && DX9COARSEFREQ[i] <= newFreqVal)
		i++;
	if (i > 0)
		i--;

	//the index now points to our coarse setting
	oldFreqVal = DX9COARSEFREQ[i];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("In freq (%d) %d (%f) -> %d (%f) "), lpOperatorNew->OSCFreq, newFreq, newFreqVal, i, oldFreqVal);
	ConsoleOut(genOut);
#endif
	coarse = i;
	//given this coarse freq, get the fine steps
	fineStep = oldFreqVal / 100.0F;
	//see if there is any fine needed i.e any difference between the DX9 coarse value and the newer 4op ratio value
	freqFine = newFreqVal - oldFreqVal;
	if (freqFine > 0.0)
	{
		//there is a difference, so work out what that is in the available DX9 fine steps		
		//work out the number of steps
		fineWork = freqFine / fineStep;
		fine = (UCHAR)(int)fineWork;
		calcFreq = oldFreqVal + (fineStep*(double)fine);
#ifdef _DX9_DEBUG
		swprintf(genOut, BUFSIZE, TEXT("fine delta:%f , step %f, steps %d , calc %f"), freqFine, fineStep, fine, calcFreq);
		ConsoleOut(genOut);
#endif
	}
	//set the coarse (0-31);
	coarse &= 0x1f;
	*pCoarseOut = coarse;
	//then the fine (0-99)
	*pFineOut = fine;
	//detune
	//DX9 Detune breaks the current fine step into 15 smaller steps, centered around 7
	detuneStep = fineStep / 15.0F;
	//Later 4op detune does +/- 2 cents in 7 steps, centered around 3
	//interestingly 
	//The DX21 manual says it's 2 cents and the values are -7 to +7, though the bulk data format at least (VMEM) only allows for -3 to +3
	//DX100, 27,11 and tx81z all agree it's 2.6 cents -3 to +3
	//comething like:
	//    0       1        2     3     4     5       6
	//  -2.6    -1.73    -0.87   0    0.87  1.73    2.6
	//
	//we're doing a simple translation of the detune - just moving the centre to 7 rather than 3
	//
	newDetune = lpOperatorNew->RateScalingDetune & 7;
	/*
	Here's some maths to translate the cents into ratios - a WIP so sticking with the simple 'recentre' approach for now
	maxCents = 2.6F;
	centStep = maxCents / 3.0f;
	centWork = ((float)(newDetune-3.0f))*centStep;
	//form.ratioout.value = Math.round(1000000 * Math.pow(2, (centin / 100 / 12))) / 1000000;

	centRatio = ((1000000.0f * pow(2, (centWork/100.0f/12.0f))) / 1000000.0f)-1.0f;

	oldDetune = (UCHAR)(centRatio / detuneStep);
	printf("\n in det %d cent %f , rat %f, detStep %f, oDet %d", newDetune,centWork, centRatio,detuneStep,oldDetune);
	*/
	oldDetune = DETUNECONVERT[newDetune];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT(" Det in:%d out%d \r\n"), newDetune, oldDetune);
	ConsoleOut(genOut);
#endif	
	*pDetuneOut = oldDetune;	
	return;
}

//convert the frequency settings of the operator
void FreqConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld)
{
	UCHAR coarse;
	UCHAR fine;
	UCHAR detune;
//calculate/convert the coarse, fine and detune settings for this 'new style' operator
	CalcFreqDetune(lpOperatorNew, &coarse, &fine, &detune);	
	
//then set those into the DX9 operator
	lpOperatorOld->FreqCMode = (coarse << 1);
	lpOperatorOld->FreqFine = fine;
	lpOperatorOld->OscDetuneRateScale = detune << 3;	
	return;
}


//convert from 'new' Envelopes to old ones
void EGConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld)
{

/*
EG notes
Converting from the later 4op 'Enhanced ADSR' to the older (and more flexible) DX9 4 levels and rates
L1 is 99
R1 is Attack Rate

L2 is Decay 1 Level
R2 is Decay 1 Rate

If Decay 2 Rate is 0
  L3 is Decay 1 Level (sustain)
else
  L3 is 0
R3 is Decay 2 rate

L4 is 0
R4 is release rate
*/

//level 1 is fixed at 99
	lpOperatorOld->Level1 = 99;
//level 2 is Decay1Level
	lpOperatorOld->Level2 = SustainLevel[lpOperatorNew->Decay1Level];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("L1:%d , L2:%d(%d) , "), lpOperatorOld->Level1, lpOperatorOld->Level2, lpOperatorNew->Decay1Level);
	ConsoleOut(genOut);
#endif
//level 3 is the sustain level if D2R is 0
	if (lpOperatorNew->DecayRate2 == 0)
		lpOperatorOld->Level3 = SustainLevel[lpOperatorNew->Decay1Level];
	else
//otherwise level 3 is 0
		lpOperatorOld->Level3 = 0;
//level 4 is fixed - 0
	lpOperatorOld->Level4 = 0;
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("L3:%d , L4:%d , "), lpOperatorOld->Level3, lpOperatorOld->Level4);
	ConsoleOut(genOut);
#endif
//now the rates
//convert the attack rate
	lpOperatorOld->Rate1 = AttackRate[lpOperatorNew->AttackRate];
//the 2 decay rates
	lpOperatorOld->Rate2 = DecayRate[lpOperatorNew->DecayRate1];
	lpOperatorOld->Rate3 = DecayRate[lpOperatorNew->DecayRate2];
//then the release rate
	lpOperatorOld->Rate4 = ReleaseRate[lpOperatorNew->ReleaseRate];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("R1:%d(%d) , R2:%d(%d) ,  R3:%d(%d) , R4:%d(%d) "), lpOperatorOld->Rate1, lpOperatorNew->AttackRate,
		lpOperatorOld->Rate2, lpOperatorNew->DecayRate1, lpOperatorOld->Rate3, lpOperatorNew->DecayRate2, lpOperatorOld->Rate4, lpOperatorNew->ReleaseRate);				
	ConsoleOut(genOut);
#endif
	return;
}

//convert algorithm
void AlgorithmConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch)
{
//for compound values
	UCHAR outByte;

	//algorithm is in bottom 3 bits
	lpOldPatch->Algorithm.Algorithm = MAPALGO[(lpNewPatch->LFOParams.SyncFBackAlgo & 0x07)];
	//get the key sync (value in bit 6 move to bit 3)
	outByte = (lpNewPatch->LFOParams.SyncFBackAlgo & 0x40) >> 3;
	//add the feedback (value in bits 5,4,3 move to 2,1,0)
	outByte |= ((lpNewPatch->LFOParams.SyncFBackAlgo & 0x38) >> 3);
	lpOldPatch->Algorithm.OscKeySyncFeedback = outByte;
}

//convert LFO params
void LFOConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch)
{
//for compound values
	UCHAR outByte;
	UCHAR workByte;
//some simple values	
	lpOldPatch->Lfo.LFOSpeed = lpNewPatch->LFOParams.LFOSpeed;
	lpOldPatch->Lfo.LFODelay = lpNewPatch->LFOParams.LFODelay;
	lpOldPatch->Lfo.Transpose = lpNewPatch->PerfParameters.Transpose;

//Amp and pitch mod depth
	lpOldPatch->Lfo.LFOAmpModDepth = min(99,lpNewPatch->PerfParameters.AModDepth+17);
	lpOldPatch->Lfo.LFOPitchModDepth = lpNewPatch->PerfParameters.PModDepth;

//PmsAmsLFOWave breakdown
//b6 5 4 - Pitch Modulation Sensitivity (0-7)
//b3 2 - Amplitude Modulation Sensitivity (0-3)
//b1 0 - LFO Wave (0-3)
//get the LFO wave
	workByte = lpNewPatch->PerfParameters.PmsAmsLFOWave & 0x03;
//no LFO sync on DX9 - so bit 0 is always iff and we can start with the wave in bits 3 2 1
	outByte = LFOCONVERT[workByte]<<1;	
	//get pitch mod sensitivity - bits 6 5 4 in both 'old' and 'new' structures
	outByte |= (lpNewPatch->PerfParameters.PmsAmsLFOWave & 0x70);

	//breakdown LFOPModWaveSync;
	//b6 5 4 - LFO Pitch Mod Sensitivity (0-7)
	//b3 2 1 - LFO Wave (0-5)
	//b0	 - Sync (0-1)
	lpOldPatch->Lfo.LFOPModWaveSync = outByte;
	
	return;
}

//default out all the unused parameters in the patch
void SetDefaults(lpFM_BULK_OLD_PATCH lpOldPatch,int PatchNo)
{
	char dispName[16];
	
//set the 2 unnused operators in the DX9 structure to 0's
	memset(&lpOldPatch->FMOp[4], 0, sizeof(FM_BULK_OPERATOR_OLD)*2);

//pitch envelope (fixed for DX9)
	lpOldPatch->PitchEnvelope.Level1 = PEG_LEVEL;
	lpOldPatch->PitchEnvelope.Level2 = PEG_LEVEL;
	lpOldPatch->PitchEnvelope.Level3 = PEG_LEVEL;
	lpOldPatch->PitchEnvelope.Level4 = PEG_LEVEL;
	lpOldPatch->PitchEnvelope.Rate1 = PEG_RATE;
	lpOldPatch->PitchEnvelope.Rate2 = PEG_RATE;
	lpOldPatch->PitchEnvelope.Rate3 = PEG_RATE;
	lpOldPatch->PitchEnvelope.Rate4 = PEG_RATE;
//set the DX9 name, 
//be lazy and pad more than the length of the string, then just copy over the first 10 chars so we don't get the NULL
	sprintf_s(dispName,16, "DX9.%*d     ", 2,PatchNo+1);
	memcpy(lpOldPatch->PatchName, dispName, 10);
	return;
}




//set the check byte
void SetCheckSum(UCHAR *writeBuffer,BOOL display)
{
	UCHAR work = 0;
	UCHAR *workPointer = &writeBuffer[SYSDATAPOS];
//total of all data bytes
	for (int i = 0; i < SYSDATABYTES; i++,workPointer++)
		work += *workPointer;		
//generate the 2's complement checksum midi style (no bit 7)
	work= (~work + 1)&0x7f;
//either display it (largely for debug purposes)
	if (display)
	{
		swprintf(genOut, BUFSIZE, TEXT("Check Sum is 0x%02X\r\n"), work);
		ConsoleOut(genOut);
	}	
//or wite it into the output buffer
	else	
		writeBuffer[SYSCHKSUM] = work;
	
	return;
}

//Convert the new patch to 'old' style DX9 patch
BOOL ConvertVoice(lpFM_BULK_NEW_PATCH lpNewPatch,int PatchNo,lpFM_BULK_OLD_PATCH lpOldPatch)
{
	char dispName[12];
//Get the voice name
	dispName[10] = 0;
	memcpy(dispName, lpNewPatch->PatchName, 10);	
	swprintf(genOut, BUFSIZE, TEXT("Converting Voice (%d) : %S\r\n"), (PatchNo + 1), dispName);
	ConsoleOut(genOut);

//Now convert to the DX9
//set the easy stuff
	SetDefaults(lpOldPatch,PatchNo);
//Algorithm
	AlgorithmConvert(lpNewPatch, lpOldPatch);
//convert the operators
	OperatorConvert(lpNewPatch, lpOldPatch);
//LFO
	LFOConvert(lpNewPatch, lpOldPatch);
	return TRUE;
}

//write out the DX9 20 voice dump structure
BOOL WriteDX9BulkFile(LPCWSTR fileName, UCHAR *outBuffer)
{
	HANDLE hFile;
	DWORD bytesWritten;
	BOOL retValue = TRUE;

	hFile = CreateFile(fileName,                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create file always, even if it already exists
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		swprintf(genOut, BUFSIZE, TEXT("Unable to create output file %s\r\n"), fileName);
		ConsoleOut(genOut);
		return FALSE;
	}

	if (!WriteFile(hFile, outBuffer, SYSLEN, &bytesWritten, NULL))
	{
		swprintf(genOut, BUFSIZE, TEXT("Failed to write to output file %s\r\n"), fileName);
		ConsoleOut(genOut);
		retValue = FALSE;
	}
	CloseHandle(hFile);

	return retValue;
}

//write out the DX9 20 voice dump structure
BOOL WriteDX9PatchNames(LPCWSTR fileName, LPCSTR outBuffer)
{
	HANDLE hFile;
	DWORD bytesWritten;
	BOOL retValue = TRUE;
	
	int fPos = 0;
	int xx = (int)_tcslen(fileName);
	for (fPos = xx; fPos > 0; fPos--)
		if (fileName[fPos] == '\\')
			break;

	hFile = CreateFile(fileName,                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create file always, even if it already exists
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		swprintf(genOut, BUFSIZE, TEXT("**** Unable to create output file %ls\r\n"), fileName);		
		ConsoleOut(genOut);
		return FALSE;
	}


	if (!WriteFile(hFile, outBuffer, (DWORD)strlen((char *)outBuffer), &bytesWritten, NULL))
	{
		swprintf(genOut, BUFSIZE, TEXT("**** Failed to write to output file %ls\r\n"), fileName);
		ConsoleOut(genOut);
		retValue = FALSE;
	}
	CloseHandle(hFile);

	return retValue;
}

void getPatchName(char *outName, char *patchName)
{
	int i;
	int x;
//copy across the filename
	return;
	strcpy_s(patchName,1024, outName);
	return;
//read backwards till we find a '.'

	x = (int)strlen(patchName);
	for (i = x; i >=0; i--)
	{
		if (patchName[i] == '.')
		{
			patchName[i] = 0;
			strcat_s(patchName, 1024,".txt");
		}
	}
	swprintf(genOut, BUFSIZE, TEXT("Get patch name %S -> %S\r\n"), outName, patchName);
	ConsoleOut(genOut);
}

void getStrName(lpFM_BULK_NEW_PATCH lpNewPatch,char *dispName)
{
	dispName[10] = 0;
	strncpy_s(dispName, 16, (char *)lpNewPatch->PatchName,10);
}
	

//convert bulk data dump
void ConvertBulkVoices(HANDLE hFile,UCHAR *readBuffer,UCHAR *writeBuffer, int midiChannel)
{
	TCHAR tWork[BUFSIZE];
	TCHAR tPath[BUFSIZE];
	TCHAR tTxtFile[BUFSIZE];
	TCHAR tTxtName[BUFSIZE];
	TCHAR tOutName[BUFSIZE];
	
	char textBuffer[BUFSIZE];
	char workBuffer[BUFSIZE];
	char dispName[16];

	DWORD inBufferPos = 0;
	DWORD outBufferPos = 0;
	int pass, xx, y, byteLen;
	DWORD dwRet;

	lpFM_BULK_NEW_PATCH lpNewPatch;
	FM_BULK_OLD_PATCH oldPatch;

//32 voice bulk dump in 'new' 4op format (DX21/27/100/11)
//go through the file twice, creating 2 DX9 files
//32 patches -> 2x20 patches with 8 patches repeated
	for (pass = 0; pass < 2; pass++)
	{
		dwRet = GetFinalPathNameByHandle(hFile, tWork, BUFSIZE, VOLUME_NAME_DOS);

		//add the prefix
		xx = (int)_tcslen(tWork);
		for (y = xx; y > 0; y--)
		{
			if (tWork[y] == '.')
				tWork[y] = 0;
			if (tWork[y] == '\\')
				break;
		}
		_tcsncpy_s(tPath, tWork, y + 1);

		swprintf(tTxtFile, BUFSIZE, TEXT("DX9_%s_%d"), &tWork[y + 1], pass + 1);
		swprintf(tOutName, BUFSIZE, TEXT("%s%s.syx"), tPath, tTxtFile);
		swprintf(tTxtName, BUFSIZE, TEXT("%s%s.txt"), tPath, tTxtFile);
//create UTF-8 version of name for text file		
		byteLen = WideCharToMultiByte(CP_UTF8, 0, tTxtFile, (int)_tcslen(tTxtFile), workBuffer, BUFSIZE, NULL, NULL);
		textBuffer[0] = 0;
		if (byteLen)
		{
			workBuffer[byteLen] = 0;
			sprintf_s(textBuffer, BUFSIZE, "-------------\r\n  Sysex File : %s.syx\r\n  Patch File : %s.txt\r\n-------------\r\n", workBuffer, workBuffer);
		}

//text buffer containing patch list
		strcat_s(textBuffer, BUFSIZE, "\r\n     Patches\r\n=================\r\n");

		ConsoleOut(L"\r\n");
		inBufferPos = 6 + (FMDX9_SKIP_PATCH_SIZE * pass);

//write out the header
		memcpy(writeBuffer, FMDX9_BULKHEADER, 6);
		outBufferPos = 6;
//set MIDI channel
		writeBuffer[MIDI_CHANNEL_POS] = (midiChannel - 1);
//'new' (non-DX9) format, so convert
//loop through the patches in the file	
		for (int i = 0; i < 20; i++, inBufferPos += 128, outBufferPos += 128)
		{
//save the patch name into our text buffer
			lpNewPatch = (lpFM_BULK_NEW_PATCH)&readBuffer[inBufferPos];
			getStrName(lpNewPatch, dispName);
			sprintf_s(workBuffer, BUFSIZE, "%02d - %s\r\n", (i + 1), dispName);
			strcat_s(textBuffer, BUFSIZE, workBuffer);
			//convert the patch
			ConvertVoice(lpNewPatch, i, &oldPatch);
			//write the patch out
			memcpy(&writeBuffer[outBufferPos], &oldPatch, sizeof(FM_BULK_OLD_PATCH));
		}
		//fill out the remainder of the file
		memset(&writeBuffer[outBufferPos], DX9FILLER, (sizeof(FM_BULK_OLD_PATCH)*FILLERPATCHES));
		//add the checksum
		SetCheckSum(writeBuffer, FALSE);
		//add EOF
		writeBuffer[SYSEOF] = SYSEND;
		//write out the file
		swprintf(tWork, BUFSIZE, TEXT("DX9_%s_%d.syx"), &tWork[y + 1], pass + 1);
		ConsoleOut(L"Writing Files :\r\n\t");
		ConsoleOut(tWork);
		ConsoleOut(L"\r\n\t");
		swprintf(tWork, BUFSIZE, TEXT("DX9_%s_%d.txt"), &tWork[y + 1], pass + 1);
		ConsoleOut(tWork);
		ConsoleOut(L"\r\n");
		ConsoleOut(L"\r\n======================================================\r\n");
		WriteDX9BulkFile(tOutName, writeBuffer);
		WriteDX9PatchNames(tTxtName, textBuffer);
	}
}