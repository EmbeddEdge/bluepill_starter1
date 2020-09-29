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
 * @brief Modem information string
 *
 * This file defines a modem initialisation string used by the
 * Thingstream SDK and is provided for use (or modification) by customers.
 */

#include <modem_transport.h>

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * This string is used to obtain information from the modem once it has been
 * initialised. Each entry in the string is terminated by "\n".
 * If an entry starts with a "?" then the "?" is not passed to the modem, but
 * any ERROR (or +CME ERROR:) produced will be ignored.
 */
const char Thingstream_Modem_informationString[] =
    "AT+CREG?\n"       /* Network Registration (current state) */
    "?AT+CSQ\n"        /* Signal Quality (show current) */
    "?AT+COPS?\n"      /* Operator selection (show current) */
    "?AT+CIMI\n"       /* Request the IMSI */
    "?AT+GMI\n"        /* Request manufacturer identification */
    "?AT+GMM\n"        /* Request TA model identification */
    "?AT+GMR\n"        /* Request TA software release revision */
     ;

#if defined(__cplusplus)
}
#endif
