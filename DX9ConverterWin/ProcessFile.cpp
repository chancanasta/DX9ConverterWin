//where most of the file processing happens
#include "stdafx.h"
#include "DX9ConverterWin.h"
#include "FileConstants.h"
#include "SysExConstants.h"
#include "FMStructs.h"
#include "FMConversion.h"
#include "ProcessFile.h"
#include "DXDisplay.h"

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
		return FALSE;
	}

	readBuffer = (UCHAR *)phInputBuffer;
	writeBuffer = (UCHAR *)phOutputBuffer;
//check it's a valid SYSEX file
	if (!ValidDXSysEx(readBuffer, dwBytesRead))
		return FALSE;
		
//figure out which format it is
	dumpFormat = GetDumpFormat(readBuffer);
	
	switch (dumpFormat)
	{
		case SINGLE_VOICE:
			ConsoleOut(L"Single voice data dump\r\n");
			ConvertSingleVoice(hFile, readBuffer, writeBuffer, midiChannel);
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
		case ADDITIONAL_VOICE:
			ConsoleOut(L"2GEN Single voice (DX11/TX81Z)\r\n");
			ConvertAdditionalVoice(hFile, readBuffer, writeBuffer, midiChannel, dwBytesRead);
			break;
		default:
			ConsoleOut(L"Invalid Voice Dump format\r\n");
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
		case 0x7e:
			return ADDITIONAL_VOICE;

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
		swprintf(genOut, BUFSIZE, TEXT("SysEx MIDI Channel value (%d)\r\n"), (int)readBuffer[2] + 1);
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

void SingleOperatorConvert(lpFM_SINGLE_NEW_PATCH lpNewPatch, lpFM_SINGLE_OLD_PATCH lpOldPatch,lpFM_SINGLE_ADD_PARAMS lpFMAdditional)
{
	lpFM_SINGLE_OPERATOR_NEW lpNewOperator;
	lpFM_SINGLE_OPERATOR_OLD lpOldOperator;
	lpFM_SINGLE_ADD_OP	lpAddOperator = NULL;

	UCHAR algo;
	FM_DX9_CARRIERS OpsMap;
	

	/*
	loop through the 4 operators
	Note that the operators oddly and helpfully go 4,2,3,1
	*/

	//get the algorithm, as we may need to remap the operators (algorithm 3 (7))
	algo = lpNewPatch->Algorithm.Algorithm;
	//get the map
	OpsMap = OPERATORMAP[algo];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("Algo :%d \r\n"), algo);
	ConsoleOut(genOut);
#endif
	for (int i = 0; i < 4; i++)
	{

		//get pointers to the operators
		int mappedOp = OpsMap.Operators[i];
		lpNewOperator = &lpNewPatch->FMOp[mappedOp];
		if (lpFMAdditional)
			lpAddOperator = &lpFMAdditional->FMOp[mappedOp];

		lpOldOperator = &lpOldPatch->FMOp[i];
			
		//Envlopes
#ifdef _DX9_DEBUG
		swprintf(genOut, BUFSIZE, TEXT("Ops i -%d from %d, EG %d - "), i, OpsMap.Operators[i], (i + 1));
		ConsoleOut(genOut);
#endif
		EGConvert(&lpNewOperator->EG, &lpOldOperator->EG);
		//Frequency
		SingleFreqConvert(lpNewOperator, lpOldOperator,lpAddOperator);
		//General Operator values
		//output level
		lpOldOperator->OutputLevel = FM_DX9_OUTLEVEL[lpNewOperator->OutputLevel];
		//DX9 has simple keyboard scaling compared to DX7, just rate and level
		lpOldOperator->KeyRateScaling = SCALINGRATECONVERT[lpNewOperator->KeyScalingRate];
		//scaling level
		//DX9 uses the ScaleRightDepth parameter for level scaling
		lpOldOperator->ScaleRightDepth = lpNewOperator->KeyScalingLevel;
			
		//default the other scaling parameters
		lpOldOperator->BreakPoint = 0x0F;
		lpOldOperator->ScaleLeftDepth = 0x00;
		lpOldOperator->ScaleRightCurve = 0x01;
		lpOldOperator->ScaleLeftCurve = 0x00;
		//key velocity is always zero,
		lpOldOperator->KeyVelSens = 0;
		
		//amplitute mod sensitvity
		int amod = lpNewPatch->LFOParams.AModSens;
		//Amplitute mod enable is on/off per operator, so get flag		
		//then set amp mod for this operator		
		lpOldOperator->AmpModSens = amod*lpNewOperator->AmpModEnable;;			
	}

}

void OperatorConvert(lpFM_BULK_NEW_PATCH lpNewPatch, lpFM_BULK_OLD_PATCH lpOldPatch)
{
	lpFM_BULK_OPERATOR_NEW lpNewOperator;
	lpFM_BULK_OPERATOR_OLD lpOldOperator;
	lpFM_BULK_ADD_OPERATOR lpAddOperator;
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
		int mappedOp = OpsMap.Operators[i];
//get pointers to the operators
		lpNewOperator = &lpNewPatch->FMOp[mappedOp];
		lpAddOperator = &lpNewPatch->AddOp[mappedOp];
		lpOldOperator = &lpOldPatch->FMOp[i];
		
//Envlopes
#ifdef _DX9_DEBUG
		swprintf(genOut, BUFSIZE, TEXT("Ops i -%d from %d, EG %d - "), i, OpsMap.Operators[i], (i + 1));
		ConsoleOut(genOut);
#endif
		EGConvert(&lpNewOperator->EG, &lpOldOperator->EG);
//Frequency
		FreqConvert(lpNewOperator, lpOldOperator,lpAddOperator);
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
void CalcFreqDetune(UCHAR freqIn,UCHAR fineIn,UCHAR detuneIn, UCHAR *pCoarseOut, UCHAR *pFineOut, UCHAR *pDetuneOut)
{
	UCHAR coarse = 0;
	UCHAR fine = 0;
	UCHAR work = 0;
	UCHAR oldDetune;
	double newFreqVal;
	double oldFreqVal;
	double freqFine;
	double fineStep;
	double detuneStep;
	double fineWork;
	double calcFreq;
	int i;
// the frequency (0-63) from new 4 op
//get a double that represents that ratio
//	newFreqVal = DX21FREQ[freqIn];
//support 2gen DX11 / TX81Z
	fineIn &= 15;
	freqIn &= 63;
	newFreqVal = TX81FREQ[freqIn][fineIn];
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
//	newDetune = lpOperatorNew->RateScalingDetune & 7;
//	newFreq = lpOperatorNew->OSCFreq;	
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
	oldDetune = DETUNECONVERT[detuneIn];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT(" Det in:%d out%d \r\n"), detuneIn, oldDetune);
	ConsoleOut(genOut);
#endif	
	*pDetuneOut = oldDetune;	
	return;
}

//convert the frequency settings of the operator
void FreqConvert(lpFM_BULK_OPERATOR_NEW lpOperatorNew, lpFM_BULK_OPERATOR_OLD lpOperatorOld, lpFM_BULK_ADD_OPERATOR lpAddOperator)
{
	UCHAR coarse;
	UCHAR fine;
	UCHAR detune;
	UCHAR inFreq = lpOperatorNew->OSCFreq;
	UCHAR inDetune = lpOperatorNew->RateScalingDetune & 7;
//fine - padded to 0 in DX21/27/100 sysex files, part of additional parameters for DX11/TX81Z
	UCHAR inFine = lpAddOperator->WaveFine & 0x0f;
//calculate/convert the coarse, fine and detune settings for this 'new style' operator
	CalcFreqDetune(inFreq,inFine,inDetune,&coarse, &fine, &detune);
//then set those into the DX9 operator
	lpOperatorOld->FreqCMode = (coarse << 1);
	lpOperatorOld->FreqFine = fine;
	lpOperatorOld->OscDetuneRateScale = detune << 3;	
	return;
}

//convert the frequency settings of the operator
void SingleFreqConvert(lpFM_SINGLE_OPERATOR_NEW lpOperatorNew, lpFM_SINGLE_OPERATOR_OLD lpOperatorOld, lpFM_SINGLE_ADD_OP lpFMAddOperator)
{
	UCHAR coarse;
	UCHAR fine;
	UCHAR detune;
	UCHAR inFreq = lpOperatorNew->OSCFreq;
	UCHAR inDetune = lpOperatorNew->Detune;
	UCHAR inFine = 0;

	if (lpFMAddOperator)
		inFine = lpFMAddOperator->FreqRangeFine;

	//calculate/convert the coarse, fine and detune settings for this 'new style' operator
	CalcFreqDetune(inFreq, inFine, inDetune, &coarse, &fine, &detune);
	//then set those into the DX9 operator
	lpOperatorOld->FreqCoarse = coarse;
	lpOperatorOld->FreqFine = fine;
	lpOperatorOld->OscMode = 1;
	lpOperatorOld->OscDetune = detune;
	return;
}

//convert from 'new' Envelopes to old ones
void EGConvert(lpFM_EG_NEW lpEgNew, lpFM_EG_OLD lpEgOld)
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
	lpEgOld->Level1= 99;
//level 2 is Decay1Level
	lpEgOld->Level2 = SustainLevel[lpEgNew->Decay1Level];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("L1:%d , L2:%d(%d) , "), lpEgOld->Level1, lpEgOld->Level2, lpEgNew->Decay1Level);
	ConsoleOut(genOut);
#endif
//level 3 is the sustain level if D2R is 0
	if (lpEgNew->DecayRate2 == 0)
		lpEgOld->Level3 = SustainLevel[lpEgNew->Decay1Level];
	else
//otherwise level 3 is 0
		lpEgOld->Level3 = 0;
//level 4 is fixed - 0
	lpEgOld->Level4 = 0;
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("L3:%d , L4:%d , "), lpEgOld->Level3, lpOEgOld->Level4);
	ConsoleOut(genOut);
#endif
//now the rates
//convert the attack rate
	lpEgOld->Rate1 = AttackRate[lpEgNew->AttackRate];
//the 2 decay rates
	lpEgOld->Rate2 = DecayRate[lpEgNew->DecayRate1];
	lpEgOld->Rate3 = DecayRate[lpEgNew->DecayRate2];
//then the release rate
	lpEgOld->Rate4 = ReleaseRate[lpEgNew->ReleaseRate];
#ifdef _DX9_DEBUG
	swprintf(genOut, BUFSIZE, TEXT("R1:%d(%d) , R2:%d(%d) ,  R3:%d(%d) , R4:%d(%d) "), lpEgOld->Rate1, lpEgNew->AttackRate,
		lpEgOld->Rate2, lpEgNew->DecayRate1, lpEgOld->Rate3, lpEgNew->DecayRate2, lpEgOld->Rate4, lpEgNew->ReleaseRate);				
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
//there is an LFO sync in the 'new' 4ops, which is not on the DX9
//also there is a OSC sync on the DX9 which is not present in the newer 4ops
//so default the OSC sync to on or off ?
	outByte = OSC_BULK_SYNC_ON;
	//add the feedback (value in bits 5,4,3 move to 2,1,0)
	outByte |= ((lpNewPatch->LFOParams.SyncFBackAlgo & 0x38) >> 3);
	lpOldPatch->Algorithm.OscKeySyncFeedback = outByte;
}

void SingleAlgorithmConvert(lpFM_SINGLE_NEW_PATCH lpNewPatch, lpFM_SINGLE_OLD_PATCH lpOldPatch)
{
	lpOldPatch->Alogrithm.Algorithm = MAPALGO[lpNewPatch->Algorithm.Algorithm];
	lpOldPatch->Alogrithm.Feedback = lpNewPatch->Algorithm.Feedback;	
	lpOldPatch->Alogrithm.OscSync = 1;
}

/*
New 4op LFOs =
TX81z
Speed:
00.007Hz   (150 seconds for 1 cycle)
50.000Hz   (50 cycles a second)

Delay:
0  seconds
15 seconds

DX21
Speed
00.06Hz (16 seconds for 1 cycle)
50.00Hz (50 cycles a second)

Delay:
0  seconds
15 seconds

DX9
00.06Hz (16 seconds for 1 cycle)
47.2Hz  (47 cycles a second)

->LFO speed maps closely to DX21/27/100 but var significantly at lower settings for TX81z/DX11
*/

void SingleLFOConvert(lpFM_SINGLE_NEW_PATCH lpNewPatch, lpFM_SINGLE_OLD_PATCH lpOldPatch)
{
	lpOldPatch->Lfo.LFOSpeed = lpNewPatch->LFOParams.LFOSpeed;
	lpOldPatch->Lfo.LFODelay = lpNewPatch->LFOParams.LFODelay;
	lpOldPatch->Lfo.Transpose = lpNewPatch->LFOParams.Transpose;
	lpOldPatch->Lfo.LFOAmpModDepth = lpNewPatch->LFOParams.AModDepth;
	lpOldPatch->Lfo.LFOPitchModDepth = lpNewPatch->LFOParams.PModDepth;
	lpOldPatch->Lfo.LFOSync = 0;
	lpOldPatch->Lfo.WaveForm = LFOCONVERT[lpNewPatch->LFOParams.LFOWave];
	lpOldPatch->Lfo.PitchModSens = lpNewPatch->LFOParams.PModSens;
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
	lpOldPatch->Lfo.LFOAmpModDepth = lpNewPatch->PerfParameters.AModDepth;
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
void SetDefaults(lpFM_BULK_OLD_PATCH lpOldPatch,int PatchNo,char *dispName,BOOL oldName)
{
	
	
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
	if(oldName)
		sprintf_s(dispName,12, "DX9.%*d     ", 2,PatchNo+1);
	memcpy(lpOldPatch->PatchName, dispName, 10);
	return;
}

//default out all the unused parameters in the patch
void SetSingleDefaults(lpFM_SINGLE_OLD_PATCH lpOldPatch)
{
	char dispName[16];

	//set the 2 unnused operators in the DX9 structure to 0's
	memset(&lpOldPatch->FMOp[4], 0, sizeof(FM_SINGLE_OPERATOR_OLD) * 2);

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
	sprintf_s(dispName, 16, "DX9.%*d     ", 2, 1);
	memcpy(lpOldPatch->PatchName, dispName, 10);
	return;
}





UCHAR CalcChecksum(UCHAR *writeBuffer, int startPos, int dataLen)
{
	UCHAR work = 0;
	UCHAR *workPointer = &writeBuffer[startPos];
	//total of all data bytes
	for (int i = 0; i < dataLen; i++, workPointer++)
		work += *workPointer;
	//generate the 2's complement checksum midi style (no bit 7)
	work = (~work + 1) & 0x7f;
	return work;
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
	SetDefaults(lpOldPatch,PatchNo,dispName,TRUE);
//Algorithm
	AlgorithmConvert(lpNewPatch, lpOldPatch);
//convert the operators
	OperatorConvert(lpNewPatch, lpOldPatch);
//LFO
	LFOConvert(lpNewPatch, lpOldPatch);
	return TRUE;
}

//write out the DX9 voice dump structure
BOOL WriteDX9BulkFile(LPCWSTR fileName, UCHAR *outBuffer,int outLen)
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

	if (!WriteFile(hFile, outBuffer, outLen, &bytesWritten, NULL))
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
	

void getStrSingleName(lpFM_SINGLE_NEW_PATCH lpNewPatch, char *dispName)
{
	dispName[10] = 0;
	strncpy_s(dispName, 16, (char *)lpNewPatch->PatchName, 10);
}

//convert 2gen voice - (has additional params)
void ConvertAdditionalVoice(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer, int midiChannel, DWORD dwBytesRead)
{
	DWORD inBufferPos = 0;
	inBufferPos = 6;
	WORD dumpFormat;
	int chkLen = sizeof(FM_SINGLE_ADD_HEADER);

	lpFM_SINGLE_ADD_PARAMS lpFMAdditional;
	//check this is not just the additional dump without the main voice
	if (dwBytesRead < FM_2GEN_SINGLE_SIZE)
	{
		swprintf(genOut, BUFSIZE, TEXT("**** File size (%d) is too small for 2Gen Additional + Standard Voice Params\r\n"), dwBytesRead);
		ConsoleOut(genOut);
		return;
	}
	//read in the additional voice details
//check the header
	if (memcmp(&readBuffer[inBufferPos], FM_SINGLE_ADD_HEADER, chkLen))
	{
		ConsoleOut(L"***** 2Gen voice has incorrect text header\r\n");
		return;
	}
	
	inBufferPos += chkLen;
	lpFMAdditional = (lpFM_SINGLE_ADD_PARAMS)&readBuffer[inBufferPos];
	//move onto the voice params
	readBuffer += FM_2GEN_ADD_SIZE;
	//and check the format again
	if (ValidDXSysEx(readBuffer, (dwBytesRead - FM_2GEN_ADD_SIZE)))
	{
		dumpFormat = GetDumpFormat(readBuffer);

		if (dumpFormat == SINGLE_VOICE)
		{
			ConsoleOut(L"Single voice data found in 2Gen dump - converting\r\n");
			ConvertSingleVoice(hFile, readBuffer, writeBuffer, midiChannel, lpFMAdditional);
		}
		else
			//anything is an error
			ConsoleOut(L"***** Format of 2Gen dump is incorrect, aborting \r\n");
	}
	else
		ConsoleOut(L"**** Invalid SYSEX Format in 2Gen dump , aborting \r\n");

}

//convert single voice data dump
void ConvertSingleVoice(HANDLE hFile, UCHAR *readBuffer, UCHAR *writeBuffer, int midiChannel, lpFM_SINGLE_ADD_PARAMS lpFMAdditional)
{
	TCHAR tWork[BUFSIZE];
	TCHAR tPath[BUFSIZE];
	TCHAR tTxtFile[BUFSIZE];
	TCHAR tTxtName[BUFSIZE];
	TCHAR tOutName[BUFSIZE];

	char textBuffer[BUFSIZE];
	char workBuffer[BUFSIZE];
	char dispName[16];

	DWORD dwRet;
	DWORD inBufferPos = 0;
	DWORD outBufferPos = 0;
	lpFM_SINGLE_NEW_PATCH lpNewPatch;
	FM_SINGLE_OLD_PATCH oldPatch;
	int xx, y, byteLen;
	inBufferPos = 6;

	lpNewPatch = (lpFM_SINGLE_NEW_PATCH)&readBuffer[inBufferPos];
	getStrSingleName(lpNewPatch, dispName);

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

	swprintf(tTxtFile, BUFSIZE, TEXT("DX9_%s"), &tWork[y + 1]);
	swprintf(tOutName, BUFSIZE, TEXT("%s%s.syx"), tPath, tTxtFile);
	swprintf(tTxtName, BUFSIZE, TEXT("%s%s.txt"), tPath, tTxtFile);
	//create UTF-8 version of name for text file		
	byteLen = WideCharToMultiByte(CP_UTF8, 0, tTxtFile, (int)_tcslen(tTxtFile), workBuffer, BUFSIZE, NULL, NULL);
	textBuffer[0] = 0;
	if (byteLen)
	{
		workBuffer[byteLen] = 0;
		sprintf_s(textBuffer, BUFSIZE, "-------------\r\n  Sysex File : %s.syx\r\n  Patch File : %s.txt\r\n-------------\r\n Patch Name: %s\r\n", workBuffer, workBuffer,dispName);
	}
	

	swprintf(genOut, BUFSIZE, TEXT("Converting Voice : %S\r\n"),  dispName);
	ConsoleOut(genOut);

//Now convert to the DX9
//set the easy stuff
	SetSingleDefaults(&oldPatch);
//Algorithm
	SingleAlgorithmConvert(lpNewPatch, &oldPatch);
	//convert the operators
	SingleOperatorConvert(lpNewPatch, &oldPatch,lpFMAdditional);
//LFO
	SingleLFOConvert(lpNewPatch, &oldPatch);
//store the header
	memcpy(writeBuffer, FMDX9_SINGLE_HEADER, 6);
	outBufferPos = 6;
	//set MIDI channel
	writeBuffer[MIDI_CHANNEL_POS] = (midiChannel - 1);
//then the voice data
	memcpy(&writeBuffer[outBufferPos], &oldPatch, sizeof(oldPatch));outBufferPos += (sizeof(oldPatch)+1);
//set the checksum
	writeBuffer[SYSCHKSUM_SINGLE] = CalcChecksum(writeBuffer, 6, 155);
//then EOF	
	writeBuffer[outBufferPos] = SYSEND;
//write out the 2 files
	swprintf(tWork, BUFSIZE, TEXT("DX9_%s.syx"), &tWork[y + 1]);
	ConsoleOut(L"Writing Files :\r\n\t");
	ConsoleOut(tWork);
	ConsoleOut(L"\r\n\t");
	swprintf(tWork, BUFSIZE, TEXT("DX9_%s.txt"), &tWork[y + 1]);
	ConsoleOut(tWork);
	ConsoleOut(L"\r\n");
	ConsoleOut(L"\r\n======================================================\r\n");


	WriteDX9BulkFile(tOutName, writeBuffer, SYSLEN_SINGLE);
	WriteDX9PatchNames(tTxtName, textBuffer);
	return;

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
		memcpy(writeBuffer, FMDX9_BULK_HEADER, 6);
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
		writeBuffer[SYSCHKSUM] = CalcChecksum(writeBuffer, SYSDATAPOS, SYSDATABYTES);		
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
		WriteDX9BulkFile(tOutName, writeBuffer,SYSLEN);
		WriteDX9PatchNames(tTxtName, textBuffer);
	}
}