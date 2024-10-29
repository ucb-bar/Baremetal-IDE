#include <unistd.h>

#include "chip_config.h"

/**
 * @brief Sleep for a specified number of seconds
 * 
 * sleep() causes the calling thread to sleep either until the
 * number of real-time seconds specified in seconds have elapsed or
 * until a signal arrives which is not ignored.
 * 
 * @param seconds time to sleep
 * @return 0 if the requested time has elapsed, or the number of
 *         seconds left to sleep, if the call was interrupted by a signal handler.
 */
__attribute__((weak)) unsigned int sleep(unsigned int seconds);

/**
 * @brief Suspend execution for milliseconds intervals
 * 
 * The usleep() function suspends execution of the calling thread
 * for (at least) usec milliseconds.  The sleep may be lengthened
 * slightly by any system activity or by the time spent processing
 * the call or by the granularity of system timers.
 * 
 * @param msec time to sleep
 * @return 0 on success. On error, -1 is returned, with errno set to indicate the error.
*/
__attribute__((weak)) int msleep(useconds_t msec);

/**
 * @brief Suspend execution for microseconds intervals
 * 
 * The usleep() function suspends execution of the calling thread
 * for (at least) usec microseconds.  The sleep may be lengthened
 * slightly by any system activity or by the time spent processing
 * the call or by the granularity of system timers.
 * 
 * @param usec time to sleep
 * @return 0 on success. On error, -1 is returned, with errno set to indicate the error.
 */
__attribute__((weak)) int usleep(useconds_t usec);
