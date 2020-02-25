#ifndef __DXDISPLAY_H
#define __DXDISPLAY_H


//mainly for debug purposes - display the settings for 'old' (DX7/9) bulk voice
void DisplayBulkVoices(UCHAR *readBuffer);
void DisplayVoice(lpFM_BULK_OLD_PATCH lpOldPatch, int patchNo);
//single voice dump
void DisplaySingleVoice(UCHAR *readBuffer);


#endif

