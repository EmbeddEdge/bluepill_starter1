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
 * @brief The API describing transport implementations
 *
 * Implementations of this API offer an implementation-specific function for
 * creating an instance of Transport. Note that implementations can choose
 * to return a single, statically allocated instance rather than allowing
 * multiple instances to exist in parallel.
 */

#ifndef INC_TRANSPORT_API_H_
#define INC_TRANSPORT_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * Transport API version. Used for checking at runtime that the API of the
 * transport instance matches the header file used to compile the caller.
 */
#define TRANSPORT_VERSION 0x0105

/**
 * A buffer of size THINGSTREAM_USSD_BUFFER_LEN is large enough to receive the
 * biggest packet that the server believes can be handled by typical modems.
 * The normal Thingstream modem transport will also use only this number of
 * bytes in the buffer obtained from the serial (or line buffer transport) so
 * using a larger value in the serial transport would just waste space.
 */
#define THINGSTREAM_USSD_BUFFER_LEN 145


/**
 * Transport layer result codes.
 */
typedef enum TransportResult_e {
    /** Transport operation completed successfully. */
    TRANSPORT_SUCCESS =  0,
    /** Transport operation failed with an unspecified error. */
    TRANSPORT_ERROR   = -40,
    /** Transport initialized with wrong version. */
    TRANSPORT_VERSION_MISMATCH = -41,
    /** Transport encountered unexpected data. */
    TRANSPORT_UNEXPECTED_DATA  = -42,
    /** Read operation timed out. */
    TRANSPORT_INIT_TIMEOUT = -43,
    /** Read operation timed out. */
    TRANSPORT_READ_TIMEOUT = -44,
    /** Read operation overflowed the available buffer. */
    TRANSPORT_READ_OVERFLOW = -45,
    /** Send operation timed out. */
    TRANSPORT_SEND_TIMEOUT = -46,
    /** Illegal argument or parameter */
    TRANSPORT_ILLEGAL_ARGUMENT = -47,
    /** Internal protocol error */
    TRANSPORT_INTERNAL_ERROR = -48,
    /** Send operation timed out waiting for ACK. */
    TRANSPORT_ACK_TIMEOUT = -49,
    /** Send operation timed out waiting for ACK between packets */
    TRANSPORT_INTERPACKET_ACK_TIMEOUT = -50,
    /** Send operation timed out waiting for END which had been deferred
     * from a previous ->send() operation.
     */
    TRANSPORT_DEFERRED_END_TIMEOUT = -51,
    /** Send operation timed out waiting for END */
    TRANSPORT_END_TIMEOUT = -52
} TransportResult;

/**
 * Type representing a transport instance.
 */
typedef struct Transport_s Transport;

/**
 * Type definition for the raw message received callback.
 * When the transport-specific code discovers a new message has arrived (either
 * by polling or using an interrupt or similar) the callback function will be
 * called to pass the inbound raw data to the next outermost Transport.
 * @param cookie a pointer to the user supplied cookie
 * @param data a pointer to the data
 * @param len the length of the data
 */
typedef void (*Transport_callback)(void* cookie, uint8_t* data, uint16_t len);

/**
 * Opaque type representing the internal state of a transport instance.
 */
typedef struct TransportState_s TransportState;

/**
 * Type definition for a Transport instance.
 */
struct Transport_s {
    /**
     * Internal representation of the transport state. For portability,
     * Transport implementations should not access this directly, but rather
     * cast it to their concrete type definitions.
     */
    TransportState* _state;

    /**
     * Initialize the transport.
     * This may involve the setup on GPIO, UART ports, interrupts and other
     * platform dependencies.
     * @param version the transport API version (#TRANSPORT_VERSION)
     * @return an integer status code (success / fail)
     */
    TransportResult (*init)(Transport* self, uint16_t version);

    /**
     * Shutdown the transport (i.e. the opposite of initialize)
     * @return an integer status code (success / fail)
     */
    TransportResult (*shutdown)(Transport* self);

    /**
     * Obtain a buffer to write data into.
     * @param buffer where to write the buffer pointer
     * @param len where the write the buffer length
     * @return an integer status code (success / fail)
     */
    TransportResult (*get_buffer)(Transport* self, uint8_t** buffer, uint16_t* len);

    /**
     * Obtain the client ID from the transport.
     * This is useful for cases where the transport implementation can obtain
     * a unique ID, such as reading the IMSI from an attached cellular modem.
     * The returned pointer remains valid until shutdown of the transport.
     * @return the client ID as a C string, never NULL
     */
    const char* (*get_client_id)(Transport* self);

    /**
     * Send the data to the next innermost Transport or hardware device.
     *
     * @param flags an indication of the type of the data, zero is normal.
     * @param data a pointer to the data
     * @param len the length of the raw data
     * @param millis the maximum number of milliseconds to run
     * @return an integer status code (success / fail)
     */
    TransportResult (*send)(Transport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);

    /**
     * Register a callback function that will be called when the this transport
     * has data to send to its next outermost Transport.
     *
     * @param callback the callback function
     * @param cookie a opaque value passed to the callback function
     * @return an integer status code (success / fail)
     */
    TransportResult (*register_callback)(Transport* self, Transport_callback callback, void* cookie);

    /** Deregister the callback function
     * @return an integer status code (success / fail)
     */
    TransportResult (*deregister_callback)(Transport* self);

    /**
     * Allow the transport instance to run for at most the given number of
     * milliseconds.
     * @param millis the maximum number of milliseconds to run (a value of zero
     *        processes all pending operations).
     * @return an integer status code (success / fail)
     */
    TransportResult (*run)(Transport* self, uint32_t millis);
};

/** This function is used by a number of transport loggers */
typedef int (*transport_logger)(const char* format, ...);

/* And these trace log bits are passed to the logger create apis */
/** enable tracing */
#define TLOG_TRACE    (1<<0)
/** enable verbose tracing */
#define TLOG_VERBOSE  (1<<1)
/** enable protocol tracing */
#define TLOG_PROTOCOL (1<<2)
/** add timestamps to start of log lines */
#define TLOG_TIME     (1<<3)

/* The TSEND_xxx bits can be used to pass extra info with transport->send(). */

/** packet being sent requires additional UserAgent block
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_NEED_USERAGENT     (1U << 15)

/** a USSD disconnect is needed
 * (valid for use between thingstream, base64 and modem transports only).
 */
#define TSEND_USSD_SESSION_END   (1U << 14)

/** Used with a zero length send to flush the transport stack and process
 * any pending delays.
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_JUST_FLUSH         (1U << 13)

/** packet being sent would like additional GsmBearer block if the information
 * is available.
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_WANT_GSM_BEARER    (1U << 12)

#if defined(__cplusplus)
}
#endif

#endif /* INC_TRANSPORT_API_H_ */
