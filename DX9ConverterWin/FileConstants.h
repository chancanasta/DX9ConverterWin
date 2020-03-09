#ifndef __FILECONSTANTS_H
#define __FILECONSTANTS_H

#define BUFSIZE				1024
//input buffer size
#define INPUTBUFFERSIZE		(BUFSIZE*32)
//output buffer
#define OUTPUTBUFFERSIZE	(BUFSIZE*16)
//Text file
#define NAMEBUFFERSIZE		BUFSIZE


//sysex start and end bytes
#define SYSEX			0xF0
#define SYSEND			0xF7
//how the remaining 12 patches are filled
#define DX9FILLER		0x2E
#define FILLERPATCHES	12

#define SYSDATAPOS		6
#define SYSDATABYTES	0x1000

#define SYSCHKSUM		0x1006

#define SYSCHKSUM_SINGLE	0xa1
#define SYSDATABYTES_SINGLE	0x9b	

#define SYSEOF			0x1007
#define SYSLEN			0x1008
#define SYSLEN_SINGLE	0xa3

#define MAX_LOADSTRING 100


#define FM_2GEN_SINGLE_SIZE	142
#define FM_2GEN_ADD_SIZE	41

#endif
