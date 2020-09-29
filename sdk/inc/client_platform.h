/*
 * Copyright 2017-2020 Thingstream AG
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * @brief ThingstreamClient platform porting interface
 */
#include <stdint.h>

#ifndef INC_CLIENT_PLATFORM_H
#define INC_CLIENT_PLATFORM_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @addtogroup porting-platform
 * @{
 */

/**
 * Return the current time in milliseconds. May return milliseconds since
 * system start, epoch, or any other reference point. This is used by the
 * client for measuring time intervals only.
 * @return the time in milliseconds.
 */
extern uint32_t Thingstream_Platform_getTimeMillis(void);

/**
 * A macro to compare two times, as returned from
 * Thingstream_Platform_getTimeMillis(), and return TRUE if
 * the given comparison holds.
 * This macro handles zero-wrapping of either left or right values and provides
 * a result assuming that the times are within 24 days of each other.
 * @ingroup util-time
 * @param left the left millisecond count
 * @param cmp the comparison
 * @param right the right millisecond count
 * @return TRUE if (left cmp right) is true when treating left and right as
 *         reasonably close to each other.
 */
#define TIME_COMPARE(left, cmp, right)     \
    (((int32_t)((left) - (right))) cmp 0)

/**
 * Returns a string representation of the current time (e.g. for logging) <br/>
 * **Optional** only needed if transport loggers are used.
 * It is valid to return "" if forming the string is too difficult on the
 * platform.
 *
 * @return the string representation of the time
 */
extern const char* Thingstream_Platform_getTimeString(void);

/**
 * Output a string to a debugging stream. <br/>
 * **Optional** only needed if Thingstream_Util_printf() is called.
 * @param str the string to be written (may not be 0-terminated)
 * @param len the number of characters to write.
 */
extern void Thingstream_Platform_puts(const char* str, int len);

/**
 * Report an assertion failure. When using the debug version of
 * the SDK, calls will be made to this function to report errors.
 * For example, when invalid parameters are passed to SDK functions.
 * It is strongly recommended that this version of the SDK is used
 * during development, and that this function is implemented in
 * such a way that it is very obvious that it has been called.
 * @param location a representation of the location of the problem
 * @param expr details of the problem
 */
extern void Thingstream_Platform_assertionFailure(int location, const char *expr);

/** @} */


#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */
/** @deprecated         renamed to Thingstream_Platform_getTimeMillis() */
#define Platform_getTimeMillis     Thingstream_Platform_getTimeMillis

/** @deprecated         renamed to Thingstream_Platform_getTimeString() */
#define Platform_getTimeString     Thingstream_Platform_getTimeString
/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_PLATFORM_H */
