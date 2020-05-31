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
 * @brief Thingstream protocol implementation
 */

#ifndef INC_THINGSTREAM_TRANSPORT_H_
#define INC_THINGSTREAM_TRANSPORT_H_

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create a thingstream transport instance.
 * This wraps MQTT-SN data in a structure identifying the payload to the USSD
 * infrastructure.
 * The caller can provide a buffer to use for this codec, allowing
 * payload lengths larger than the underlying transport by using fragmentation
 * and reassembly. If NULL is passed in, or the buffer length is zero, this
 * codec will use the buffer of the inner transport, thus limiting payload
 * length to the MTU of the transport.
 * @param inner the underlying transport instance
 * @param buffer a buffer to use for reassembling inner transport packets
 * @param len the length of the provided buffer
 * @return the new Transport instance
 */
extern Transport* thingstream_transport_create(Transport* inner, uint8_t* buffer, uint16_t len);


/**
 * This api allows the default time between USSD sessions to be defined.
 * The call must happen before the call to Client_connect().
 * @param transport the thingstream transport instance
 * @param delay the delay, in milliseconds, between USSD sessions
 * @return an integer status code (success / fail)
 */
extern TransportResult thingstream_transport_set_ussd_session_delay(Transport* transport, uint32_t delay);

#if defined(__cplusplus)
}
#endif

#endif /* INC_THINGSTREAM_TRANSPORT_H_ */
