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
	UCHAR ScaleLeftDepth;	//Scale left depth (0-99) (not used by DX9)
	UCHAR ScaleRightDepth;	//Scale Right depth (0-99)
//----
	UCHAR ScaleLRCurve;		//(not used by DX9)
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
//b0 - Fixed Mode (0-1) (always 0 [ratio mode] on DX9)
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
struct FM_PITCH_ENVELOPE_OLD
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
	FM_PITCH_ENVELOPE_OLD PitchEnvelope;
	FM_BULK_ALGORITHM_OLD Algorithm;
	FM_BULK_LFO_OLD Lfo;
	UCHAR PatchName[10];	
};

//pointer to same
typedef FM_BULK_OLD_PATCH *lpFM_BULK_OLD_PATCH;

//'old' single voice structures

/*
11110000  F0   Status byte - start sysex
0iiiiiii  43   ID # (i=67; Yamaha)
0sssnnnn  00   Sub-status (s=0) & channel number (n=0; ch 1)
0fffffff  00   format number (f=0; 1 voice)
0bbbbbbb  01   byte count MS byte
0bbbbbbb  1B   byte count LS byte (b=155; 1 voice)
0ddddddd  **   data byte 1

|       |       |

0ddddddd  **   data byte 155
0eeeeeee  **   checksum (masked 2's complement of sum of 155 bytes)
11110111  F7   Status - end sysex

-------------

Data Structure: Single Voice Dump & Parameter #'s (single voice format, g=0)
-------------------------------------------------------------------------

Parameter
Number    Parameter                  Value Range
---------  ---------                  -----------
0        OP6 EG rate 1              0-99
1         "  "  rate 2               "
2         "  "  rate 3               "
3         "  "  rate 4               "
4         "  " level 1               "
5         "  " level 2               "
6         "  " level 3               "
7         "  " level 4               "
8        OP6 KBD LEV SCL BRK PT      "        C3= $27
9         "   "   "   "  LFT DEPTH   "
10         "   "   "   "  RHT DEPTH   "
11         "   "   "   "  LFT CURVE  0-3       0=-LIN, -EXP, +EXP, +LIN
12         "   "   "   "  RHT CURVE   "            "    "    "    "
13        OP6 KBD RATE SCALING       0-7
14        OP6 AMP MOD SENSITIVITY    0-3
15        OP6 KEY VEL SENSITIVITY    0-7
16        OP6 OPERATOR OUTPUT LEVEL  0-99
17        OP6 OSC MODE (fixed/ratio) 0-1        0=ratio
18        OP6 OSC FREQ COARSE        0-31
19        OP6 OSC FREQ FINE          0-99
20        OP6 OSC	DETUNE             0-14       0: det=-7
21 \
|  > repeat above for OSC 5, OSC 4,  ... OSC 1
125 /
126        PITCH EG RATE 1            0-99
127          "    " RATE 2              "
128          "    " RATE 3              "
129          "    " RATE 4              "
130          "    " LEVEL 1             "
131          "    " LEVEL 2             "
132          "    " LEVEL 3             "
133          "    " LEVEL 4             "
134        ALGORITHM #                 0-31
135        FEEDBACK                    0-7
136        OSCILLATOR SYNC             0-1
137        LFO SPEED                   0-99
138         "  DELAY                    "
139         "  PITCH MOD DEPTH          "
140         "  AMP   MOD DEPTH          "
141        LFO SYNC                    0-1
142         "  WAVEFORM                0-5, (data sheet claims 9-4 ?!?) 0:TR, 1:SD, 2:SU, 3:SQ, 4:SI, 5:SH
143        PITCH MOD SENSITIVITY       0-7
144        TRANSPOSE                   0-48   12 = C2
145        VOICE NAME CHAR 1           ASCII
146        VOICE NAME CHAR 2           ASCII
147        VOICE NAME CHAR 3           ASCII
148        VOICE NAME CHAR 4           ASCII
149        VOICE NAME CHAR 5           ASCII
150        VOICE NAME CHAR 6           ASCII
151        VOICE NAME CHAR 7           ASCII
152        VOICE NAME CHAR 8           ASCII
153        VOICE NAME CHAR 9           ASCII
154        VOICE NAME CHAR 10          ASCII
155        OPERATOR ON/OFF
bit6 = 0 / bit 5: OP1 / ... / bit 0: OP6

Note that there are actually 156 parameters listed here, one more than in
a single voice dump. The OPERATOR ON/OFF parameter is not stored with the
voice, and is only transmitted or received while editing a voice. So it
only shows up in parameter change SYS-EX's.
*/

//single voices a bit more straightfowarded with no 'bit packed' data
struct FM_SINGLE_OPERATOR_OLD
{
	UCHAR Rate1;	//(0-99)
	UCHAR Rate2;	//(0-99)
	UCHAR Rate3;	//(0-99)
	UCHAR Rate4;	//(0-99)
	UCHAR Level1;	//(0-99)
	UCHAR Level2;	//(0-99)
	UCHAR Level3;	//(0-99)
	UCHAR Level4;	//(0-99)
	UCHAR BreakPoint;	//break point (0-99)	(not used by DX9)
	UCHAR ScaleLeftDepth;	//Scale left depth (0-99) (not used by DX9)
	UCHAR ScaleRightDepth;	//Scale Right depth (0-99)						
	UCHAR ScaleLeftCurve;	// 0-3       0=-LIN, -EXP, +EXP, +LIN (not used by DX9)
	UCHAR ScaleRightCurve;	// 0-3       0=-LIN, -EXP, +EXP, +LIN (not used by DX9)
	UCHAR KeyRateScaling;	//0-7
	UCHAR AmpModSens;		//0-3
	UCHAR KeyVelSens;		//0-7 (not used by DX9) 
	UCHAR OutputLevel;		//output level (0-99)
	UCHAR OscMode;			//fixed(0) or ratio(1) - (always ratio for DX9)
	UCHAR FreqCoarse;		//coarse freq (0-31)
	UCHAR FreqFine;			//fine frequency (0-99)
	UCHAR OscDetune;		//0-14
};

typedef FM_BULK_OPERATOR_OLD *lpFM_BULK_OPERATOR_OLD;


struct FM_SINGLE_OLD_ALGORITHM
{
	UCHAR Algorithm;	//0-31
	UCHAR Feedback;		//0-7
	UCHAR OscSync;		//0-1
};
typedef FM_SINGLE_OLD_ALGORITHM *lpFM_SINGLE_OLD_ALGORITHM;


struct FM_SINGLE_LFO_OLD
{
	UCHAR LFOSpeed;	//(0-99)
	UCHAR LFODelay;	//(0-99)
	UCHAR LFOPitchModDepth;	//(0-99)
	UCHAR LFOAmpModDepth;	//(0-99)
	UCHAR LFOSync;	//(0-1) (not used by DX9)
	UCHAR WaveForm;	//0-5
	UCHAR PitchModSens;	//0-7
	UCHAR Transpose;	//0-48
};

struct FM_SINGLE_OLD_PATCH
{
	FM_SINGLE_OPERATOR_OLD FMOp[6];
	FM_PITCH_ENVELOPE_OLD PitchEnvelope;
	FM_SINGLE_OLD_ALGORITHM Alogrithm;
	FM_SINGLE_LFO_OLD Lfo;
	UCHAR PatchName[10];
};

typedef FM_SINGLE_OLD_PATCH *lpFM_SINGLE_OLD_PATCH;


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

