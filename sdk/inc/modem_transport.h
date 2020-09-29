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
 * @brief ThingstreamTransport implementation that communicates with a modem through
 * a serial connection.
 */

#ifndef INC_MODEM_TRANSPORT_H
#define INC_MODEM_TRANSPORT_H

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/** When this flag is passed to Thingstream_createModemTransport() then the modem
 * transport layer will assume that the modem hardware has already been
 * initialised. This is needed when the other features of the modem hardware
 * are programmed by non-Thingstream software.
 */
#define MODEM_SKIP_INIT  (0x01)


/**
 * The modem will usually talk to the normal (or "live") USSD/Thingstream
 * software server stack, but occasionally the Thingstream support team
 * may request that you use the debugging rather than live stack.
 * If MODEM_STAGE_SHORTCODE is in the Thingstream_createModemTransport() flags then
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
 * If the MODEM_IGNORE_PLUS_CUSD2 is in the Thingstream_createModemTransport() flags then
 * the modem driver will treat +CUSD:2 as informational only.
 */
#define MODEM_IGNORE_PLUS_CUSD2  (0x10)


/**
 * This application supplied routine will be called when the modem transport
 * receives an unexpected response.
 * @ingroup porting-application
 * @param response the unrecognized modem response
 * @param len the length of the response
 */
void Thingstream_Application_modemCallback(const char* response, uint16_t len);


/** See modem_init_string.c */
extern const char Thingstream_Modem_initString[];

/** See modem_init_string.c */
extern const char Thingstream_Modem_informationString[];

/** See modem_init_string.c */
extern const char Thingstream_Modem_ussdEndSessionString[];

/** See modem_init_string.c */
extern const char Thingstream_Modem_clearFplmnString[];

/** See modem_init_string.c */
extern const char Thingstream_Modem_forceResetString[];

/** For compatibility include the original USSD only modem definitions */
#include "modem_ussd_transport.h"

/** To simplify the user's source include the new USSD/UDP definitions */
#include "modem2_transport.h"

/* Include backward compatibility APIs to set callbacks */
#include "modem_set_callback.h"

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_TRANSPORT_H */
