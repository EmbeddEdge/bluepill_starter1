/*
 * Copyright 2017 Thingstream AG
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
 * @brief Transport implementation that adds line buffering on callbacks
 * e.g. between a serial connection and a modem driver.
 */

#ifndef INC_LINE_BUFFER_TRANSPORT_H
#define INC_LINE_BUFFER_TRANSPORT_H

#include <transport_api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create a line transport instance that line buffers a sequence of bytes for
 * callbacks.
 * The application can create multiple instances of this Transport. However, if
 * an instance wrapping the given inner transport already exists, it will be
 * returned instead of creating a new instance.
 * The number of instances is fixed at library build time, if you need more
 * please contact Thingstream.
 *
 * @param inner the inner transport instance to use
 * @param data  an area of data to use for the buffers
 * @param size  the size of the data area
 * @return an instance of the line transport
 */
extern Transport* line_buffer_transport_create(Transport* inner, uint8_t* data, uint16_t size);

/** @private */
extern Transport* line_buffer_transport_create_ex(Transport* inner, uint8_t* data, uint16_t size, uint16_t bufsize);

/** Deliver any buffered data to the layer above.
 * @private
 * Normally, this is called only by line_buffer_run(),
 * but for particular projects, the serial transport
 * might invoke it directly. Must not be called from an interrupt
 * handler.
 * @param self the Line Transport instance
 * @return an integer status code (success / fail)
 */
TransportResult line_buffer_deliver(Transport *self);

#if defined(__cplusplus)
}
#endif

#endif /* INC_LINE_BUFFER_TRANSPORT_H */
