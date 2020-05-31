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
 * @brief Transport implementation that communicates with a modem through
 * a serial connection.
 */

#ifndef INC_MODEM_TRANSPORT_H
#define INC_MODEM_TRANSPORT_H

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create an instance of the modem transport.
 * @param serial the serial transport instance to use
 * @param flags a set of control flags (e.g to skip initialisation of modem)
 * @return the instance
 */
extern Transport* modem_transport_create(Transport* serial, uint8_t flags);

/** When this flag is passed to modem_transport_create() then the modem
 * transport layer will assume that the modem hardware has already been
 * initialised. This is needed when the other features of the modem hardware
 * are programmed by non-Thingstream software.
 */
#define MODEM_SKIP_INIT  (0x01)


/**
 * The modem will usually talk to the normal (or "live") USSD/Thingstream
 * software server stack, but occasionally the Thingstream support team
 * may request that you use the debugging rather than live stack.
 * If MODEM_STAGE_SHORTCODE is in the modem_transport_create() flags then
 * the modem will use the stage debugging stack.
 */
#define MODEM_STAGE_SHORTCODE  (0x02)


/**
 * The modem allows the AT+CUSD=2 (which denotes the end of a USSD session)
 * to be combined with the last USSD message.
 *
 * This flag and #MODEM_SPLIT_USSD_SESSION_END are mutually exclusive.
 * Do not set both flags.
 * If neither flag is set, then one of them will be chosen during initialisation.
 */
#define MODEM_MERGE_USSD_SESSION_END (0x4)

/**
 * The modem requires the AT+CUSD=2 (which denotes the end of a USSD session)
 * to be split from the last USSD message.
 *
 * This flag and #MODEM_MERGE_USSD_SESSION_END are mutually exclusive.
 * Do not set both flags.
 * If neither flag is set, then one of them will be chosen during initialisation.
 */
#define MODEM_SPLIT_USSD_SESSION_END (0x8)


/**
 * Some modems (e.g. Quectel UG96) return unsolicited "+CUSD: 2" when a USSD
 * session is terminated by the server, but most modems return this for various
 * USSD error conditions.
 * By default the modem  driver will treat an unsolicited +CUSD:2 as an error
 * condition.
 * If the MODEM_IGNORE_PLUS_CUSD2 is in the modem_transport_create() flags then
 * the modem driver will treat +CUSD:2 as informational only.
 */
#define MODEM_IGNORE_PLUS_CUSD2  (0x10)


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
 * @param line a null-terminated line to send to the modem ("\r\n" will be added)
 * @param millis the maximum number of milliseconds to run
 * @return an integer status code (success / fail)
 */
TransportResult Modem_send_line(Transport* self, const char* line, uint32_t millis);

/** See modem_init_string.c */
extern const char ModemInitString[];

/** See modem_init_string.c */
extern const char ModemInformationString[];

/** See modem_init_string.c */
extern const char ModemUssdEndSessionString[];

/** See modem_init_string.c */
extern const char ModemForceResetString[];

/**
 * Return the number of accumulated +CUSD: errors.
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated +CUSD: errors.
 */
uint32_t Modem_get_CUSD_errors(Transport* self, uint32_t andClear);

/**
 * Return the number of accumulated serious errors (those that trigger
 * a modem reset to recover)
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated serious errors.
 */
uint32_t Modem_get_serious_errors(Transport* self, uint32_t andClear);

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_TRANSPORT_H */
