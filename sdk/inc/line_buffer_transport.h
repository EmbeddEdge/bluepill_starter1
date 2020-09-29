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
 * @brief ThingstreamTransport implementation that adds line buffering on callbacks
 * e.g. between a serial connection and a modem driver.
 */

#ifndef INC_LINE_BUFFER_TRANSPORT_H
#define INC_LINE_BUFFER_TRANSPORT_H

#include <transport_api.h>

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Create a transport instance that line-buffers a sequence of bytes
 * for callbacks.
 *
 * The callback of this transport can be safely called from interrupt handlers.
 *
 * Do not mix calls to Thingstream_createLineBufferTransport() with calls to
 * either the legacy macro line_buffer_transport_create() or
 * the Thingstream__createLegacyLineBufferTransport() function.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffers
 * @param dataSize  the size of the data area
 * @return an instance of the line buffer transport
 */
extern ThingstreamTransport* Thingstream_createLineBufferTransport(ThingstreamTransport* inner, uint8_t* data, uint32_t dataSize);

/**
 * Create a transport instance (number 2) that line-buffers a sequence of bytes
 * for callbacks.
 *
 * The callback of this transport can be safely called from interrupt handlers.
 *
 * Do not mix calls to Thingstream_createLineBufferTransport_2() with calls to
 * either the legacy macro line_buffer_transport_create() or
 * the Thingstream__createLegacyLineBufferTransport() function.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffers
 * @param dataSize  the size of the data area
 * @return an instance of the line buffer transport
 */
extern ThingstreamTransport* Thingstream_createLineBufferTransport_2(ThingstreamTransport* inner, uint8_t* data, uint32_t dataSize);

/**
 * Create a transport instance (number 3) that line-buffers a sequence of bytes
 * for callbacks.
 *
 * The callback of this transport can be safely called from interrupt handlers.
 *
 * Do not mix calls to Thingstream_createLineBufferTransport_3() with calls to
 * either the legacy macro line_buffer_transport_create() or
 * the Thingstream__createLegacyLineBufferTransport() function.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffers
 * @param dataSize  the size of the data area
 * @return an instance of the line buffer transport
 */
extern ThingstreamTransport* Thingstream_createLineBufferTransport_3(ThingstreamTransport* inner, uint8_t* data, uint32_t dataSize);

/**
 * @private
 * The 'dataSize' argument to Thingstream_createLineBufferTransport() can be
 * encoded to specify the length of the line buffer used for the callback.
 * This helper encodes the two sizes (overall size of the data are and the
 * length of the callback line buffer) in the format required by the
 * Thingstream_createLineBufferTransport() apis.
 *
 * @param dataSize the size of the data area (must be at least twice lineLen)
 * @param lineLen  the size of the callback buffer line
 * @return encoded size to be passed to Thingstream_createLineBufferTransport()
 */
#define Thingstream__lineBufferEncodedSize(dataSize, lineLen) \
  (dataSize | (((uint32_t)lineLen) << 16))

/**
 * @ingroup legacy
 * @deprecated               prefer Thingstream_createLineBufferTransport()
 *
 * Legacy helper for line_buffer_transport_create()
 * This api can be called multiple times to return different instances.
 *
 * Use of the legacy interface uses more text/data/bss than using the preferred
 * Thingstream_createLineBufferTransport() since the legacy api includes all
 * possible instances into the application.
 *
 * @param inner the inner #ThingstreamTransport instance to use
 * @param data  an area of data to use for the buffers
 * @param dataSize  the size of the data area
 * @return an instance of the line buffer transport
 */
extern ThingstreamTransport* Thingstream__createLegacyLineBufferTransport(ThingstreamTransport* inner, uint8_t* data, uint16_t dataSize);

/**
 * @private
 * Deliver any buffered data to the layer above.
 * Normally, this is called only by line_buffer_run(),
 * but for particular projects, the serial transport
 * might invoke it directly. Must not be called from an interrupt
 * handler.
 * @param self the line buffer transport instance
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
extern ThingstreamTransportResult Thingstream_line_buffer_deliver(ThingstreamTransport *self);


/**
 * @private
 * This string is returned by transport->get_client_id() and can be used to
 * identify this transport using simple pointer comparison.
 */
extern const char Thingstream__lineBufferTransportIdentity[];

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @ingroup legacy
 * @deprecated               renamed to Thingstream_createLineBufferTransport() with an extra argument
 */
#define line_buffer_transport_create Thingstream__createLegacyLineBufferTransport


#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_LINE_BUFFER_TRANSPORT_H */
