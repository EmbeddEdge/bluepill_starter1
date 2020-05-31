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
 * @brief The Thingstream Client API
 */

#ifndef INC_CLIENT_API_H_
#define INC_CLIENT_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"

/**
 * Enumeration of client result values
 */
typedef enum ClientResult_e
{
    CLIENT_SUCCESS                        = 0,
    CLIENT_COMMAND_RETRY                  = -1,
    CLIENT_COMMAND_IN_PROGRESS            = -2,
    CLIENT_PUBLISH_TOO_LONG               = -3,
    CLIENT_OPERATION_TIMED_OUT            = -5,
    CLIENT_CLIENT_ID_INVALID              = -6,
    CLIENT_NOT_CONNECTED                  = -10,
    CLIENT_FEATURE_NOT_IMPLEMENTED        = -11,
    CLIENT_ILLEGAL_ARGUMENT               = -12,
    CLIENT_MQTTSN_DECODE_ERROR            = -13,
    CLIENT_TOPIC_INVALID                  = -14,
    CLIENT_CONGESTION                     = -15,
    CLIENT_WRONG_STATE                    = -16,
    CLIENT_RX_PACKET_DISCARDED            = -17,
    CLIENT_CONNECT_BAD_ACK                = -18,
    CLIENT_REGISTER_BAD_ACK               = -19,
    CLIENT_PUBLISH_BAD_ACK                = -20,
    CLIENT_SUBSCRIBE_BAD_ACK              = -21,
    CLIENT_UNSUBSCRIBE_BAD_ACK            = -22,
    /* Add extra error values here */
    CLIENT_UNKNOWN_TRANSPORT_ERROR        = -39, /* Must be last CLIENT error */
    /* Values up to -127 are supplied by TransportResult */
    CLIENT_MAX_ERROR                      = -127
} ClientResult;

/**
 * Opaque type definition for a Client instance.
 */
typedef struct Client_s Client;


/**
 * @enum TopicType
 *
 * The MQTTSN type of a topic defines the interpretation of the 16-bit topicId
 * field in the Topic structure.
 */
enum TopicType
{
    /** a normal topic holds the 16-bit topicId for a publish, and a topic name
     * with topicId equal to zero for a subscribe.
     */
    topicTypeNormal,

    /** a predefined topic type uses a 16-bit topicId in all cases. */
    topicTypePredefined,

    /** a short topic type uses a two character name stored in the 16-bits of
     * the topicId.
     */
    topicTypeShort
};

/**
 * Structure definition for a Topic.
 * This holds the topic type::id pair and can be either
 *   a) registered topic topicTypeNormal::NNNN
 *   b) pre-regsitered   topicTypePredefined::PPPP
 *   c) short-code       topicTypeShort::AABB
 */
typedef struct Topic_s {
    /** The type of the topic (this is actually of type enum TopicType)  */
    uint16_t topicType;
    /** The identifier of the topic */
    uint16_t topicId;
} Topic;

/* By including the cast, gcc allows these to be used both
 * as an initialiser and as a compound literal
 */
/** Create predefined Topic with given Id */
#define MAKE_PREDEFINED_TOPIC(id)  (Topic) {            \
     .topicType = (uint16_t) topicTypePredefined,       \
     .topicId = id                                      \
}
/** Create short Topic with given character pair */
#define MAKE_SHORT_TOPIC(first, second)  (Topic) {   \
     .topicType = (uint16_t) topicTypeShort,         \
     .topicId = ( (first) << 8) | (second)           \
}

/**
 * The quality of service (QoS) assigned to the message.
 * There are three levels of QoS:
 * <DL>
 * <DT><B>QoS0</B></DT>
 * <DD>Fire and forget - the message may not be delivered</DD>
 * <DT><B>QoS1</B></DT>
 * <DD>At least once - the message will be delivered, but may be
 * delivered more than once in some circumstances.</DD>
 * <DT><B>QoS2</B></DT>
 * <DD>Once and one only - the message will be delivered exactly once.</DD>
 * <DT><B>QoS-1</B></DT>
 * <DD>Just publish (no connect, subscribe, register, nor acks)</DD>
 * </DL>
 */
typedef enum QOS_e {MQTT_QOS0, MQTT_QOS1, MQTT_QOS2, MQTTSN_QOSM1} QOS;

/**
 * Type definition of the subscribed message arrived callback
 * @param cookie the cookie passed to Client_set_subscribe_callback()
 * @param topic the topic that this message was sent to
 * @param qos the QualityOfService of the message
 * @param payload a pointer to the payload data
 * @param payloadlen the length of the payload
 */
typedef void (*Subscribe_callback)(void *cookie, Topic topic, QOS qos, uint8_t* payload, uint16_t payloadlen);

/**
 * Type definition of the register message arrived callback
 * @param cookie the cookie passed to Client_connect()
 * @param topicName the name of the topic
 * @param topic the topic type and id
 */
typedef void (*Register_callback)(void *cookie, const char* topicName, Topic topic);

/**
 * Create a new client, initialise anything that needs initialising, etc
 *
 * @param transport the Transport instance
 * @param domainKey unique identifier for the client
 * @return a pointer to a Client instance, or NULL if the client instance
 *         cannot be created due to network not being present
 */
extern Client* Client_create(Transport * transport, const char* domainKey);

/**
 * Destroy the client, free any resources etc.
 * After this call the application is free to turn off the modem.
 * Before using any more Client_xxx() apis, the application must call
 * Client_create() again to re-initialise the Transport stack.
 *
 * @param client the Client instance
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_destroy(Client* client);

/**
 * Prepare the client to do some real work and send announcement to the server
 * that this device is alive.
 *
 * @param client the Client instance
 * @param cleanSession set non-zero if the client wants a clean MQTT session
 * @param callback if non-NULL and cleanSession is non-zero then the supplied callback will be called with any Topic that was registered in a previous session.
 * @param cookie a caller supplied opaque item passed when callback is called.
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_connect(Client* client, bool cleanSession, Register_callback callback, void* cookie);


/**
 * Shut down the client and turn things off. The server will persist all current
 * subscriptions, which may then be cleared out by the server on the next connect
 * operation.
 * @param client the Client instance
 * @param duration if non-zero the number of minutes that the server should
 *        treat the client as sleeping and buffer any publish messages until
 *        client awakes.
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_disconnect(Client* client, uint16_t duration);

/**
 * Return the current state of the client.
 * Again this might be a null-op, but if we do end up sending hello/goodbye messages
 * it might be useful
 * @param client the Client instance
 * @return a ClientResult status showing the connection state of the client
 */
extern ClientResult Client_isConnected(Client* client);

/**
 * Register the named topic by asking the gateway for the topicId
 *
 * @param client the Client instance
 * @param topicName the name of the Topic to be registered
 * @param pOutTopic a pointer to the Topic to receive the topic type and id.
 * @return a ClientResult indicating success/fail
 */
extern ClientResult Client_register(Client* client, const char* topicName, Topic *pOutTopic);


/**
 * Send a message to the given topic. The function will split the message up into
 * discrete USSD packets and send them across the network. Should the payload be too large,
 * then this message will fail.
 * @param client the Client instance
 * @param topic the topic (type and id) to publish onto
 * @param qos the QualityOfService required
 * @param retained if true, server will keep message for future subscribers
 * @param payload a pointer to the payload
 * @param payloadlen the length of the payload
 * @param unused an unused parameter which should be passed NULL. (Will be removed from a future release.)
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_publish(Client* client, Topic topic, QOS qos, bool retained, uint8_t* payload, uint16_t payloadlen, void* unused);

/**
 * Set the function that will be called when a new inbound subscription message is received. The callback function will be passed the topic and payload.
 * @param client the Client instance
 * @param callback the Subscribe_callback function
 * @param cookie a caller supplied opaque item passed when callback is called.
 */
extern void Client_set_subscribe_callback(Client* client, Subscribe_callback callback, void* cookie);

/**
 * Clear the subscribe message callback function.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept inbound subscription messages.
 */
extern void Client_clear_subscribe_callback(Client* client);

/**
 * Set the function that will be called when an inbound register message is
 * is received. The register message is sent by the server when it knows that
 * the client does not have a valid mapping between the topic name and topic id
 * for a message to be published.
 *
 * The callback function will be passed the topic and cookie.
 * @param client the Client instance
 * @param callback the Register_callback function
 * @param cookie a caller supplied opaque item passed when callback is called.
 */
extern void Client_set_register_callback(Client* client, Register_callback callback, void* cookie);

/**
 * Clear the register message callback function.
 * Note that calling this will indicate to the stack that the application no
 * longer wishes to accept inbound register messages.
 * @param client the Client instance
 */
#define Client_clear_register_callback(client)  \
        Client_set_register_callback((client), NULL, NULL)

/**
 * Subscribe to receive messages that have been published on a topic. Note that
 * the callback function {@link #Client_set_subscribe_callback()} should be set
 * prior to calling this function.
 * @param client the Client instance
 * @param topicName the name of the topic to subscribe to (can be wild-carded)
 * @param qos the QualityOfService required
 * @param pOutTopic a pointer (may be NULL) to the Topic to receive the topic
 *        type and id of the named topic.
 * @return an integer status code (success / fail)
 */
  extern ClientResult Client_subscribeName(Client* client, const char* topicName, QOS qos, Topic* pOutTopic);

/**
 * Subscribe to receive messages that have been published on a topic. Note that
 * the callback function {@link #Client_set_subscribe_callback()} should be set
 * prior to calling this function.
 * @param client the Client instance
 * @param topic the type and id of the topic to subscribe to
 * @param qos the QualityOfService required
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_subscribeTopic(Client* client, Topic topic, QOS qos);

/**
 * Unsubscribe and stop receiving messages published to a topic. If the caller unsubscribes from
 * all topics, then the implementation can decide to shutdown and inbound resources and even
 * power down transports etc, since there will be no inbound communication
 * @param client the Client instance
 * @param topicName the name of the topic to unsubscribe from
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_unsubscribeName(Client* client, const char *topicName);

/**
 * Unsubscribe and stop receiving messages published to a topic. If the caller unsubscribes from
 * all topics, then the implementation can decide to shutdown and inbound resources and even
 * power down transports etc, since there will be no inbound communication
 * @param client the Client instance
 * @param topic the type and id of the topic to unsubscribe from
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_unsubscribeTopic(Client* client, Topic topic);

/**
 * Process any outstanding tasks that are waiting. Registered callbacks will
 * only be called from within this function.
 * @param client the Client instance
 * @param waitMs the time in milliseconds to wait for things to happen,
 *        treat zero as "the smallest time that allows pending things to happen"
 * @return an integer status code (success / fail)
 */
extern ClientResult Client_run(Client* client, uint32_t waitMs);


/**
 * The duration (in minutes) that the server will consider the connection
 * active after receiving a message from the client.
 * This value is sent to the server during Client_connect().
 */
extern uint16_t Client_ConnectionTimeout;

/**
 * Translate an error code into a concise representative string.
 * @param result a ClientResult code
 * @return text describing the result
 */
extern const char *Client_getErrorText(ClientResult result);

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_API_H_ */
