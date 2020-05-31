/*
 * Copyright 2017-2019 Thingstream AG
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

extern void debug_puts(const char* str, int len);
extern int debug_vprintf(const char* fmt, va_list ap);
extern int debug_printf(const char* fmt, ...);

#endif /* INC_DEBUG_PRINTF_CORE_H_ */
