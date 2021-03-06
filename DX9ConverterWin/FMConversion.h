#ifndef __FMCONVERSION_H
#define __FMCONVERSION_H

/*
LFO
New - Saw Up,Square,Triangle, S/H
Old - Triangle, Saw Down, Saw Up, Square, Sine S/h
*/
//The DX9 LFO waves
#define OLD_LFO_TRIANGLE	0
#define OLD_LFO_SAW_DOWN	1
#define OLD_LFO_SAW_UP		2
#define OLD_LFO_SQUARE		3
#define OLD_LFO_SINE		4
#define OLD_LFO_SH			5

//bit in the param that indictes if OSC sync is on or off
#define OSC_BULK_SYNC_ON	8
#define OSC_BULK_SYNC_OFF	0

//Pitch envelope defaults (no PEG on DX9)
#define PEG_LEVEL	0x32
#define PEG_RATE	0x63

//from DX21 Manual - DX21/27/100 frequency settings
static double DX21FREQ[64] = { 0.50, 0.71, 0.78, 0.87, 1.00, 1.41, 1.57, 1.73,
	2.00, 2.82, 3.00, 3.14, 3.46, 4.00, 4.24, 4.71,
	5.00, 5.19, 5.65, 6.00, 6.28, 6.92, 7.00, 7.07,
	7.85, 8.00, 8.48, 8.65, 9.00, 9.42, 9.89, 10.00,
	10.38, 10.99, 11.00, 11.30, 12.00, 12.11, 12.56, 12.72,
	13.00, 13.84, 14.00, 14.10, 14.13, 15.00, 15.55, 15.57,
	15.70, 16.96, 17.27, 17.30, 18.37, 18.84, 19.03, 19.78,
	20.41, 20.76, 21.20, 21.98, 22.49, 23.55, 24.22, 25.95 };

//from the https://mgregory22.me/tx81z/ - the TX18z/DX11 frequencies from fine/coarse settings
//an extension of the DX21/27/100 64 freq settings - with 16 addition fine settings per 'coarse' setting
static double TX81FREQ[64][16] = {
													//Fine values > 7 ignored for first 4 coarse values
	{0.50, 0.56, 0.62, 0.68, 0.75, 0.81, 0.87, 0.93, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50},//0
	{0.71, 0.79, 0.88, 0.96, 1.05, 1.14, 1.23, 1.32, 0.71, 0.71, 0.71, 0.71, 0.71, 0.71, 0.71, 0.71},//1
	{0.78, 0.88, 0.98, 1.07, 1.17, 1.27, 1.37, 1.47, 0.78, 0.78, 0.78, 0.78, 0.78, 0.78, 0.78, 0.78},//2
	{0.87, 0.97, 1.08, 1.18, 1.29, 1.40, 1.51, 1.62, 0.87, 0.87, 0.87, 0.87, 0.87, 0.87, 0.87, 0.87},//3
												   //Fine values > 7 ignored for first 4 coarse values
	{1.00, 1.06, 1.12, 1.18, 1.25, 1.31, 1.37, 1.43, 1.50, 1.56, 1.62, 1.68, 1.75, 1.81, 1.87, 1.93},//4
	{1.41, 1.49, 1.58, 1.67, 1.76, 1.85, 1.93, 2.02, 2.11, 2.20, 2.29, 2.37, 2.46, 2.55, 2.64, 2.73},//5
	{1.57, 1.66, 1.76, 1.86, 1.96, 2.06, 2.15, 2.25, 2.35, 2.45, 2.55, 2.64, 2.74, 2.84, 2.94, 3.04},//6
	{1.73, 1.83, 1.94, 2.05, 2.16, 2.27, 2.37, 2.48, 2.59, 2.70, 2.81, 2.91, 3.02, 3.13, 3.24, 3.35},//7
	{2.00, 2.06, 2.12, 2.18, 2.25, 2.31, 2.37, 2.43, 2.50, 2.56, 2.62, 2.68, 2.75, 2.81, 2.87, 2.93},//8
	{2.82, 2.90, 2.99, 3.08, 3.17, 3.26, 3.34, 3.43, 3.52, 3.61, 3.70, 3.78, 3.87, 3.96, 4.05, 4.14},//9
	{3.00, 3.06, 3.12, 3.18, 3.25, 3.31, 3.37, 3.43, 3.50, 3.56, 3.62, 3.68, 3.75, 3.81, 3.87, 3.93},//10
	{3.14, 3.23, 3.33, 3.43, 3.53, 3.63, 3.72, 3.82, 3.92, 4.02, 4.12, 4.21, 4.31, 4.41, 4.51, 4.61},//11
	{3.46, 3.56, 3.67, 3.78, 3.89, 4.00, 4.10, 4.21, 4.32, 4.43, 4.54, 4.64, 4.75, 4.86, 4.97, 5.08},//12
	{4.00, 4.06, 4.12, 4.18, 4.25, 4.31, 4.37, 4.43, 4.50, 4.56, 4.62, 4.68, 4.75, 4.81, 4.87, 4.93},//13
	{4.24, 4.31, 4.40, 4.49, 4.58, 4.67, 4.75, 4.84, 4.93, 5.02, 5.11, 5.19, 5.28, 5.37, 5.46, 5.55},//14
	{4.71, 4.80, 4.90, 5.00, 5.10, 5.20, 5.29, 5.39, 5.49, 5.59, 5.69, 5.78, 5.88, 5.98, 6.08, 6.18},//15
	{5.00, 5.06, 5.12, 5.18, 5.25, 5.31, 5.37, 5.43, 5.50, 5.56, 5.62, 5.68, 5.75, 5.81, 5.87, 5.93},//16
	{5.19, 5.29, 5.40, 5.51, 5.62, 5.73, 5.83, 5.94, 6.05, 6.16, 6.27, 6.37, 6.48, 6.59, 6.70, 6.81},//17
	{5.65, 5.72, 5.81, 5.90, 5.99, 6.08, 6.16, 6.25, 6.34, 6.43, 6.52, 6.60, 6.69, 6.78, 6.87, 6.96},//18
	{6.00, 6.06, 6.12, 6.18, 6.25, 6.31, 6.37, 6.43, 6.50, 6.56, 6.62, 6.68, 6.75, 6.81, 6.87, 6.93},//19
	{6.28, 6.37, 6.47, 6.57, 6.67, 6.77, 6.86, 6.96, 7.06, 7.16, 7.26, 7.35, 7.45, 7.55, 7.65, 7.75},//20
	{6.92, 7.02, 7.13, 7.24, 7.35, 7.46, 7.56, 7.67, 7.78, 7.89, 8.00, 8.10, 8.21, 8.32, 8.43, 8.54},//21
	{7.00, 7.06, 7.12, 7.18, 7.25, 7.31, 7.37, 7.43, 7.50, 7.56, 7.62, 7.68, 7.75, 7.81, 7.87, 7.93},//22
	{7.07, 7.13, 7.22, 7.31, 7.40, 7.49, 7.57, 7.66, 7.75, 7.84, 7.93, 8.01, 8.10, 8.19, 8.28, 8.37},//23
	{7.85, 7.94, 8.04, 8.14, 8.24, 8.34, 8.43, 8.53, 8.63, 8.73, 8.83, 8.92, 9.02, 9.12, 9.22, 9.32},//24
	{8.00, 8.06, 8.12, 8.18, 8.25, 8.31, 8.37, 8.43, 8.50, 8.56, 8.62, 8.68, 8.75, 8.81, 8.87, 8.93},//25
	{8.48, 8.54, 8.63, 8.72, 8.81, 8.90, 8.98, 9.07, 9.16, 9.25, 9.34, 9.42, 9.51, 9.60, 9.69, 9.78},//26
	{8.65, 8.75, 8.86, 8.97, 9.08, 9.19, 9.29, 9.40, 9.51, 9.62, 9.73, 9.83, 9.94, 10.05, 10.16, 10.27},//27
	{9.00, 9.06, 9.12, 9.18, 9.25, 9.31, 9.37, 9.43, 9.50, 9.56, 9.62, 9.68, 9.75, 9.81, 9.87, 9.93},//28
	{9.42, 9.51, 9.61, 9.71, 9.81, 9.91, 10.00, 10.10, 10.20, 10.30, 10.40, 10.49, 10.59, 10.69, 10.79, 10.89},//29
	{9.89, 9.95, 10.04, 10.13, 10.22, 10.31, 10.39, 10.48, 10.57, 10.66, 10.75, 10.83, 10.92, 11.01, 11.10, 11.19},//30
	{10.00, 10.06, 10.12, 10.18, 10.25, 10.31, 10.37, 10.43, 10.50, 10.56, 10.62, 10.68, 10.75, 10.81, 10.87, 10.93},//31
	{10.38, 10.48, 10.59, 10.70, 10.81, 10.92, 11.02, 11.13, 11.24, 11.35, 11.46, 11.56, 11.67, 11.78, 11.89, 12.00},//32
	{10.99, 11.08, 11.18, 11.28, 11.38, 11.48, 11.57, 11.67, 11.77, 11.87, 11.97, 12.06, 12.16, 12.26, 12.36, 12.46},//33
	{11.00, 11.06, 11.12, 11.18, 11.25, 11.31, 11.37, 11.43, 11.50, 11.56, 11.62, 11.68, 11.75, 11.81, 11.87, 11.93},//34
	{11.30, 11.36, 11.45, 11.54, 11.63, 11.72, 11.80, 11.89, 11.98, 12.07, 12.16, 12.24, 12.33, 12.42, 12.51, 12.60},//35
	{12.00, 12.06, 12.12, 12.18, 12.25, 12.31, 12.37, 12.43, 12.50, 12.56, 12.62, 12.68, 12.75, 12.81, 12.87, 12.93},//36
	{12.11, 12.21, 12.32, 12.43, 12.54, 12.65, 12.75, 12.86, 12.97, 13.08, 13.19, 13.29, 13.40, 13.51, 13.62, 13.73},//37
	{12.56, 12.65, 12.75, 12.85, 12.95, 13.05, 13.14, 13.24, 13.34, 13.44, 13.54, 13.63, 13.37, 13.83, 13.93, 14.03},//38
	{12.72, 12.77, 12.86, 12.95, 13.04, 13.13, 13.21, 13.30, 13.39, 13.48, 13.57, 13.65, 13.74, 13.83, 13.92, 14.01},//39
	{13.00, 13.06, 13.12, 13.18, 13.25, 13.31, 13.37, 13.43, 13.50, 13.56, 13.62, 13.68, 13.75, 13.81, 13.87, 13.93},//40
	{13.84, 13.94, 14.05, 14.16, 14.27, 14.38, 14.48, 14.59, 14.70, 14.81, 14.92, 15.02, 15.13, 15.24, 15.35, 15.46},//41
	{14.00, 14.06, 14.12, 14.18, 14.25, 14.31, 14.37, 14.43, 14.50, 14.56, 14.62, 14.68, 14.75, 14.81, 14.87, 14.93},//42
	{14.10, 14.18, 14.27, 14.36, 14.45, 14.54, 14.62, 14.71, 14.80, 14.89, 14.98, 15.06, 15.15, 15.24, 15.33, 15.42},//43
	{14.13, 14.22, 14.32, 14.42, 14.52, 14.62, 14.71, 14.81, 14.91, 15.01, 15.11, 15.20, 15.30, 15.40, 15.50, 15.60},//44
	{15.00, 15.06, 15.12, 15.18, 15.25, 15.31, 15.37, 15.43, 15.50, 15.56, 15.62, 15.68, 15.75, 15.81, 15.87, 15.93},//45
	{15.55, 15.59, 15.68, 15.77, 15.86, 15.95, 16.03, 16.12, 16.21, 16.30, 16.39, 16.47, 16.56, 16.65, 16.74, 16.83},//46
	{15.57, 15.67, 15.78, 15.89, 16.00, 16.11, 16.21, 16.32, 16.43, 16.54, 16.65, 16.75, 16.86, 16.97, 17.08, 17.19},//47
	{15.70, 15.79, 15.89, 15.99, 16.09, 16.19, 16.28, 16.38, 16.48, 16.58, 16.68, 16.77, 16.87, 16.97, 17.07, 17.17},//48
	{16.96, 17.00, 17.09, 17.18, 17.27, 17.36, 17.44, 17.53, 17.62, 17.71, 17.80, 17.88, 17.97, 18.06, 18.15, 18.24},//49
	{17.27, 17.36, 17.46, 17.56, 17.66, 17.76, 17.85, 17.95, 18.05, 18.15, 18.25, 18.35, 18.44, 18.54, 18.64, 18.74},//50
	{17.30, 17.40, 17.51, 17.62, 17.73, 17.84, 17.94, 18.05, 18.16, 18.27, 18.38, 18.48, 18.59, 18.70, 18.81, 18.92},//51
	{18.37, 18.41, 18.50, 18.59, 18.68, 18.77, 18.85, 18.94, 19.03, 19.12, 19.21, 19.29, 19.38, 19.47, 19.56, 19.65},//52
	{18.84, 18.93, 19.03, 19.13, 19.23, 19.33, 19.42, 19.52, 19.62, 19.72, 19.82, 19.91, 20.01, 20.11, 20.21, 20.31},//53
	{19.03, 19.13, 19.24, 19.35, 19.46, 19.57, 19.67, 19.78, 19.89, 20.00, 20.11, 20.21, 20.32, 20.43, 20.54, 20.65},//54
	{19.78, 19.82, 19.91, 20.00, 20.09, 20.18, 20.26, 20.35, 20.44, 20.53, 20.62, 20.70, 20.79, 20.88, 20.97, 21.06},//55
	{20.41, 20.50, 20.60, 20.70, 20.80, 20.90, 20.99, 21.09, 21.19, 21.29, 21.39, 21.48, 21.58, 21.68, 21.78, 21.88},//56
	{20.76, 20.86, 20.97, 21.08, 21.19, 21.30, 21.40, 21.51, 21.62, 21.73, 21.48, 21.94, 22.05, 22.16, 22.27, 22.38},//57
	{21.20, 21.23, 21.32, 21.41, 21.50, 21.59, 21.67, 21.76, 21.85, 21.94, 22.03, 22.11, 22.20, 22.29, 22.38, 22.47},//58
	{21.98, 22.07, 22.17, 22.27, 22.37, 22.47, 22.56, 22.66, 22.76, 22.86, 22.96, 23.05, 23.15, 23.25, 23.35, 23.45},//59
	{22.49, 22.59, 22.70, 22.81, 22.92, 23.03, 23.13, 23.24, 23.35, 23.46, 23.57, 23.67, 23.78, 23.89, 24.00, 24.11},//60
	{23.55, 23.64, 23.74, 23.84, 23.94, 24.04, 24.13, 24.23, 24.33, 24.43, 24.53, 24.62, 24.72, 24.82, 24.92, 25.02},//61
	{24.22, 24.32, 24.43, 24.54, 24.65, 24.76, 24.86, 24.97, 25.08, 25.19, 25.30, 25.40, 25.51, 25.62, 25.73, 25.84},//62
	{25.95, 26.05, 26.16, 26.27, 26.38, 26.49, 26.59, 26.70, 26.81, 26.92, 27.03, 27.13, 27.24, 27.35, 27.46, 27.57} //63
};


/* This table converts LFO speed to frequency in Hz. It is based on
* interpolation of Jamie Bullock's measurements. */
static double DX9LFOFREQ[100] = {
	0.062506,  0.124815,  0.311474,  0.435381,  0.619784, 0.744396,  0.930495,  1.116390,  1.284220,  1.496880,
	1.567830,  1.738994,  1.910158,  2.081322,  2.252486, 2.423650,  2.580668,  2.737686,  2.894704,  3.051722,
	3.208740,  3.366820,  3.524900,  3.682980,  3.841060, 3.999140,  4.159420,  4.319700,  4.479980,  4.640260,
	4.800540,  4.953584,  5.106628,  5.259672,  5.412716, 5.565760,  5.724918,  5.884076,  6.043234,  6.202392,
	6.361550,  6.520044,  6.678538,  6.837032,  6.995526, 7.154020,  7.300500,  7.446980,  7.593460,  7.739940,
	7.886420,  8.020588,  8.154756,  8.288924,  8.423092, 8.557260,  8.712624,  8.867988,  9.023352,  9.178716,
	9.334080,  9.669644, 10.005208, 10.340772, 10.676336, 11.011900, 11.963680, 12.915460, 13.867240, 14.819020,
	15.770800, 16.640240, 17.509680, 18.379120, 19.248560, 20.118000, 21.040700, 21.963400, 22.886100, 23.808800,
	24.731500, 25.759740, 26.787980, 27.816220, 28.844460, 29.872700, 31.228200, 32.583700, 33.939200, 35.294700,
	36.650200, 37.812480, 38.974760, 40.137040, 41.299320, 42.461600, 43.639800, 44.818000, 45.996200, 47.174400
};

//0.4714

//DX9 coarse frequencies			
static double DX9COARSEFREQ[31] = { 0.5,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,19,20,21,22,23,24,25,26,27,28,29,30 };


//array to convert new LFO waves to DX9
static UCHAR LFOCONVERT[4] = { OLD_LFO_SAW_UP,OLD_LFO_SQUARE,OLD_LFO_TRIANGLE,OLD_LFO_SH };

//Keyboard scaling rate array
//static UCHAR SCALINGRATECONVERT[4] = { 0,2,5,7 };	//0 = 0
static UCHAR SCALINGRATECONVERT[4] = { 1,2,5,7 };	//0 = 1

//detune
//2 arrays - one spanning the full DX9 detune range
//static UCHAR DETUNECONVERT[7] = { 0,2,5,7,9,11,14 };

//and the second spanning only the same range as the later 4ops but recentered for the DX9
static UCHAR DETUNECONVERT[7] = { 4,5,6,7,8,9,10 };
//This is the one we're using for now

//Algorithm mapping
//DX9 1  2  3  4  5  6  7  8
//DX7 1  14 8  7  5  22 31 32
static UCHAR MAPALGO[8] = { 0,13,7,6,4,21,30,31 };

/*Mapping operators in the algorithms
Note that the operators oddly and unhelpfully are stored 4, 2, 3, 1 in the later 4op synths compared to 6,5,4,3 in the DX9
In addition we also need to remap algorithm 8

DX9 version of algorithm
    4
    |
   (2  3
	| /
	1
	DX9 Algo 3
	DX7 Algo 8

version on the later 4ops

	3
	|
	2  4)
	| /
	1

	Operator Mapping :
4 <- 3
3 <- 2
2 <- 4
1 <- 1
*/
static FM_DX9_CARRIERS OPERATORMAP[8] = {	{ 0,2,1,3 },//1 (1)
											{ 0,2,1,3 },//2 (14)
											{ 2,1,0,3 },//3 (8)<-- operator re-mapping
											{ 0,2,1,3 },//4 (7) 
											{ 0,2,1,3 },//5 (5)
											{ 0,2,1,3 },//6 (21)
											{ 0,2,1,3 },//7 (30)
											{ 0,2,1,3 } //8 (31)																		
										};

//Output levels
//based on tables from http://www.angelfire.com/in2/yala/t2dx-fm.htm
/*
static UCHAR FM_DX9_OUTLEVEL[100] = { 0,  3,  4,  5,  7,  8, 10, 11, 12, 13,
									 14, 15, 16, 18, 19, 20, 21, 22, 24, 25,
									 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
								     37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
									 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
									 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
									 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
									 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
									 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
									 97, 98, 99, 99, 99, 99, 99, 99, 99, 99 };
*/
/*
static UCHAR FM_DX9_OUTLEVEL[100] = {	0,  3,  4,  5,  7,  8,  10, 11, 12, 13,
										14, 15, 16, 17, 19, 20, 21, 22, 24, 25,
										26, 27, 28, 30, 31, 32, 33, 34, 35, 36,
										37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
										47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
										57, 58, 59, 60, 60, 60, 61, 61, 61, 62,
										63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
										73, 76, 79, 80, 81, 82, 83, 84, 86, 86,
										80, 88, 89, 90, 91, 92, 93, 94, 95, 96,
										97, 98, 99, 99, 99, 99, 99, 99, 99, 99};
*/

static UCHAR FM_DX9_OUTLEVEL[100] = {	0,  3,  4,  5,  7,  8,  10, 11, 12, 13,
										14, 15, 16, 17, 19, 20, 21, 22, 24, 25,
										26, 27, 28, 30, 31, 32, 33, 34, 35, 36,
										37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
										47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
										57, 58, 59, 60, 60, 60, 61, 61, 61, 62,
										62, 63, 63, 63, 64, 64, 66, 67, 68, 69,
										70, 73, 75, 78, 80, 82, 83, 84, 86, 86,
										80, 88, 89, 90, 91, 92, 93, 94, 95, 96,
										97, 98, 99, 99, 99, 99, 99, 99, 99, 99 };
/*
Orig:    5   10   15   20   25   30   35   40   45   50   55   60   65   70   75
 	 X : 3    6   11   15   19   23   28   33   38   43   48   53   58   63   68

	Orig : 80   82   84   86   88   90   91   92   93   94   95   96   97   98   99
  	X    : 73   75   77   79   81   83   84   85   86   87   88   89   90   91   92
*/

	
//array to figure out which operators are carriers within the DX9 algorithms
//not currently used, but useful to know
static FM_DX9_CARRIERS  DX9ALGOCARRIERS[32] = {
	              //DX7 DX9
	{ { 0,0,0,1 } },//1 1
	{ { 0,0,0,0 } },//2
	{ { 0,0,0,0 } },//3
	{ { 0,0,0,0 } },//4
	{ { 0,1,0,1 } },//5 5
	{ { 0,0,0,0 } },//6
	{ { 0,0,0,1 } },//7 4
	{ { 0,0,0,1 } },//8 3
	{ { 0,0,0,0 } },//9
	{ { 0,0,0,0 } },//10
	{ { 0,0,0,0 } },//11
	{ { 0,0,0,0 } },//12
	{ { 0,0,0,0 } },//13
	{ { 0,0,0,1 } },//14 2
	{ { 0,0,0,0 } },//15
	{ { 0,0,0,0 } },//16
	{ { 0,0,0,0 } },//17
	{ { 0,0,0,0 } },//18
	{ { 0,0,0,0 } },//19
	{ { 0,0,0,0 } },//20
	{ { 0,0,0,0 } },//21
	{ { 0,1,1,1 } },//22 6
	{ { 0,0,0,0 } },//23
	{ { 0,0,0,0 } },//24
	{ { 0,0,0,0 } },//25
	{ { 0,0,0,0 } },//26
	{ { 0,0,0,0 } },//27
	{ { 0,0,0,0 } },//28
	{ { 0,0,0,0 } },//29
	{ { 0,0,0,0 } },//30
	{ { 0,1,1,1 } },//31 7
	{ { 1,1,1,1 } } //32 8
};


/*
Taken from http://www.angelfire.com/in2/yala/t2dx-fm.htm
ENVELOPE PARAMETERS
Attack(A) ~Rate(R) Conversion
DX - 7  R:  15  21  27  34  40  47  54  60  67  74  80  85  89  93  96  99
DX - 21 A : 1   3   5   7   9  11  13  15  17  19  21  23  25  27  29  31
Decay(D) ~Rate(R) Conversion
DX - 7  R : 10  16  21  27  33  39  45  51  57  63  69  75  81  87  93  99
DX - 21 D : 1   3   5   7   9  11  13  15  17  19  21  23  25  27  29  31
Sustain(S) ~Level(L) Conversion
DX - 7  L : 35  39  44  48  53  57  62  66  71  75  80  84  89  93  99
DX - 21 S : 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
Release(R bottom) ~Rate(R top) Conversion
DX - 7  R : 21  27  32  38  43  49  54  60  65  71  76  82  87  94  99
DX - 21 R : 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
*/


//Array to convert from new attack rate to DX9 attack rate
static UCHAR AttackRate[32] = {0,15,18,21,24,27,30,34,37,40,43,47,51,54,57,60,63,67,71,74,77,80,82,85,87,89,91,93,94,96,97,99};
//Array to convert from new decay rate to DX9 attack rate
static UCHAR DecayRate[32] = {0,10,13,16,19,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,93,96,99};
//Array to convert from new sustain level to DX9
static UCHAR SustainLevel[16] = {0,35,39,44,48,53,57,62,66,71,75,80,84,89,93,99};
//Array to convert from new release rate to DX9
static UCHAR ReleaseRate[16] = {0,21,27,32,38,43,49,54,60,65,71,76,82,87,94,99};

/*EG notes
Converting from the later 4op 'Enhanced ADSR' to the older DX9 4 levels and rates
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



#endif 

