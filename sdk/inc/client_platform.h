/*
 * Copyright 2017 Thingstream AG
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
 * @brief Client platform porting interface
 */
#include <stdint.h>

#ifndef INC_CLIENT_PLATFORM_H
#define INC_CLIENT_PLATFORM_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Return the current time in milliseconds. May return milliseconds since
 * system start, epoch, or any other reference point. This is used by the
 * client for measuring time intervals only.
 * @return the time in milliseconds.
 */
extern uint32_t Platform_getTimeMillis(void);

/**
 * A macro to compare two times, as returned from Platform_getTimeMillis(),
 * and return TRUE if the given comparison holds.
 * This macro handles zero-wrapping of either left or right values and provides
 * a result assuming that the times are within 24 days of each other.
 *
 * @param left the left millisecond count
 * @param cmp the comparison
 * @param right the right millisecond count
 * @return TRUE if (left cmp right) is true when treating left and right as
 *         reasonably close to each other.
 */
#define TIME_COMPARE(left, cmp, right)     \
    (((int32_t)((left) - (right))) cmp 0)

/**
 * Returns a string representation of the current time (e.g. for logging)
 * It is valid to return "" if forming the string is too difficult on the
 * platform.
 *
 * @return the string representation of the time
 */
extern const char* Platform_getTimeString(void);

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_PLATFORM_H */
