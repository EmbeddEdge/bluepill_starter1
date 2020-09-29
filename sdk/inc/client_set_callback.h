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
 * @brief The deprecated Thingstream_Client_setXxxxCallback APIs
 */

#ifndef INC_CLIENT_SET_CALLBACK_H_
#define INC_CLIENT_SET_CALLBACK_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"


/**
 * @addtogroup legacy-callback
 * @{
 */

/**
 * Type definition of the "inbound subscribed message" callback
 * @param cookie the cookie passed to Thingstream_Client_setSubscribeCallback()
 * @param topic the #ThingstreamTopic that this message was sent to
 * @param qos the #ThingstreamQualityOfService_t of the message
 * @param payload a pointer to the payload data
 * @param payloadlen the length of the payload
 *
 * @deprecated Alternative Thingstream_Application_subscribeCallback() is
 * preferred
 */
typedef void (*ThingstreamSubscribeCallback_t)(void *cookie, ThingstreamTopic topic, ThingstreamQualityOfService_t qos, uint8_t* payload, uint16_t payloadlen);

/**
 * Set the function that will be called when a new inbound subscription message is received.
 * The callback function will be passed the topic, payload and cookie.
 * @param client the #ThingstreamClient instance
 * @param callback the #ThingstreamSubscribeCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 *
 * @deprecated Alternative Thingstream_Application_subscribeCallback() is
 * preferred
 */
extern void Thingstream_Client_setSubscribeCallback(ThingstreamClient* client, ThingstreamSubscribeCallback_t callback, void* cookie);

/**
 * Clear the subscribe message callback function.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept inbound subscription messages.
 * @param client the #ThingstreamClient instance
 */
#define Thingstream_Client_clearSubscribeCallback(client) \
    Thingstream_Client_setSubscribeCallback((client), NULL, NULL)

/**
 * Type definition of the "inbound register message" callback
 * @param cookie the cookie passed to Thingstream_Client_setRegisterCallback()
 * @param topicName the name of the topic
 * @param topic the #ThingstreamTopic type and id
 *
 * @deprecated Alternative Thingstream_Application_registerCallback() is preferred
 */
typedef void (*ThingstreamRegisterCallback_t)(void *cookie, const char* topicName, ThingstreamTopic topic);

/**
 * Set the function that will be called when an inbound register message is
 * received. The register message is sent by the server when it knows that
 * the client does not have a valid mapping between the topic name and topic id
 * for a message to be published.
 *
 * The callback function will be passed the topic and cookie.
 * @param client the #ThingstreamClient instance
 * @param callback the #ThingstreamRegisterCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 *
 * @deprecated Alternative Thingstream_Application_registerCallback() is preferred
 */
extern void Thingstream_Client_setRegisterCallback(ThingstreamClient* client, ThingstreamRegisterCallback_t callback, void* cookie);

/**
 * Clear the register message callback function.
 * Note that calling this will indicate to the stack that the application no
 * longer wishes to accept inbound register messages.
 * @param client the #ThingstreamClient instance
 */
#define Thingstream_Client_clearRegisterCallback(client)  \
        Thingstream_Client_setRegisterCallback((client), NULL, NULL)

/** @} */

#ifndef THINGSTREAM_NO_SHORT_NAMES
/**
 * @addtogroup legacy
 * @{
 */

/** @deprecated Alias for #ThingstreamRegisterCallback_t */
typedef ThingstreamRegisterCallback_t Register_callback;

/** @deprecated Alias for #ThingstreamSubscribeCallback_t */
typedef ThingstreamSubscribeCallback_t Subscribe_callback;

/** @deprecated              renamed to Thingstream_Client_setSubscribeCallback() */
#define Client_set_subscribe_callback   Thingstream_Client_setSubscribeCallback

/** @deprecated              renamed to Thingstream_Client_clearSubscribeCallback() */
#define Client_clear_subscribe_callback Thingstream_Client_clearSubscribeCallback

/** @deprecated              renamed to Thingstream_Client_setRegisterCallback() */
#define Client_set_register_callback    Thingstream_Client_setRegisterCallback

/** @deprecated             renamed to Thingstream_Client_clearRegisterCallback() */
#define Client_clear_register_callback Thingstream_Client_clearRegisterCallback


/** @} */
#endif /* !THINGSTREAM_NO_SHORT_NAMES */

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_SET_CALLBACK_H_ */
