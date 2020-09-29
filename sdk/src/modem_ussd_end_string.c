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
 * @brief Modem ussd end session string
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
 * This string is used to terminate the current USSD session
 */
const char Thingstream_Modem_ussdEndSessionString[] =
    "AT+CUSD=2\n"    /* Terminate the current USSD session */
     ;


#if defined(__cplusplus)
}
#endif
