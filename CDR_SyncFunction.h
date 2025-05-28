#ifndef __GN2146Sync_H_
#define __GN2146Sync_H_

#ifdef CDR_GN2146
#define GN2146_DEVICE_ADDRESS			0x56
void resetGN2146(unsigned char* vendorFn);
void initializeMemoryGN2146(unsigned char* syncMemory);
void syncGN2146(unsigned char* syncMemory);
void manualGN2146(unsigned char* vendorFn);

#endif

#endif
