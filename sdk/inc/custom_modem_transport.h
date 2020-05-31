/*
 * Copyright 2017-2018 Thingstream AG
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
 * @brief Transport implementation that communicates with a custom modem
 */

#ifndef INC_CUSTOM_MODEM_TRANSPORT_H
#define INC_CUSTOM_MODEM_TRANSPORT_H

#include <transport_api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/** The initialisation AT commands that can be sent before we are registered
  * on the network.
  */
extern const char ModemInitStringPreReg[];

/** The initialisation AT commands that must be sent after we are registered
  * on the network.
  */
extern const char ModemInitStringPostReg[];

/**
 * Create an instance of the modem transport.
 * TODO: @param ?
 * @return the instance
 */
extern Transport* modem_transport_create(/* porting specific options */);


/**
 * Type definition of the modem message arrived callback
 * @param cookie the cookie passed to Client_set_modem_callback()
 * @param response the unrecognized modem response
 * @param len the length of the response
 */
typedef void (*Modem_callback)(void *cookie, const char* response, uint16_t len);

/**
 * Set the function that will be called when the modem receives an
 * unrecognized response.
 * @param self this instance of modem transport
 * @param callback the Modem_callback function
 * @param cookie a caller supplied opaque item passed when callback is called.
 */
void Modem_set_modem_callback(Transport* self, Modem_callback callback, void* cookie);

/**
 * Clear the function that will be called when the transport layer receive an
 * unrecognized response.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept unrecognized responses from the modem.
 * @param self this instance of modem transport
 */
void Modem_clear_modem_callback(Transport* self);

/**
 * Send the line to the modem and wait for an OK response.
 *
 * @param self this instance of modem transport
 * @param line a null-terminated line to send to the modem
 * @param millis the maximum number of milliseconds to run
 * @return an integer status code (success / fail)
 */
TransportResult Modem_send_line(Transport* self, const char* line, uint32_t millis);

#if defined(__cplusplus)
}
#endif

#endif /* INC_CUSTOM_MODEM_TRANSPORT_H */
