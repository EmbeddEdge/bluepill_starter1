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
 * @brief The deprecated Thinsgtream_Modem*_setXxxxCallback APIs
 */

#ifndef INC_MODEM_SET_CALLBACK_H_
#define INC_MODEM_SET_CALLBACK_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"

/**
 * @addtogroup legacy-modem
 * @{
 */

/**
 * Type definition of the "modem response arrived" callback
 * @param cookie the cookie passed to Thingstream_Modem_setCallback()
 * @param response the unrecognized modem response
 * @param len the length of the response
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
typedef void (*ThingstreamModemCallback_t)(void *cookie, const char* response, uint16_t len);

/**
 * Set the function that will be called when the modem receives an
 * unrecognized response.
 * @param self instance of modem transport
 * @param callback the #ThingstreamModemCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
void Thingstream_ModemUssd_setCallback(ThingstreamTransport* self, ThingstreamModemCallback_t callback, void* cookie);

/**
 * Clear the function that will be called when the transport layer receives an
 * unrecognized response.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept unrecognized responses from the modem.
 * @param self instance of modem transport
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
#define Thingstream_ModemUssd_clearCallback(self) \
    Thingstream_ModemUssd_setCallback((self), NULL, NULL)

/**
 * Set the function that will be called when the modem receives an
 * unrecognized response.
 * @param self this instance of modem transport
 * @param callback the #ThingstreamModemCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
void Thingstream_Modem2_setCallback(ThingstreamTransport* self, ThingstreamModemCallback_t callback, void* cookie);

/**
 * Clear the function that will be called when the transport layer receive an
 * unrecognized response.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept unrecognized responses from the modem.
 * @param self this instance of modem transport
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
#define Thingstream_Modem2_clearCallback(self) \
    Thingstream_Modem2_setCallback((self), NULL, NULL)

/** @} */

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated Alias for #ThingstreamModemCallback_t */
typedef ThingstreamModemCallback_t Modem_callback;

/** @deprecated         renamed to Thingstream_ModemUssd_setCallback() */
#define Modem_set_modem_callback   Thingstream_ModemUssd_setCallback

/** @deprecated         renamed to Thingstream_ModemUssd_clearCallback() */
#define Modem_clear_modem_callback Thingstream_ModemUssd_clearCallback

/** @deprecated renamed to Thingstream_Modem2_setCallback() */
#define Modem2_set_modem_callback   Thingstream_Modem2_setCallback

/** @deprecated renamed to Thingstream_Modem2_clearCallback() */
#define Modem2_clear_modem_callback Thingstream_Modem2_clearCallback

/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_SET_CALLBACK_H_ */
