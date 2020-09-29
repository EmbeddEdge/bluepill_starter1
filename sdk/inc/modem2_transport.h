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
 * @ingroup udp
 * @brief A modem driver (implemented as a ThingstreamTransport instance) that
 * supports both USSD and UDP communication.
 *
 * This driver is used with a hardware specific modem configuration.
 * See the @ref udp_modems "list of supported modems" here.

 * Example code initializing the stack and sending a test message:
 *
 * ~~~{.c}
 *
 * #include <stdio.h>
 * #include <string.h>
 * #include <stdlib.h>
 *
 * // add prototype for ThingstreamTransport to send/receive data to/from the
 * // uart connected to the modem hardware.
 * #include "serial_transport.h"
 *
 * // add prototype for chosen UDP hardware configuration
 * #include "sim7600_modem2_config.h"
 *
 * // add prototypes for the Thingtream stack components
 * #include "ring_buffer_transport.h"
 * #include "log_modem_transport.h"
 * #include "base64_codec_transport.h"
 * #include "thingstream_transport.h"
 * #include "log_client_transport.h"
 * #include "client_api.h"
 *
 * // declare some buffers
 * static uint8_t ringBuf[250];
 * static uint8_t modemBuf[MODEM2_UDP_BUFFER_LEN];
 *
 * // A macro to check if an error is reported and print/exit if problem found
 * #define ASSERT(truth)                                      \
 *    do                                                      \
 *    {                                                       \
 *      if (!(truth))                                         \
 *      {                                                     \
 *        printf("ASSERT fail @ %d\n", __LINE__);             \
 *        exit(1);                                            \
 *      }                                                     \
 *    } while (0)
 *
 * // a routine that constructs the Thingstream stack to use UDP protocol and
 * // the Simcom 7600 modem.
 * ThingstreamClient* create_thingstream_stack(uint16_t logFlags,
 *                                             uint16_t modemFlags)
 * {
 *   // create the lowest level transport (note that the arguments to
 *   // serial_transport_create(...) are target specific and are defined
 *   // in the target specific file serial_transport.h)
 *   ThingstreamTransport* transport = serial_transport_create("/dev/ttyS0");
 *   ASSERT(transport != NULL);
 *
 *   // wrap data received from the serial port with a ring buffer.
 *   // The ring buffer can be called from the interrupt service routine of the
 *   // serial driver.
 *   // The buffer must be statically allocated.
 *   transport = Thingstream_createRingBufferTransport(transport,
 *                                                     ringBuf,
 *                                                     sizeof(ringBuf));
 *   ASSERT(transport != NULL);
 *
 *   // wrap modem to/from serial activity with logger if required
 *   if (logFlags != 0)
 *   {
 *     transport = Thingstream_createModemLogger(transport, printf, logFlags);
 *     ASSERT(transport != NULL);
 *   }
 *
 *   // wrap it into a modem transport
 *   // The buffer must be statically allocated and is used to buffer data sent
 *   // over UDP or USSD.
 *   // When using UDP we suggest a buffer size of MODEM2_UDP_BUFFER_LEN (approx 1 Kb);
 *   // when using only USSD, use a buffer size of MODEM2_USSD_BUFFER_LEN.
 *   transport = Thingstream_createModem2Transport(transport, modemFlags,
 *                                                 modemBuf, sizeof(modemBuf),
 *                                                 Thingstream_Simcom7600Init,
 *                                                 printf);
 *   ASSERT(transport != NULL);
 *
 *   // wrap it with base64 encoding (mandatory for USSD, optional for UDP)
 *   transport = Thingstream_createBase64CodecTransport(transport);
 *   ASSERT(transport != NULL);
 *
 *   // wrap Thingstream protocol implementation
 *   // If you wish to send/receive messages larger than the size of modemBuf then
 *   // you should supply an additional buffer to this api.
 *   transport = Thingstream_createProtocolTransport(transport, NULL, 0);
 *   ASSERT(transport != NULL);
 *
 *   // wrap client to/from thingstream activity with logger if required
 *   if (logFlags != 0)
 *   {
 *     transport = Thingstream_createClientLogger(transport, printf, logFlags);
 *     ASSERT(transport != NULL);
 *   }
 *
 *   // create the client utilizing the stack.
 *   ThingstreamClient* client = Thingstream_createClient(transport);
 *   ASSERT(client != NULL);
 *
 *   // initialise the client before use
 *   ThingstreamClientResult cRes = Thingstream_Client_init(client);
 *   ASSERT(cRes == CLIENT_SUCCESS);
 *
 *   return client;
 * }
 *
 *
 * // A test application to create Thingstream stack using UDP transport
 * // protocol and send a test message.
 * int main(int argc, char **argv)
 * {
 *   ThingstreamClient *client = create_thingstream_stack(0xff, 0);
 *   ThingstreamClientResult cRes;
 *   cRes = Thingstream_Client_connect(client, true, 0, NULL);
 *   ASSERT(cRes == CLIENT_SUCCESS);
 *   ThingstreamTopic topic;
 *   cRes = Thingstream_Client_register(client, "Test", &topic);
 *   ASSERT(cRes == CLIENT_SUCCESS);
 *   const char *message = "Hello Thingstream Test";
 *   cRes = Thingstream_Client_publish(client, topic, ThingstreamQOS0, false,
 *                                     (uint8_t*)message, strlen(message));
 *   ASSERT(cRes == CLIENT_SUCCESS);
 *   // Check for any inbound messages
 *   cRes = Thingstream_Client_ping(client);
 *   ASSERT(cRes == CLIENT_SUCCESS);
 * }
 * ~~~
 */

#ifndef INC_MODEM2_TRANSPORT_H
#define INC_MODEM2_TRANSPORT_H

#include <stdbool.h>
#include "modem_transport.h"
#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

struct ThingstreamModem2SharedState_s;
struct ThingstreamModem2UdpConfig_s;

/**
 * Type definition for the hardware specific configuration initialisation
 * routine. When calling Thingstream_createModem2Transport() the application
 * must select the appropriate routine to match the hardware.\n
 * See the @ref udp_modems "list of modem initialisation routines"
 * here.
 *
 * For other modems please contact support@thingstream.io
 */
typedef const struct ThingstreamModem2UdpConfig_s* ThingstreamModem2UdpInit
(
    uint32_t version,
    struct ThingstreamModem2SharedState_s* gState
);

/**
 * Create an instance of the modem transport.
 * @param inner the inner transport instance to use
 * @param flags a set of control flags (e.g to select USSD or UDP protocol)
 * @param buffer the transport buffer to be used by outer transports
 * @param bufSize the size of the transport buffer
 * @param udpConfigInit the initialisation routine for a particular udp modem
 * @param logger the function to use for debug logging
 * @return the modem transport instance
 */
extern ThingstreamTransport* Thingstream_createModem2Transport
(
    ThingstreamTransport* inner,
    uint16_t flags,
    uint8_t* buffer,
    uint16_t bufSize,
    ThingstreamModem2UdpInit udpConfigInit,
    ThingstreamPrintf_t logger
);

/**
 * @name Modem2 Transport flags
 * Flags that can be passed to Thingstream_createModem2Transport()
 */
/**@{*/
/**
 * If MODEM_PREFER_USSD is in the Thingstream_createModem2Transport() flags
 * then the modem driver will prefer to use USSD for the network connection.
 */
#define MODEM_PREFER_USSD       (0x100)

/**
 * If MODEM2_LOG_PARSED is in the flags passed to
 * Thingstream_createModem2Transport() then the modem driver will log items
 * that it successfully parsed from the bytes received from the underlying
 * modem hardware.
 *
 * This flag bit is ignored if the SDK has been built without the matching
 * support.
*/
#define MODEM2_LOG_PARSED       (0x400)

/**
 * If MODEM2_LOG_PARSING is in the flags passed to
 * Thingstream_createModem2Transport() then the modem driver will log parsing
 * steps of bytes received from the underlying modem hardware.
 *
 * Note that this produces a very large amount of logging.
 *
 * This flag bit is ignored if the SDK has been built without the matching
 * support.
 */
#define MODEM2_LOG_PARSING      (0x800)
/**@}*/

/**
 * @private
 * @hideinitializer
 * Space reserved from buffer for parsing modem messages.
 */
#define MODEM2__RESERVED_BUFFER 64

/**
 * Recommended buffer size for ussd-only sessions.
 * @hideinitializer
 */
#define MODEM2_USSD_BUFFER_LEN (THINGSTREAM_USSD_BUFFER_LEN + MODEM2__RESERVED_BUFFER)

/**
 * Recommended buffer size for udp sessions.
 * @hideinitializer
 */
#define MODEM2_UDP_BUFFER_LEN (1000 + MODEM2__RESERVED_BUFFER)

/**
 * Value to be passed to Thingstream_createModem2Transport() for 'udpConfigInit'
 * when using USSD protocol.
 * @hideinitializer
 */
#define Thingstream_UssdInit NULL

/**
 * Send the line to the modem and wait for an OK response.
 * The application can implement Thingstream_Application_modemCallback() to
 * receive any response from the modem caused by the sent line.
 *
 * @param self this instance of modem transport
 * @param line a null-terminated line to send to the modem ("\r\n" will be added)
 * @param millis the maximum number of milliseconds to run
 * @return an integer status code (success / fail)
 */
ThingstreamTransportResult Thingstream_Modem2_sendLine(ThingstreamTransport* self, const char* line, uint32_t millis);

/**
 * Return the number of accumulated +CUSD: errors.
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated +CUSD: errors.
 */
uint32_t Thingstream_Modem2_getCUSDErrors(ThingstreamTransport* self, uint32_t andClear);

/**
 * Return the number of accumulated serious errors (those that trigger
 * a modem reset to recover)
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated serious errors.
 */
uint32_t Thingstream_Modem2_getSeriousErrors(ThingstreamTransport* self, uint32_t andClear);
/**
 * Set the maximum sector size for the UDP transmissions.
 * This must be called before the Thingstring stack is initialised.
 *
 * @param self this Transport instance
 * @param mss the maximum-sector-size for the UDP transmissions
 */
ThingstreamTransportResult Thingstream_Modem2_setBearerMSS(ThingstreamTransport* self, uint16_t mss);

/**
 * This application supplied routine will be called when the modem transport
 * receives binary data. If the application does not provide this routine then
 * the binary data will be sent to the Thingstream_Application_modemCallback().
 *
 * This feature is not supported for all modems, contact support@thingstream.io
 * for more details.
 *
 * @param data the binary data
 * @param len the length of the binary data
 */
void Thingstream_Application_modemBinaryCallback(const uint8_t* data, uint16_t len);

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated renamed to Thingstream_createModem2Transport() */
#define modem2_transport_create     Thingstream_createModem2Transport

/** @deprecated renamed to Thingstream_Modem2_sendLine() */
#define Modem2_send_line            Thingstream_Modem2_sendLine

/** @deprecated renamed to Thingstream_Modem2_getCUSDErrors() */
#define Modem2_get_CUSD_errors      Thingstream_Modem2_getCUSDErrors

/** @deprecated renamed to Thingstream_Modem2_getSeriousErrors() */
#define Modem2_get_serious_errors   Thingstream_Modem2_getSeriousErrors

/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

/**
 * The symbol #Thingstream__dontUseLineBufferTransportWithModem2 is used for
 * link-time checking.
 *
 * If your application gets multiple definition (or duplicate symbol) errors
 * for this symbol then your application may have attempted to create a SDK
 * transport stack with both the modem2 transport and the line buffer transport.
 *
 * These two transports are incompatible so your application should be using
 * the ring buffer transport instead.
 *
 * If your application needs to link both modem2 transport and line buffer
 * transports (e.g. you are using line buffer outside the SDK transport stack)
 * then please use Thingstream_createLineBufferTransport() instead of the
 * legacy line_buffer_transport_create() api.
 *
 * @deprecated
 */
void Thingstream__dontUseLineBufferTransportWithModem2(void);

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM2_TRANSPORT_H */
