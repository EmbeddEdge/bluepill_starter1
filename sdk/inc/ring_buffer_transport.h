/*
 * Copyright 2019 Thingstream AG
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
 * @brief ThingstreamTransport implementation that adds ring buffering on callbacks
 * e.g. between a serial connection and a modem driver.
 */

#ifndef INC_RING_BUFFER_TRANSPORT_H
#define INC_RING_BUFFER_TRANSPORT_H

#include <transport_api.h>

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Create a transport instance that ring-buffers a sequence of bytes for
 * callbacks.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffer
 * @param size  the size of the data area
 * @return an instance of the ring buffer transport
 */
extern ThingstreamTransport* Thingstream_createRingBufferTransport(ThingstreamTransport* inner, uint8_t* data, uint16_t size);

/**
 * Create a second transport instance that ring-buffers a sequence of bytes
 * for callbacks.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffer
 * @param size  the size of the data area
 * @return an instance of the ring buffer transport
 */
extern ThingstreamTransport* Thingstream_createRingBufferTransport_2(ThingstreamTransport* inner, uint8_t* data, uint16_t size);


/**
 * Create a third transport instance that ring-buffers a sequence of bytes
 * for callbacks.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffer
 * @param size  the size of the data area
 * @return an instance of the ring buffer transport
 */
extern ThingstreamTransport* Thingstream_createRingBufferTransport_3(ThingstreamTransport* inner, uint8_t* data, uint16_t size);


#if defined(__cplusplus)
}
#endif

#endif /* INC_RING_BUFFER_TRANSPORT_H */
