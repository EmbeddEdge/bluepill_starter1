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
 * @brief Modem initialisation strings
 *
 * This file defines the modem initialisation strings used by the
 * Thingstream SDK and is provided for use (or modification) by customers.
 */

#include <modem_transport.h>

/**
 * This string is used to initialise the modem.
 * Each entry in the string is terminated by "\n".
 * If an entry starts with a "?" then the "?" is not passed to the modem, but
 * any ERROR (or +CME ERROR:) produced will be ignored.
 * An entry of the form ~n inserts a delay of n ms into the sequence.
 */
const char ModemInitString[] =
    "ATZ\n"            /* Reset Default Configuration */
    "~100\n"           /* Wait 100 ms before proceeding */
    "ATE0\n"           /* Set command echo mode (off) */
    "AT+CMEE=2\n"      /* Report Mobile Equipment Error */
    "AT+CREG=2\n"      /* Network Registration (+location) */
    "AT+CUSD=1\n"      /* USSD Enable result codes */
    "?AT+CUSD=2\n"     /* USSD Cancel session (some modems only) */
    "?AT&W\n"          /* Store Active Profile (in case spontaneous restart) */
    "AT+CREG?\n"       /* Network Registration (current state)
                        *    This must follow AT+CREG=2 above for
                        *    the modem transport to finish the
                        *    initialisation.
                        */
     ;

/**
 * This string is used to obtain information from the modem once it has been
 * initialised. Each entry in the string is terminated by "\n".
 * If an entry starts with a "?" then the "?" is not passed to the modem, but
 * any ERROR (or +CME ERROR:) produced will be ignored.
 */
const char ModemInformationString[] =
    "AT+CREG?\n"       /* Network Registration (current state) */
    "?AT+CSQ\n"        /* Signal Quality (show current) */
    "?AT+COPS?\n"      /* Operator selection (show current) */
    "?AT+CIMI\n"       /* Request the IMSI */
    "?AT+GMI\n"        /* Request manufacturer identification */
    "?AT+GMM\n"        /* Request TA model identification */
    "?AT+GMR\n"        /* Request TA software release revision */
     ;

/**
 * This string is used to terminate the current USSD session
 */
const char ModemUssdEndSessionString[] =
    "AT+CUSD=2\n"    /* Terminate the current USSD session */
     ;


/**
 * This string is used to reset the modem after a serious problem has been
 * detected. Each entry in the string is terminated by "\n".
 * If an entry starts with a "?" then the "?" is not passed to the modem, but
 * any ERROR (or +CME ERROR:) produced will be ignored.
 *
 * If the forced reset is successful it will be followed by the commands in
 * ModemInitString[] and ModemInformationString[].
  */
const char ModemForceResetString[] =
    "AT+CFUN=1,1\n"    /* Set Phone Functionality (reset, then full function) */
     ;
