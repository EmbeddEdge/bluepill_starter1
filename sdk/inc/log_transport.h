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

/**
 * @file
 * @brief ThingstreamTransport implementation that logs traffic between any two transports.
 */

#ifndef INC_LOG_TRANSPORT_H_
#define INC_LOG_TRANSPORT_H_

#include <stdint.h>

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Add logging to an underlying transport instance.
 * @param inner the #ThingstreamTransport instance to wrap
 * @param log the function to use for printing to the log
 * @param level_mask a bitmask specifying which messages to write to the log
 * @return the new ThingstreamTransport instance
 */
extern ThingstreamTransport* Thingstream_createTransportLogger(ThingstreamTransport* inner, ThingstreamPrintf_t log, uint8_t level_mask);

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */
/** @deprecated    renamed to Thingstream_createTransportLogger() */
#define log_transport_create  Thingstream_createTransportLogger
/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_LOG_TRANSPORT_H_ */
