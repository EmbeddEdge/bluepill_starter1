/*
 * Copyright 2019-2020 Thingstream AG
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
 * @brief Thingstream utilities
 */

#ifndef INC_THINGSTREAM_UTIL_H_
#define INC_THINGSTREAM_UTIL_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdarg.h>

/**
 * @addtogroup util-conversions
 * @{
 */

/**
 * Convert an integer to an unsigned base-10 string representation.
 * A helper routine to avoid pulling in the large sprintf() library.
 * Note that the result is not 0-terminated.
 * @param buf buffer to receive the string - assumed to be big enough
 *    to hold largest possible uint32_t value (10 bytes).
 * @param num the number to be converted
 * @return a pointer to just after the last byte of the conversion.
 */
char *Thingstream_Util_sprintfUInt(char *buf, uint32_t num);

/**
 * Convert an integer to an unsigned base-16 string representation.
 * A helper routine to avoid pulling in the large sprintf() library.
 * Note that the result is not 0-terminated.
 * @param buf buffer to receive the string - assumed to be big enough
 *    to hold largest possible uint32_t value (8 bytes).
 * @param num the number to be converted
 * @return a pointer to just after the last byte of the conversion.
 */
char *Thingstream_Util_sprintfHex(char *buf, uint32_t num);

/**
 * A helper to parse a sequence of ascii digits as a signed decimal number.
 * @param ptr start of sequence
 * @param end end of sequence, or NULL to parse until non-digit character
 * @param pOut if non-NULL, address of first non-digit is written here
 */
int32_t Thingstream_Util_parseInt(const char *ptr, const char *end, char **pOut);

/**
 * A helper to parse a sequence of ascii digits as an unsigned decimal number.
 * @param ptr start of sequence
 * @param end end of sequence, or NULL to parse until non-digit character
 * @param pOut if non-NULL, address of first non-digit is written here
 */
uint32_t Thingstream_Util_parseUInt(const char *ptr, const char *end, char **pOut);

/**
 * A helper to parse a sequence of ascii digits as a hex number.
 * @param ptr start of sequence
 * @param end end of sequence, or NULL to parse until non-digit character
 * @param pOut if non-NULL, address of first non-digit is written here
 */
uint32_t Thingstream_Util_parseHex(const char *ptr, const char *end, char **pOut);

/** @} */


/**
 * @addtogroup util-printing
 * @{
 */

/**
 * vprintf-style routine suitable for basic output. Supports:
 * <ul>
 * <li>\%d /\%x /\%p with optional 0-padding and width
 * <li>\%s with optional width
 * <li>\%c / \%\%
 * </ul>
 * @param fmt printf-style format string
 * @param ap va_list arguments
 */
int Thingstream_Util_vprintf(const char * fmt, va_list ap);

/**
 * printf-style routine suitable for basic output and SDK loggers
 * @param fmt printf-style format string,
 *            supports a sub-set of standard printf formatting,
 *            see Thingstream_Util_vprintf() for details
 */
int Thingstream_Util_printf(const char * fmt, ...);

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* INC_THINGSTREAM_UTIL_H_ */
