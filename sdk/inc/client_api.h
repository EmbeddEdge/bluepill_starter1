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
 * @brief The Thingstream Client API
 */

#ifndef INC_CLIENT_API_H_
#define INC_CLIENT_API_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"
#include "sdk_data.h"

/**
 * Enumeration of client result values
 */
typedef enum ThingstreamClientResult_e
{
    /** The successful client API response */
    CLIENT_SUCCESS                        = 0,
    /** @private The current command is being retried (async API only) */
    CLIENT_COMMAND_RETRY                  = -1,
    /** @private The current command is being processed (async API only) */
    CLIENT_COMMAND_IN_PROGRESS            = -2,
    /** The data supplied to Thingstream_Client_publish() is too long to be
     * transmitted.
     */
    CLIENT_PUBLISH_TOO_LONG               = -3,

    /** The Thingstream_Client_xxx() API timed out while waiting for the API
     * to complete.
     */
    CLIENT_OPERATION_TIMED_OUT            = -5,
    /** The server rejected the domainKey that was passed to
     * Thingstream_Client_connect()
     */
    CLIENT_DOMAIN_KEY_INVALID             = -6,
    /** @deprecated renamed to #CLIENT_DOMAIN_KEY_INVALID */
    CLIENT_CLIENT_ID_INVALID = CLIENT_DOMAIN_KEY_INVALID,
    /** The client API could not be completed as the client is not connected.\n
     * The application should call Thingstream_Client_connect() to establish a
     * connection with the server.
     */
    CLIENT_NOT_CONNECTED                  = -10,
    /** A feature is not implemented in this version of the SDK */
    CLIENT_FEATURE_NOT_IMPLEMENTED        = -11,
    /** The client API was supplied with an illegal argument */
    CLIENT_ILLEGAL_ARGUMENT               = -12,
    /** The MQTTSN packet received from the server could not be decoded */
    CLIENT_MQTTSN_DECODE_ERROR            = -13,
    /** The server reports that the topic was invalid (e.g. contains illegal
     * characters or no predefined topic with that alias was found).
     */
    CLIENT_TOPIC_INVALID                  = -14,
    /** The server reports severe congestion and requests a later retry */
    CLIENT_CONGESTION                     = -15,
    /** The SDK state machine received responses that do not match with
     * expected state transitions.
     */
    CLIENT_WRONG_STATE                    = -16,
    /** Thingstream_Client_connect() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_CONNECT_BAD_ACK                = -18,
    /** Thingstream_Client_register() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_REGISTER_BAD_ACK               = -19,
    /** Thingstream_Client_publish() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_PUBLISH_BAD_ACK                = -20,
    /** Thingstream_Client_subscribeName() or
     * Thingstream_Client_subscribeTopic() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_SUBSCRIBE_BAD_ACK              = -21,
    /** Thingstream_Client_unsubscribeName() or
     * Thingstream_Client_unsubscribeTopic() received an unexpected response
     * when waiting for acknowledgment.
     */
    CLIENT_UNSUBSCRIBE_BAD_ACK            = -22,

    /** The information requested is not available */
    CLIENT_INFORMATION_NOT_AVAILABLE      = -23,

    /** @internal
     * Add extra ClientResult error values before
     * CLIENT_UNKNOWN_TRANSPORT_ERROR which must be the last CLIENT_XXX error.
     */

    /** The Thingstream_Client_xxx() APIs can also return
     * #ThingstreamTransportResult error values.\n
     * All returned #ThingstreamTransportResult values will be greater than
     * #CLIENT_MAX_ERROR and less than or equal to
     * #CLIENT_UNKNOWN_TRANSPORT_ERROR
     */
    CLIENT_UNKNOWN_TRANSPORT_ERROR          = -39,

    /** All error values from Thingstream_Client_xxx() APIs will be greater
     * than #CLIENT_MAX_ERROR and less than #CLIENT_SUCCESS
     */
    CLIENT_MAX_ERROR                      = -127
} ThingstreamClientResult;

/**
 * Opaque type definition for a client instance.
 */
typedef struct ThingstreamClient_s ThingstreamClient;


/**
 * @enum TopicType
 *
 * The MQTTSN type of a topic defines the interpretation of the 16-bit topicId
 * field in the ThingstreamTopic structure.
 */
enum TopicType
{
    /** a normal topic type uses a 16-bit topicId which is obtained by
     * converting the topic name with the Thingstream_Client_register() API.
     */
    topicTypeNormal,

    /** a predefined topic type uses a 16-bit topicId which has been defined
     * via the Thingstream Management Console Topics page.
     */
    topicTypePredefined,

    /** a short topic type uses a two character name stored in the 16-bits of
     * the topicId.
     */
    topicTypeShort
};

/**
 * Structure definition for a ThingstreamTopic.
 * This holds the topic type::id pair and can be one of
 *   - registered topic #topicTypeNormal\::NNNN
 *   - predefined topic #topicTypePredefined\::PPPP
 *   - short-code topic #topicTypeShort\::AABB
 */
typedef struct ThingstreamTopic_s {
    /** The type of the topic (this is actually of type enum #TopicType) */
    uint16_t topicType;
    /** The identifier of the topic */
    uint16_t topicId;
} ThingstreamTopic;

/** Initialise predefined ThingstreamTopic with given Id
 *
 * Usage: Topic my_topic = MAKE_PREDEFINED_TOPIC(id)
 *
 * On gcc, it can also be used as a literal, with a cast.
 * Eg  Thingstream_Client_publish(client, (ThingstreamTopic)MAKE_PREDEFINED_TOPIC(id), ...)
 */
#define MAKE_PREDEFINED_TOPIC(id)  {              \
     .topicType = (uint16_t) topicTypePredefined, \
     .topicId = id                                \
}
/** Initialise short ThingstreamTopic with given character pair.
 * Usage: Topic my_topic = MAKE_SHORT_TOPIC('x', 'y')
 *
 * On gcc, it can also be used as a literal, with a cast.
 * Eg  Thingstream_Client_publish(client, (ThingstreamTopic)MAKE_SHORT_TOPIC('x', 'y'), ...)
 */
#define MAKE_SHORT_TOPIC(first, second)     {   \
     .topicType = (uint16_t) topicTypeShort,    \
     .topicId = ( (first) << 8) | (second)      \
}

/**
 * The default connection timeout used by
 * Thingstream_Client_connect() when 0 is passed as
 * the keepAlive value.
 */
#define THINGSTREAM_DEFAULT_CONNECT_KEEPALIVE 360

/**
 * The quality of service (QoS) assigned to a message.
 * There are three levels of QoS (0, 1, 2) which can be used while
 * connected for subscribe or publish. In addition,
 * QoS -1 is available and is equivalent to
 * QoS 0, but can be used to publish messages with #topicTypeShort or
 * #topicTypePredefined without connecting.
 */
typedef enum ThingstreamQualityOfService_e
{
    /** Fire and forget without the need to be connected (publish only) */
    ThingstreamQOSM1 = -1,
    /** Fire and forget - the message may not be delivered */
    ThingstreamQOS0 = 0,
    /** At least once - the message may be delivered more than once */
    ThingstreamQOS1 = 1,
    /** Once and only once - the message will be delivered exactly once */
    ThingstreamQOS2 = 2
} ThingstreamQualityOfService_t;



/**
 * This API is called by the SDK when the server sends the Client a message.
 * The application should define this routine if it wishes to receive inbound
 * messages.
 * @ingroup porting-application
 * @param topic the #ThingstreamTopic that this message was sent to
 * @param qos the #ThingstreamQualityOfService_t of the message
 * @param payload a pointer to the payload data
 * @param payloadlen the length of the payload
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic, ThingstreamQualityOfService_t qos, uint8_t* payload, uint16_t payloadlen);

/**
 * This API is called by the SDK when the server sends the Client a mapping
 * between the name of a topic and the topic ID. This mapping is sent just prior
 * to the first message of that topic during the current connection.
 *
 * @ingroup porting-application
 * @param topicName the name of the topic
 * @param topic the #ThingstreamTopic that matches the topicName
 */
void Thingstream_Application_registerCallback(const char* topicName, ThingstreamTopic topic);

/**
 * This API is called by the SDK when the server sends a disconnect message.
 * This result is usually also made available via the return code of a
 * client call, but a publish with quality #ThingstreamQOS0, no reply
 * from the server is expected.
 *
 * @ingroup porting-application
 */
void Thingstream_Application_disconnectCallback(void);

/**
 * Create a new client
 *
 * @param transport the #ThingstreamTransport instance
 * @return a pointer to a #ThingstreamClient instance, or NULL if the client instance
 *         cannot be created
 */
extern ThingstreamClient* Thingstream_createClient(ThingstreamTransport * transport);

/**
 * Destroy the client
 *
 * Currently a no-op
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
#define Thingstream_Client_destroy(client) ((void)(client), CLIENT_SUCCESS)

/**
 * Initialise anything that needs initialising, etc
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_init(ThingstreamClient* client);

/**
 * Shut down the client, free any resources etc.
 * After this call the application is free to turn off the modem.
 * Before using any more Thingstream_Client_xxx() APIs, the application must call
 * Thingstream_Client_init() again to re-initialise the #ThingstreamTransport stack.
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_shutdown(ThingstreamClient* client);

/**
 * Connect to the server.
 *
 * @param client the #ThingstreamClient instance
 * @param cleanSession set non-zero if the client wants a clean MQTT session
 * @param keepAlive the duration (in minutes) that the server will keep the
 *        connection active after receiving a message from the client.
 *        A value of zero selects #THINGSTREAM_DEFAULT_CONNECT_KEEPALIVE.
 * @param domainKey a unique identifier for the client (use NULL to obtain
 *        a value from the transport layer).
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_connect(ThingstreamClient* client, bool cleanSession, uint16_t keepAlive, const char *domainKey);

/**
 * Either enter a sleeping state, or disconnect from the server.
 * The server will persist all current subscriptions, which may then be
 * cleared out by the server on the next "cleanSession" connect.
 * @param client the #ThingstreamClient instance
 * @param duration if non-zero the number of minutes that the server should
 *        treat the client as sleeping. During this time, Thingstream_Client_ping()
 *        can be used to retrieve messages and refresh the timeout.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_disconnect(ThingstreamClient* client, uint16_t duration);


/**
 * Test if the server has indicated that there are queued messages waiting on
 * the server for this device.
 *
 * The server sends extra data when executing the APIs
 * Thingstream_Client_publish(), Thingstream_Client_connect(), or
 * Thingstream_Client_ping() and this is remembered by the Client SDK.
 * The extra data is sent by the server at the start of the publish and at the
 * end of the connect or the ping.
 *
 * This stored number of queued messages can be queried using
 * Thingstream_Client_messageWaiting()
 *
 * @param client the #ThingstreamClient instance (not currently used, but
 *        may be required in future).
 * @return
 *    <b>>= 0</b>: the number of queued messages on the server at the time of
 *          the previous publish, ping or connect.
 * @return
 *    #CLIENT_INFORMATION_NOT_AVAILABLE: the Client SDK failed to receive the
 *          expected extra data with the last publish, connect or ping.
 *          E.g. If the device has never previously connected to (or pinged) the
 *          server then the server is unable to send this information with the
 *          publish.
 */
#define Thingstream_Client_messageWaiting(client)                \
    ((ThingstreamClientResult)SDK_DATA_INBOUND_QUEUE(size))


/**
 * Perform an MQTT-SN ping, flushing pending subscribed messages and refreshing
 * the keepAlive or sleep timer.
 *
 * The Ping Flush setting on the Thingstream Management Console may cause the
 * server to complete the ping request before all pending messages have been
 * sent to the device. If this is important to the application then the
 * Thingstream_Client_messageWaiting() API can be used to identify this early
 * return case.
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 * @return
 *    #CLIENT_SUCCESS:       the command completed normally
 * @return
 *    #CLIENT_NOT_CONNECTED: unable to retrieve messages, the application must
 *                          reconnect to the server first.
 *
 *    other negative results indicate some other error condition.
 */
extern ThingstreamClientResult Thingstream_Client_ping(ThingstreamClient* client);

/**
 * Register the named topic by asking the gateway for the topicId
 *
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to be registered
 * @param pOutTopic a pointer to the #ThingstreamTopic to receive the topic type and id.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_register(ThingstreamClient* client, const char* topicName, ThingstreamTopic *pOutTopic);


/**
 * Send a message to the given topic.
 * @param client the #ThingstreamClient instance
 * @param topic the #ThingstreamTopic (type and id) to publish onto
 * @param qos the #ThingstreamQualityOfService_t required
 * @param retained if true, server will keep message for future subscribers
 * @param payload a pointer to the payload
 * @param payloadlen the length of the payload
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_publish(ThingstreamClient* client, ThingstreamTopic topic, ThingstreamQualityOfService_t qos, bool retained, uint8_t* payload, uint16_t payloadlen);

/**
 * Subscribe to receive messages that are published on the named topic.
 * When messages arrive the Thinsgtream_Application_subscribeCallback() will
 * be called with the topic, QoS, and message payload.
 *
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to subscribe to (can be wild-carded)
 * @param qos the #ThingstreamQualityOfService_t required
 * @param pOutTopic a pointer (may be NULL) to the ThingstreamTopic to receive the topic
 *        type and id of the named topic.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_subscribeName(ThingstreamClient* client, const char* topicName, ThingstreamQualityOfService_t qos, ThingstreamTopic* pOutTopic);

/**
 * Subscribe to receive messages that are published on a topic.
 * When messages arrive the Thinsgtream_Application_subscribeCallback() will
 * be called with the topic, QoS, and message payload.
 *
 * @param client the #ThingstreamClient instance
 * @param topic the type and id of the #ThingstreamTopic to subscribe to
 * @param qos the #ThingstreamQualityOfService_t required
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_subscribeTopic(ThingstreamClient* client, ThingstreamTopic topic, ThingstreamQualityOfService_t qos);

/**
 * Unsubscribe and stop receiving messages published to a topic.
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to unsubscribe from
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_unsubscribeName(ThingstreamClient* client, const char *topicName);

/**
 * Unsubscribe and stop receiving messages published to a topic.
 * @param client the #ThingstreamClient instance
 * @param topic the type and id of the #ThingstreamTopic to unsubscribe from
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_unsubscribeTopic(ThingstreamClient* client, ThingstreamTopic topic);

/**
 * Process any outstanding tasks that are waiting. Registered callbacks will
 * only be called from within this function.
 * @param client the #ThingstreamClient instance
 * @param waitMs the time in milliseconds to wait for things to happen,
 *        treat zero as "the smallest time that allows pending things to happen"
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_run(ThingstreamClient* client, uint32_t waitMs);


/**
 * Translate an error code into a concise representative string.
 * @param result a #ThingstreamClientResult code
 * @return text describing the result
 */
extern const char *Thingstream_Client_getErrorText(ThingstreamClientResult result);


/**
 * This string matches the Thingstream version e.g. BLD1234-v5.6
 */
extern const char Thingstream_Client_versionString[];


/**
 * This application supplied routine will be called when the Thingstream SDK
 * receives a time packet from the server.
 *
 * The server currently sends a time packet is response to the device calling
 *    Thingstream_Client_ping()
 *    Thingstream_Client_connect()
 *    Thingstream_Client_publish()
 * but time packets may be sent more frequently in the future.
 *
 * @ingroup porting-application
 * @param unixEpochTime the number of seconds since 00:00:00 UTC on 1 Jan 1970
 */
void Thingstream_Application_serverTimeCallback(uint32_t unixEpochTime);

#ifndef THINGSTREAM_NO_SHORT_NAMES

/**
 * @addtogroup legacy-api
 * @{
 */

/** @deprecated Alias for #ThingstreamClient */
typedef ThingstreamClient Client;

/** @deprecated Alias for #ThingstreamTopic */
typedef ThingstreamTopic  Topic;

/** @deprecated Alias for #ThingstreamClientResult */
typedef ThingstreamClientResult ClientResult;

/** @deprecated Alias for #ThingstreamQualityOfService_t */
typedef ThingstreamQualityOfService_t QOS;

/** @deprecated Alias for #ThingstreamQOS0 */
#define MQTT_QOS0          ThingstreamQOS0

/** @deprecated Alias for #ThingstreamQOS1 */
#define MQTT_QOS1          ThingstreamQOS1

/** @deprecated Alias for #ThingstreamQOS2 */
#define MQTT_QOS2          ThingstreamQOS2

/** @deprecated Alias for #ThingstreamQOSM1 */
#define MQTTSN_QOSM1       ThingstreamQOSM1

/**
 * The duration (in minutes) that the server will consider the connection
 * active after receiving a message from the client.
 * @deprecated This is used by the compatabilty macro Client_connect() but
 * not when Thingstream_Client_connect() is used.
 */
extern uint16_t Thingstream_Client_connectionTimeout;

/**
 * @private
 * A place to store the domain key between Client_create() and Client_connect()
 */
extern char Thingstream_Client__domainKey[];

/**
 * @private implementation of the legacy Client_create()
 */
extern ThingstreamClient* Thingstream__deprecatedCreateClient(ThingstreamTransport * transport, const char* domainKey);

/** @deprecated
 * Split into two APIs Thingstream_createClient() and Thingstream_Client_init()
 */
#define Client_create      Thingstream__deprecatedCreateClient


/** @deprecated
 * Split into two APIs Thingstream_Client_shutdown() and Thingstream_Client_destroy()
 * but since the latter is currently a no-op, this is just mapped to the former.
 */
#define Client_destroy     Thingstream_Client_shutdown

/** @deprecated renamed to Thingstream_Client_disconnect() */
#define Client_disconnect  Thingstream_Client_disconnect

/** @deprecated renamed to Thingstream_Client_ping() */
#define Client_isConnected Thingstream_Client_ping

/** @deprecated renamed to Thingstream_Client_register() */
#define Client_register    Thingstream_Client_register

/** @deprecated       renamed to Thingstream_Client_subscribeName() */
#define Client_subscribeName     Thingstream_Client_subscribeName

/** @deprecated       renamed to Thingstream_Client_subscribeTopic() */
#define Client_subscribeTopic    Thingstream_Client_subscribeTopic

/** @deprecated       renamed to Thingstream_Client_unsubscribeName() */
#define Client_unsubscribeName   Thingstream_Client_unsubscribeName

/** @deprecated       renamed to Thingstream_Client_unsubscribeTopic() */
#define Client_unsubscribeTopic  Thingstream_Client_unsubscribeTopic

/** @deprecated       renamed to Thingstream_Client_run() */
#define Client_run               Thingstream_Client_run

/** @deprecated       renamed to Thingstream_Client_getErrorText() */
#define Client_getErrorText      Thingstream_Client_getErrorText

/** @deprecated
 * Renamed to Thingstream_Client_connect() with
 * additional parameters to set the timeout (in place of #Client_ConnectionTimeout),
 * and to take the domain key (previously passed to Client_create())
 * The callback and cookie parameters should instead be passed to a separate call to
 * Thingstream_Client_setRegisterCallback().
 */
#define Client_connect(client, clean, callback, cookie) \
    ( Thingstream_Client_setRegisterCallback((client), (callback), (cookie)), \
      Thingstream_Client_connect((client), (clean), Thingstream_Client_connectionTimeout, Thingstream_Client__domainKey) )

/** @deprecated renamed to Thingstream_Client_publish() without the final parameter */
#define Client_publish(client, topic, qos, retained, payload, payloadlen, unused) \
    Thingstream_Client_publish((client), (topic), (qos), (retained), (payload), (payloadlen))

/**
 * @deprecated This is used by the compatabilty macro Client_connect() but
 * not when Thingstream_Client_connect() is used.
 */
#define Client_ConnectionTimeout Thingstream_Client_connectionTimeout

/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

/* Include backward compatibility APIs to set callbacks */
#include "client_set_callback.h"

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_API_H_ */
