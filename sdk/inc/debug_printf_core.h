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

#ifndef INC_DEBUG_PRINTF_CORE_H_
#define INC_DEBUG_PRINTF_CORE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include <thingstream_util.h>

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/* These functions have been renamed and compiled into the SDK library */

#define Platform_debug_vprintf Thingstream_Util_vprintf
#define Platform_debug_printf  Thingstream_Util_printf

#define Platform_debug_puts  Thingstream_Platform_puts

#ifndef THINGSTREAM_NO_SHORT_NAMES

/** @deprecated renamed to Platform_debug_puts() */
#define debug_puts         Platform_debug_puts

/** @deprecated renamed to Platform_debug_printf() */
#define debug_printf       Platform_debug_printf

/** @deprecated renamed to Platform_debug_vprintf() */
#define debug_vprintf      Platform_debug_vprintf

#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined (__cplusplus)
}
#endif

#endif /* INC_DEBUG_PRINTF_CORE_H_ */
