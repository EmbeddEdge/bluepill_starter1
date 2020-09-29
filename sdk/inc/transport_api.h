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
 * @brief The API describing transport implementations
 *
 * Implementations of this API offer an implementation-specific function for
 * creating an instance of ThingstreamTransport. Note that implementations can choose
 * to return a single, statically allocated instance rather than allowing
 * multiple instances to exist in parallel.
 */

#ifndef INC_TRANSPORT_API_H_
#define INC_TRANSPORT_API_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>

/**
 * A buffer of size THINGSTREAM_USSD_BUFFER_LEN is large enough to receive the
 * biggest packet that the server believes can be handled by typical modems.
 * The normal Thingstream modem transport will also use only this number of
 * bytes in the buffer obtained from the serial (or line buffer transport) so
 * using a larger value in the serial transport would just waste space.
 */
#define THINGSTREAM_USSD_BUFFER_LEN 145


/**
 * ThingstreamTransport layer result codes.
 */
typedef enum ThingstreamTransportResult_e {
    /** Operation completed successfully. */
    TRANSPORT_SUCCESS =  0,
    /** Operation failed with an unspecified error. */
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
    TRANSPORT_END_TIMEOUT = -52,
    /** modem->init() timed out waiting for OK after initial AT command */
    TRANSPORT_INIT_AT_FAILURE = -53,
    /** modem->init() timed out waiting for +CREG:5 */
    TRANSPORT_INIT_CREG5_TIMEOUT = -54,
    /** modem->init() registration refused */
    TRANSPORT_INIT_CREG_REFUSED = -55,
    /** buffer too small */
    TRANSPORT_BUFFER_TOO_SMALL = -56,
    /** modem->init() unable to reset UDP network stack */
    TRANSPORT_INIT_UDP_NETWORK_RESET_FAILED = -57,
    /** modem->init() unable to open the UDP network stack */
    TRANSPORT_INIT_UDP_NETWORK_OPEN_FAILED = -58,
    /** modem->init() unable to attach UDP context to network stack */
    TRANSPORT_INIT_UDP_NETWORK_ATTACH_FAILED = -59,
    /** modem->init() unable to set the required APN in the modem */
    TRANSPORT_INIT_APN_SETUP_FAILED = -60,
    /** modem->init() unable to obtain local IP address */
    TRANSPORT_INIT_UDP_IP_ADDR_FAILED = -61,
    /** modem->init() unable to open UDP connection to the server */
    TRANSPORT_INIT_UDP_CONNECT_FAILED = -62,
    /** modem->init() unable to create a UDP socket */
    TRANSPORT_INIT_UDP_SOCKET_CREATE_FAILED = -63,

    /** Modem operation failed with an unspecified error. */
    TRANSPORT_MODEM_ERROR = -64,
    /** Modem operation failed with an unspecified CME error. */
    TRANSPORT_MODEM_CME_ERROR   = -65,
    /** Modem operation failed and the modem driver forced a soft reset. */
    TRANSPORT_MODEM_FORCED_RESET  = -66,
    /** Modem operation failed with an +CUSD:2 error. */
    TRANSPORT_MODEM_CUSD2_ERROR   = -67,
    /** Modem operation failed with an +CUSD:3 error. */
    TRANSPORT_MODEM_CUSD3_ERROR   = -68,
    /** Modem operation failed with an +CUSD:4 error. */
    TRANSPORT_MODEM_CUSD4_ERROR   = -69,
    /** Modem operation failed with an +CUSD:5 error. */
    TRANSPORT_MODEM_CUSD5_ERROR   = -70,
    /** Modem2 used with line (not ring) buffer transport. */
    TRANSPORT_MODEM2_WITH_LINE_BUFFER_ERROR   = -71,
    /** Modem ussd requires base64 in the transport stack. */
    TRANSPORT_MODEM_USSD_BASE64_ERROR = -72,
} ThingstreamTransportResult;

/**
 * A macro to return true if the given ThingstreamTransportResult is an error
 * that is specific to the modem Transport layer.
 * @param tRes the ThingstreamTransportResult to test
 * @return true if tRes is a modem specific to the modem Transport layer.
 */
#define THINGSTREAM_IS_MODEM_TRANSPORT_ERROR(tRes)                   \
    ((tRes <= TRANSPORT_MODEM_ERROR)                                 \
  && (tRes >= TRANSPORT_MODEM_CUSD5_ERROR))


/**
 * ThingstreamTransport API version. Used for checking at runtime that the API of the
 * transport instance matches the header file used to compile the caller.
 * The size of the transport result is included in order to detect mismatch of enum size
 * between client and SDK (which can cause problems).
 */
#define TRANSPORT_VERSION ((sizeof(ThingstreamTransportResult) << 8) + 6)


/**
 * Type representing a transport instance.
 */
typedef struct ThingstreamTransport ThingstreamTransport;

/**
 * Type definition for the raw message received callback.
 * When the transport-specific code discovers a new message has arrived (either
 * by polling or using an interrupt or similar) the callback function will be
 * called to pass the inbound raw data to the next outermost ThingstreamTransport.
 * @param cookie a pointer to the user supplied cookie
 * @param data a pointer to the data
 * @param len the length of the data
 */
typedef void (*ThingstreamTransportCallback_t)(void* cookie, uint8_t* data, uint16_t len);

/**
 * Opaque type representing the internal state of a transport instance.
 */
typedef struct ThingstreamTransportState_s ThingstreamTransportState_t;

/**
 * Type definition for a transport instance.
 */
struct ThingstreamTransport {
    /**
     * Internal representation of the transport state. For portability,
     * ThingstreamTransport implementations should not access this directly, but rather
     * cast it to their concrete type definitions.
     */
    ThingstreamTransportState_t* _state;

    /**
     * Initialize the transport.
     * This may involve the setup on GPIO, UART ports, interrupts and other
     * platform dependencies.
     * @param version the transport API version (#TRANSPORT_VERSION)
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*init)(ThingstreamTransport* self, uint16_t version);

    /**
     * Shutdown the transport (i.e. the opposite of initialize)
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*shutdown)(ThingstreamTransport* self);

    /**
     * Obtain a buffer to write data into.
     * @param buffer where to write the buffer pointer
     * @param len where the write the buffer length
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*get_buffer)(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len);

    /**
     * Obtain the client ID from the transport.
     * This is useful for cases where the transport implementation can obtain
     * a unique ID, such as reading the IMSI from an attached cellular modem.
     * The returned pointer remains valid until shutdown of the transport.
     * @return the client ID as a C string, never NULL
     */
    const char* (*get_client_id)(ThingstreamTransport* self);

    /**
     * Send the data to the next innermost ThingstreamTransport or hardware device.
     *
     * @param flags an indication of the type of the data, zero is normal.
     * @param data a pointer to the data
     * @param len the length of the raw data
     * @param millis the maximum number of milliseconds to run
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*send)(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);

    /**
     * Register a callback function that will be called when this transport
     * has data to send to its next outermost ThingstreamTransport.
     *
     * @param callback the callback function
     * @param cookie a opaque value passed to the callback function
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*register_callback)(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie);

    /**
     * @deprecated
     * Slot no longer used.
     */
    ThingstreamTransportResult (*unused_slot)(ThingstreamTransport* self);

    /**
     * Allow the transport instance to run for at most the given number of
     * milliseconds.
     * @param millis the maximum number of milliseconds to run (a value of zero
     *        processes all pending operations).
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*run)(ThingstreamTransport* self, uint32_t millis);
};

/** This function is used by a number of transport loggers */
typedef int (*ThingstreamPrintf_t)(const char* format, ...);

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

/** packet being sent would like additional BearerIndicator block
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_WANT_BEARER_INDICATOR    (1U << 11)

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated Alias for #ThingstreamTransport */
typedef ThingstreamTransport Transport;

/** @deprecated Alias for #ThingstreamPrintf_t */
typedef ThingstreamPrintf_t transport_logger;

/** @deprecated Alias for #ThingstreamTransportResult */
typedef ThingstreamTransportResult TransportResult;

/** @deprecated Alias for #ThingstreamTransportCallback_t */
typedef ThingstreamTransportCallback_t Transport_callback;

/** @deprecated Alias for #ThingstreamTransportState_t */
typedef ThingstreamTransportState_t TransportState;

/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_TRANSPORT_API_H_ */
