#ifndef __mbed2_compatible_h__
#define __mbed2_compatible_h__

#if (MBED_MAJOR_VERSION > 2)
// mbed-os 5 has wait_ms deprecated
#define wait_ms ThisThread::sleep_for    
#endif


#endif
