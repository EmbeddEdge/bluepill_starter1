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

#ifndef INC_MODEM_USSD_TRANSPORT_H
#define INC_MODEM_USSD_TRANSPORT_H

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create an instance of the modem transport.
 * @param inner the inner #ThingstreamTransport instance to use
 * @param flags a set of control flags (e.g to skip initialisation of modem)
 * @return the #ThingstreamTransport instance
 */
extern ThingstreamTransport* Thingstream_createModemUssdTransport(ThingstreamTransport* inner, uint16_t flags);

/**
 * Send the line to the modem and wait for an OK response.
 * The application can implement Thingstream_Application_modemCallback() to
 * receive any response from the modem caused by the sent line.
 *
 * @param self instance of modem transport
 * @param line a null-terminated line to send to the modem ("\r\n" will be added)
 * @param millis the maximum number of milliseconds to run
 * @return a #ThingstreamTransportResult status code (0 => success / fail)
 */
ThingstreamTransportResult Thingstream_ModemUssd_sendLine(ThingstreamTransport* self, const char* line, uint32_t millis);

/**
 * Return the number of accumulated +CUSD: errors.
 *
 * @param self instance of modem transport
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated +CUSD: errors.
 */
uint32_t Thingstream_ModemUssd_getCUSDErrors(ThingstreamTransport* self, uint32_t andClear);

/**
 * Return the number of accumulated serious errors (those that trigger
 * a modem reset to recover)
 *
 * @param self instance of modem transport
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated serious errors.
 */
uint32_t Thingstream_ModemUssd_getSeriousErrors(ThingstreamTransport* self, uint32_t andClear);

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated         renamed to Thingstream_createModemUssdTransport() */
#define modem_transport_create     Thingstream_createModemUssdTransport

/** @deprecated         renamed to Thingstream_ModemUssd_sendLine() */
#define Modem_send_line            Thingstream_ModemUssd_sendLine

/** @deprecated        renamed to #Thingstream_Modem_initString */
#define ModemInitString            Thingstream_Modem_initString

/** @deprecated        renamed to #Thingstream_Modem_informationString */
#define ModemInformationString     Thingstream_Modem_informationString

/** @deprecated        renamed to #Thingstream_Modem_ussdEndSessionString */
#define ModemUssdEndSessionString  Thingstream_Modem_ussdEndSessionString

/** @deprecated        renamed to #Thingstream_Modem_forceResetString */
#define ModemForceResetString      Thingstream_Modem_forceResetString

/** @deprecated         renamed to Thingstream_ModemUssd_getCUSDErrors() */
#define Modem_get_CUSD_errors      Thingstream_ModemUssd_getCUSDErrors

/** @deprecated         renamed to Thingstream_ModemUssd_getSeriousErrors() */
#define Modem_get_serious_errors   Thingstream_ModemUssd_getSeriousErrors
/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_USSD_TRANSPORT_H */
