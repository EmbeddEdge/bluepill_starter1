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
 * @brief Thingstream protocol implementation
 */

#ifndef INC_THINGSTREAM_TRANSPORT_H_
#define INC_THINGSTREAM_TRANSPORT_H_

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create a thingstream protocol transport instance.
 * This wraps MQTT-SN data in a structure identifying the payload to the USSD
 * infrastructure.
 * The caller can provide a buffer to use for this codec, allowing
 * payload lengths larger than the underlying transport by using fragmentation
 * and reassembly. If NULL is passed in, or the buffer length is zero, this
 * codec will use the buffer of the inner transport, thus limiting payload
 * length to the MTU of the transport.
 * @param inner the underlying #ThingstreamTransport instance
 * @param buffer a buffer to use for reassembling inner #ThingstreamTransport packets
 * @param len the length of the provided buffer
 * @return the new #ThingstreamTransport instance
 */
extern ThingstreamTransport* Thingstream_createProtocolTransport(ThingstreamTransport* inner, uint8_t* buffer, uint16_t len);

/**
 * This api allows the default time between USSD sessions to be defined.
 * The call must happen before the call to Thingstream_Client_connect().
 * @param transport the #ThingstreamTransport instance
 * @param delay the delay, in milliseconds, between USSD sessions
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
extern ThingstreamTransportResult Thingstream_Protocol_setUssdSessionDelay(ThingstreamTransport* transport, uint32_t delay);

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated             renamed to Thingstream_createProtocolTransport() */
#define thingstream_transport_create   Thingstream_createProtocolTransport

/** @deprecated                           renamed to Thingstream_Protocol_setUssdSessionDelay() */
#define thingstream_transport_set_ussd_session_delay Thingstream_Protocol_setUssdSessionDelay
/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_THINGSTREAM_TRANSPORT_H_ */
