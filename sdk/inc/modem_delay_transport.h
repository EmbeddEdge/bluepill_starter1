/*
 * Copyright 2020 Thingstream AG
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
 * @brief ThingstreamTransport implementation that delays a modem's
 * communication a serial connection so that the modem hardware can wake up
 * from low power mode.
 */

#ifndef INC_MODEM_DELAY_TRANSPORT_H
#define INC_MODEM_DELAY_TRANSPORT_H

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Create an instance of a delay transport for the modem transport.
 *
 * Some modems (e.g. Simcom, Quectel and u-blox) have a low power mode where
 * the UART clocks are reduced. When in this mode the modem wakes up when the
 * first byte is sent by the MCU, but the character is discarded (it will have
 * framing errors since the clocks are not correct). The MCU should send
 * something (e.g. "\n") and then wait to allow the modem to wakeup before
 * sending the real command to the modem.
 *
 * This transport sends the wakeupString and delays when needed by simulating
 * the sleep and wakeup.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param sleepMs the number of milliseconds before sleep mode activated.
 * @param wakeupString the string to send to the modem to wakeup (e.g. "\n")
 * @param afterWakeMs the number of milliseconds to wait after wakeupString.
 * @return the #ThingstreamTransport instance
 */
extern ThingstreamTransport* Thingstream_createModemDelayTransport(ThingstreamTransport* inner, uint16_t sleepMs, const char *wakeupString, uint16_t afterWakeMs);

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_DELAY_TRANSPORT_H */
