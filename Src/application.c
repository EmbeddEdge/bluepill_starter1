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

#include <string.h>

#include "client_api.h"
#include "line_buffer_transport.h"
#include "modem_transport.h"
#include "base64_codec_transport.h"
#include "thingstream_transport.h"
#include "client_platform.h"

#include "debug_printf_core.h"

#include "log_client_transport.h"
#include "log_modem_transport.h"

#include "serial_transport.h"

#include "application.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#define DEBUG_LOG_MODEM 1
#define DEBUG_LOG_CLIENT 1

#define THINGSTREAM_BUFFER_LENGTH 512
static uint8_t thingstream_buffer[THINGSTREAM_BUFFER_LENGTH];

#define LINE_BUFFER_LENGTH 300
static uint8_t line_buffer[LINE_BUFFER_LENGTH];

static uint16_t exampleTopicId = 0; /* will get updated after registration */

#define CHECK(msg, cond) do { \
    debug_printf("%s %s @%d\n", msg, ((cond) ? "ok" : "ERROR"), __LINE__); \
    if (!(cond)) goto error; \
  } while(0)

static bool done = false;
static UART_HandleTypeDef *debug_output;

/* Thingstream required routine, see client_platform.h */
uint32_t Platform_getTimeMillis(void)
{
    return HAL_GetTick();
}

/* Used by debug logging */
void debug_puts(const char* str, int len)
{
    if (debug_output != NULL)
    {
        HAL_UART_Transmit(debug_output, (uint8_t *)str, len, 5000);
    }
}

/* Callback for receiving messages.
 * This will be called from within Client_run()
 */
static void receiveCallback(void *cookie, Topic topic, QOS qos, uint8_t* msg, uint16_t msgLen)
{
    if (msgLen > 0)
    {
        debug_printf("Received message: %s\n", msg);
    }
    else
    {
        debug_printf("Received empty message");
    }
    if (topic.topicId == exampleTopicId)
    {
        /* received the expected message */
        done = true;
    }
}

/*
 * Create and use the Thingstream stack to publish a message.
 * @param modem_uart A handle to the serial port to use for the modem
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 */
void runApplication(UART_HandleTypeDef *modem_uart, UART_HandleTypeDef *debug_uart)
{
    debug_output = debug_uart;  /* setup uart handle for debug output */

    Transport* transport = serial_transport_create(modem_uart);
    CHECK("serial", transport != NULL);

    transport = line_buffer_transport_create(transport, line_buffer, LINE_BUFFER_LENGTH);
    CHECK("linebuf", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = log_modem_transport_create(transport, debug_printf, 0xFF);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    Transport* modem = modem_transport_create(transport, 0);
    CHECK("modem", modem != NULL);

    transport = base64_codec_create(modem);
    CHECK("base64", transport != NULL);

    transport = thingstream_transport_create(transport, thingstream_buffer, THINGSTREAM_BUFFER_LENGTH);
    CHECK("thingstream", transport != NULL);

#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
    transport = log_client_transport_create(transport, debug_printf, 0xFF);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */

    Client* client = Client_create(transport, NULL);
    CHECK("client", client != NULL);

    if (client != NULL)
    {
        Topic topic;
        ClientResult cr;

        cr = Client_connect(client, true, NULL, NULL);
        CHECK("connect", cr == CLIENT_SUCCESS);

        /* Registration is redundant here, since subscribeName can
         * also return the Id.
         * Typical applications might not subscribe to topics they
         * publish to, so this is included here for illustration.
         */
        cr = Client_register(client, EXAMPLE_TOPIC, &topic);
        CHECK("register", cr == CLIENT_SUCCESS);
        exampleTopicId = topic.topicId;

        Client_set_subscribe_callback(client, receiveCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        cr = Client_subscribeName(client, EXAMPLE_TOPIC, MQTT_QOS1, NULL);
        CHECK("subscribe", cr == CLIENT_SUCCESS);

        char *msg = "Hello from STM32";
        cr = Client_publish(client, topic, MQTT_QOS1, false, (uint8_t*) msg, strlen(msg), NULL);
        CHECK("publish", cr == CLIENT_SUCCESS);

        while (!done)
        {
            /* poll for incoming messages */
            Client_run(client, 1000);
        }
        cr = Client_disconnect(client, 0);
        CHECK("disconnect", cr == CLIENT_SUCCESS);
    }

error:
    return;
}

/*
 * Setup and create the Thingstream stack
 * @param modem_uart A handle to the serial port to use for the modem
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 */
Client* setupTSStack(UART_HandleTypeDef *modem_uart, UART_HandleTypeDef *debug_uart)
{
    debug_output = debug_uart;  /* setup uart handle for debug output */

    Transport* transport = serial_transport_create(modem_uart);
    CHECK("serial", transport != NULL);

    transport = line_buffer_transport_create(transport, line_buffer, LINE_BUFFER_LENGTH);
    CHECK("linebuf", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = log_modem_transport_create(transport, debug_printf, 0xFF);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    Transport* modem = modem_transport_create(transport, 0);
    CHECK("modem", modem != NULL);

    transport = base64_codec_create(modem);
    CHECK("base64", transport != NULL);

    transport = thingstream_transport_create(transport, thingstream_buffer, THINGSTREAM_BUFFER_LENGTH);
    CHECK("thingstream", transport != NULL);

#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
    transport = log_client_transport_create(transport, debug_printf, 0xFF);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */

    Client* client = Client_create(transport, NULL);
    CHECK("client", client != NULL);

    while(client!=NULL)
    {
        ClientResult cr;

        cr = Client_connect(client, true, NULL, NULL);
        CHECK("connect", cr == CLIENT_SUCCESS);
        break;
        //HAL_Delay(100);
    }  

    return client;

error:
    return NULL;
}

/*
 * Subscribe to a Topic.
 * @param p_client A Client instance that was configured in a previous function
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 * Returns the Topic that has been registered and subscribed to
 */
Topic subscribeTopic(Client* p_client, char* p_topicName)
{
    Topic topic;

    if (p_client != NULL)
    {
        ClientResult cr;

        /* Registration is redundant here, since subscribeName can
         * also return the Id.
         * Typical applications might not subscribe to topics they
         * publish to, so this is included here for illustration.
         */
        cr = Client_register(p_client, p_topicName, &topic);
        CHECK("register", cr == CLIENT_SUCCESS);
        exampleTopicId = topic.topicId;

        Client_set_subscribe_callback(p_client, receiveCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        cr = Client_subscribeName(p_client, p_topicName, MQTT_QOS1, NULL);
        CHECK("subscribe", cr == CLIENT_SUCCESS);

    }
    return topic;

error:
    return;
}

/*
 * Use the Thingstream stack to publish a message.
 * @param p_client A Client instance that was configured in a previous function
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 */
void publishMessage(Client* p_client, Topic p_Topic, char* p_msg)
{

    if (p_client != NULL)
    {
        //Topic topic;
        ClientResult cr;

        //cr = Client_connect(p_client, true, NULL, NULL);
        //CHECK("connect", cr == CLIENT_SUCCESS);

        /* Registration is redundant here, since subscribeName can
         * also return the Id.
         * Typical applications might not subscribe to topics they
         * publish to, so this is included here for illustration.
         */
        //cr = Client_register(p_client, EXAMPLE_TOPIC, &topic);
        //CHECK("register", cr == CLIENT_SUCCESS);
        //exampleTopicId = topic.topicId;

        //Client_set_subscribe_callback(p_client, receiveCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        //cr = Client_subscribeName(p_client, EXAMPLE_TOPIC, MQTT_QOS1, NULL);
        //CHECK("subscribe", cr == CLIENT_SUCCESS);

        //char *msg = "Hello from STM32";
        cr = Client_publish(p_client, p_Topic, MQTT_QOS1, false, (uint8_t*) p_msg, strlen(p_msg), NULL);
        CHECK("publish", cr == CLIENT_SUCCESS);

        while (!done)
        {
            /* poll for incoming messages */
            Client_run(p_client, 1000);
        }
        //cr = Client_disconnect(p_client, 0);
        //CHECK("disconnect", cr == CLIENT_SUCCESS);
    }

error:
    return;
}

/*
 * Wait to recieve a message from the subscribed Topic.
 * @param p_client A Client instance that was configured in a previous function
 */
void waitForMessage(Client* p_client)
{

    if (p_client != NULL)
    {
        while (!done)
        {
            /* poll for incoming messages */
            Client_run(p_client, 10000);
        }
        return;
    }

error:
    return;
}

/*
 * Disconnect the client connection.
 * @param p_client A Client instance that was configured in a previous function
 */
void disconnectClient(Client* p_client)
{

    if (p_client != NULL)
    {
        ClientResult cr;

        cr = Client_disconnect(p_client, 0);
        CHECK("disconnect", cr == CLIENT_SUCCESS);
    }

error:
    return;
}

#if defined(__cplusplus)
}
#endif
