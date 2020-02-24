#ifndef __FMSTRUCTS_H
#define __FMSTRUCTS_H

//uncomment to add debug output
//#define _DX9_DEBUG

#define FMDX9_PATCH_DATA_SIZE	128
#define FMDX9_ALL_PATCH_SIZE	20*FMDX9_PATCH_DATA_SIZE
#define FMDX9_SKIP_PATCH_SIZE	12*FMDX9_PATCH_DATA_SIZE

static UCHAR FMDX9_BULKHEADER[6] = { 0xf0,0x43,0x00,0x09,0x020,00 };
static UCHAR FMDX9_BULKFOOTER = 0xF7;     

#define MIDI_CHANNEL_POS	2

//operator structure (new 4op, bank/bulk)
struct FM_BULK_OPERATOR_NEW
{
	UCHAR AttackRate;	//(0-31)
	UCHAR DecayRate1;	//(0-31)
	UCHAR DecayRate2;	//(0-31)
	UCHAR ReleaseRate;	//(0-15)
	UCHAR Decay1Level;	//(0-15)
	UCHAR LevelScaling;	//(0-99)
//----
	UCHAR AmeEGBiasVel;
//breakdown
//b6 - Amplitude Modulation Enable (0/1)
//b5 4 3 - EG Bias sensitivity (0-7)
//b2 1 0 - Key Velocity sensitivity (0-7)
//----	
	UCHAR OutputLevel;	//(0-99)
	UCHAR OSCFreq;	//(0-63)
//----
	UCHAR RateScalingDetune;
//breakdown
//b4 3 - Keyboard Scaling rate (0-3)
//b2 1 0 - Detune (0-6, centre is 3)

};

struct FM_DX9_CARRIERS
{
	UCHAR Operators[4];
};


typedef FM_BULK_OPERATOR_NEW *lpFM_BULK_OPERATOR_NEW;

//Algo / LFO structure (new 4op, bank/bulk)
struct FM_BULK_LFO_NEW
{
//----
	UCHAR SyncFBackAlgo;
//breakdown
//b6 - Sync (0/1)
//b5 4 3 - Feedback Level (0-7)
//b2 1 0 - Algorithm (0-7)
//----
	UCHAR LFOSpeed;
	UCHAR LFODelay;
};

//Performance parameters (new 4op, bank/bulk)
struct FM_BULK_PERFORMANCE_NEW
{
	UCHAR PModDepth;
	UCHAR AModDepth;
//----
	UCHAR PmsAmsLFOWave;
//breakdown
//b6 5 4 - Pitch Modulation Sensitivity (0-7)
//b3 2 - Amplitude Modulation Sensitivity (0-3)
//b1 0 - LFO Wave (0-3)
//----
	UCHAR Transpose;
	UCHAR PBendRange;
//----	
	UCHAR ChorusPlayModeSustPortfootPortMode;
//breakdown
//b4 Chorus On/Off (0/1)
//b3 Poly Mode (0/1)
//b2 Sustain Foot Switch (0/1)
//b1 Portamento Foot Switch (0/1) 
//b0 Portamento Mode - Full Time (0/1)
//----
	UCHAR PortTime;
	UCHAR FootVolRange;
	UCHAR WheelPitchModRange;
	UCHAR WheelAmpModRange;
	UCHAR BreathPitchRange;
	UCHAR BreathAmpRange;
	UCHAR BreathPitchBias;
	UCHAR BreathEGBias;
};

//patch name (new 4op, bank/bulk)
struct FM_BULK_NAME_NEW
{
	UCHAR PatchName[10];
};

//pitch envelope (new 4op, bank/bulk)
struct FM_BULK_PITCH_ENVELOPE_NEW
{
	UCHAR Rate1;
	UCHAR Rate2;
	UCHAR Rate3;
	UCHAR Level1;
	UCHAR Level2;
	UCHAR Level3;
};


//complete (new) 4op patch structure  (bank/bulk)
struct FM_BULK_NEW_PATCH
{
	FM_BULK_OPERATOR_NEW FMOp[4];
	FM_BULK_LFO_NEW LFOParams;
	FM_BULK_PERFORMANCE_NEW PerfParameters;
	UCHAR PatchName[10];
	FM_BULK_PITCH_ENVELOPE_NEW PitchEnvelope;
	UCHAR Padding[55];
};
//pointer to same
typedef FM_BULK_NEW_PATCH *lpFM_BULK_NEW_PATCH;

//Now - the old file format (DX9)

/*
byte             bit #
#     6   5   4   3   2   1   0   param A       range  param B       range
----  --- --- --- --- --- --- ---  ------------  -----  ------------  -----
0                R1              OP6 EG R1      0-99
1                R2              OP6 EG R2      0-99
2                R3              OP6 EG R3      0-99
3                R4              OP6 EG R4      0-99
4                L1              OP6 EG L1      0-99
5                L2              OP6 EG L2      0-99
6                L3              OP6 EG L3      0-99
7                L4              OP6 EG L4      0-99
8                BP              LEV SCL BRK PT 0-99
9                LD              SCL LEFT DEPTH 0-99
10                RD              SCL RGHT DEPTH 0-99
11    0   0   0 |  RC   |   LC  | SCL LEFT CURVE 0-3   SCL RGHT CURVE 0-3
12  |      DET      |     RS    | OSC DETUNE     0-14  OSC RATE SCALE 0-7
13    0   0 |    KVS    |  AMS  | KEY VEL SENS   0-7   AMP MOD SENS   0-3
14                OL              OP6 OUTPUT LEV 0-99
15    0 |         FC        | M | FREQ COARSE    0-31  OSC MODE       0-1
16                FF              FREQ FINE      0-99
 - repeats for all remaining 5 operators
*/

struct FM_BULK_OPERATOR_OLD
{
	UCHAR Rate1;	//(0-99)
	UCHAR Rate2;	//(0-99)
	UCHAR Rate3;	//(0-99)
	UCHAR Rate4;	//(0-99)
	UCHAR Level1;	//(0-99)
	UCHAR Level2;	//(0-99)
	UCHAR Level3;	//(0-99)
	UCHAR Level4;	//(0-99)
	UCHAR BreakPoint;	//level scaling break point (0-99)
	UCHAR ScaleLeftDepth;	//Scale left depth (0-99)
	UCHAR ScaleRightDepth;	//Scale Right depth (0-99)
//----
	UCHAR ScaleLRCurve;		
//breakdown
//b3 2 - Scale Right Curve (0-3)
//b1 0 - Scale Left Curve (0-3)
//----
//----
	UCHAR OscDetuneRateScale;
//breakdown
//b6 5 4 3 - Detune (0-14)
//b2 1 0 - Osc Rate Scale (0-7)
//----
//----
	UCHAR KeyVelSensAModSens;
//breakdown
//b4 3 2 - Key Vel Sens (0-7)
//b1 0 - Amplitute Mod Sens (0-3)
//----
	UCHAR OutputLevel;	//output level (0-99)
//----
	UCHAR FreqCMode;
//breakdown
//b5 4 3 2 1 - Frequency Coarse (0-31)
//b0 - Fixed Mode (0-1)
//----
	UCHAR FreqFine; //Frequency fine (0-99)
	
};

typedef FM_BULK_OPERATOR_OLD *lpFM_BULK_OPERATOR_OLD;

/*
byte             bit #
#     6   5   4   3   2   1   0   param A       range  param B       range
----  -- - -- - -- - -- - -- - -- - -- - ------------  ---- - ------------  ---- -
102               PR1              PITCH EG R1   0 - 99
103               PR2              PITCH EG R2   0 - 99
104               PR3              PITCH EG R3   0 - 99
105               PR4              PITCH EG R4   0 - 99
106               PL1              PITCH EG L1   0 - 99
107               PL2              PITCH EG L2   0 - 99
108               PL3              PITCH EG L3   0 - 99
109               PL4              PITCH EG L4   0 - 99
*/
//pitch envelope (old 4op, bank/bulk)
struct FM_BULK_PITCH_ENVELOPE_OLD
{
	UCHAR Rate1;	//(0-99)
	UCHAR Rate2;	//(0-99)
	UCHAR Rate3;	//(0-99)
	UCHAR Rate4;	//(0-99)
	UCHAR Level1;	//(0-99)
	UCHAR Level2;	//(0-99)
	UCHAR Level3;	//(0-99)
	UCHAR Level4;	//(0-99)
};

/*

byte             bit #
 #     6   5   4   3   2   1   0   param A       range  param B       range
----  --- --- --- --- --- --- ---  ------------  -----  ------------  -----
110    0   0 |        ALG        | ALGORITHM     0-31
111    0   0   0 |OKS|    FB     | OSC KEY SYNC  0-1    FEEDBACK      0-7
*/
struct FM_BULK_ALGORITHM_OLD
{
	UCHAR Algorithm;	//(0-31)
						//----
	UCHAR OscKeySyncFeedback;
	//breakdown
	//b3 - Osc Key Sync (0-1)
	//b2 1 0 - Feedback (0-7)
	//----
};
/*
byte             bit #
#     6   5   4   3   2   1   0   param A       range  param B       range
----  --- --- --- --- --- --- ---  ------------  -----  ------------  -----
112               LFS              LFO SPEED     0-99
113               LFD              LFO DELAY     0-99
114               LPMD             LF PT MOD DEP 0-99
115               LAMD             LF AM MOD DEP 0-99
116  |   LPMS    |    LFW    |LKS| LF PT MOD SNS 0-7   WAVE 0-5,  SYNC 0-1
117              TRNSP             TRANSPOSE     0-48
*/
struct FM_BULK_LFO_OLD
{
	UCHAR LFOSpeed;	//(0-99)
	UCHAR LFODelay;	//(0-99)
	UCHAR LFOPitchModDepth;	//(0-99)
	UCHAR LFOAmpModDepth;	//(0-99)
//----	
	UCHAR LFOPModWaveSync;
//breakdown
//b6 5 4 - LFO Pitch Mod Sensitivity (0-7)
//b3 2 1 - LFO Wave (0-5)
//b0	 - Sync (0-1)
//----
	UCHAR Transpose;	//(0-48)
};

/*
118          NAME CHAR 1           VOICE NAME 1  ASCII
119          NAME CHAR 2           VOICE NAME 2  ASCII
120          NAME CHAR 3           VOICE NAME 3  ASCII
121          NAME CHAR 4           VOICE NAME 4  ASCII
122          NAME CHAR 5           VOICE NAME 5  ASCII
123          NAME CHAR 6           VOICE NAME 6  ASCII
124          NAME CHAR 7           VOICE NAME 7  ASCII
125          NAME CHAR 8           VOICE NAME 8  ASCII
126          NAME CHAR 9           VOICE NAME 9  ASCII
127          NAME CHAR 10          VOICE NAME 10 ASCII
*/


//complete (old) DX9 4op patch structure  (bank/bulk)
struct FM_BULK_OLD_PATCH
{
	FM_BULK_OPERATOR_OLD FMOp[6];
	FM_BULK_PITCH_ENVELOPE_OLD PitchEnvelope;
	FM_BULK_ALGORITHM_OLD Algorithm;
	FM_BULK_LFO_OLD Lfo;
	UCHAR PatchName[10];	
};

//pointer to same
typedef FM_BULK_OLD_PATCH *lpFM_BULK_OLD_PATCH;

/*
DX9 Dump:
pitch envelope
63 63 63 63 32 32 32 32

04 0a

Op 6-1 (6-3) R1234L1234x6, Levl Scl BP,scl left dept, scl right depth,
1a 0c 47 1c 62 20 20 20 0f 20 20 04 28 20 62 20 20
45 0b 47 1c 63 20 20 20 0f 20 20 04 28 20 5f 20 01
62 21 47 1c 62 20 20 20 0f 20 2c 04 23 20 4c 06 11
5f 08 47 1c 63 20 20 20 0f 20 20 04 3b 20 62 02 20
20 20 20 20 20 20 20 20 20 20 20 20 38 20 20 20 20
20 20 20 20 20 20 20 20 20 20 20 20 38 20 20 20 20

Pitch envelope
63 63 63 63 32 32 32 32
Algorithm
04
OscKey Sync (0-1 - b3) Feedback (0-7 - b2-0)
0a
LFOSpeed
1a
LFODelay
20
01 20 20 18 44 58 39 2e 20 31 20 20 20 20
*/
#endif

