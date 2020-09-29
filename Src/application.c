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

#include <string.h>

#include "client_api.h"
#include "ring_buffer_transport.h"
#include "modem_transport.h"
#include "base64_codec_transport.h"
#include "thingstream_transport.h"
#include "thingstream_util.h"
#include "client_platform.h"
#include "modem2_config.h"

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

/* ------------ SETUP to use UDP or USSD ---------------- */
/* Specify the modem initialisation routine to be passed
 * to Thingstream_createModem2Transport() for UDP.
 * Undefine to use USSD.
 */
#define THINGSTREAM_UDP_MODEM Thingstream_Simcom800Init
#undef THINGSTREAM_UDP_MODEM            /* for USSD */            

#ifdef THINGSTREAM_UDP_MODEM
#define THINGSTREAM_MODEM_INIT THINGSTREAM_UDP_MODEM
static uint8_t modemBuf[1000];
#else
#define THINGSTREAM_MODEM_INIT Thingstream_UssdInit
static uint8_t modemBuf[MODEM2_USSD_BUFFER_LEN];
#endif

static uint8_t ringBuf[250];

#define EXAMPLE_TOPIC "test/stm32/first"
static uint16_t exampleTopicId = 0; /* will get updated after registration */

#define CHECK(msg, cond) do { \
    Thingstream_Util_printf("%s %s @%d\n", msg, ((cond) ? "ok" : "ERROR"), __LINE__); \
    if (!(cond)) goto error; \
  } while(0)

static bool done = false;
static UART_HandleTypeDef *debug_output;

/* Thingstream required routine, see client_platform.h */
uint32_t Thingstream_Platform_getTimeMillis(void)
{
    return HAL_GetTick();
}

/* Used by debug logging */
void Thingstream_Platform_puts(const char* str, int len)
{
    if (debug_output != NULL)
    {
        HAL_UART_Transmit(debug_output, (uint8_t *)str, len, 5000);
    }
}

/* Callback for receiving messages.
 * This will be called from within Thingstream_Client_run()
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic,
     ThingstreamQualityOfService_t qos, uint8_t* msg, uint16_t msgLen)
{
    if (msgLen > 0)
    {
        Thingstream_Util_printf("Received message: ");
        Thingstream_Platform_puts((const char *)msg, msgLen);
        Thingstream_Util_printf("\n");
    }
    else
    {
        Thingstream_Util_printf("Received empty message\n");
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

    ThingstreamTransport* transport = serial_transport_create(modem_uart);
    CHECK("serial", transport != NULL);

    transport = Thingstream_createRingBufferTransport(transport, ringBuf,
                                                        sizeof(ringBuf));
    CHECK("ringbuf", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = Thingstream_createModemLogger(transport, Thingstream_Util_printf, 0xFF);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    ThingstreamTransport* modem = Thingstream_createModem2Transport(transport,
                                                0,
                                                modemBuf, sizeof(modemBuf),
                                                THINGSTREAM_MODEM_INIT,
                                                Thingstream_Util_printf);
    CHECK("modem", modem != NULL);

    transport = Thingstream_createBase64CodecTransport(modem);
    CHECK("base64", transport != NULL);

    transport = Thingstream_createProtocolTransport(transport, NULL, 0);
    CHECK("thingstream", transport != NULL);

#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
    transport = Thingstream_createClientLogger(transport, Thingstream_Util_printf, 0xFF);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */

    ThingstreamClient* client = Thingstream_createClient(transport);
    CHECK("client", client != NULL);

    ThingstreamClientResult cr = Thingstream_Client_init(client);
    CHECK("client init", cr == CLIENT_SUCCESS);

    if (client != NULL)
    {
        ThingstreamTopic topic;

        cr = Thingstream_Client_connect(client, true, 0, NULL);
        CHECK("connect", cr == CLIENT_SUCCESS);

        /* Registration is redundant here, since subscribeName can
         * also return the Id.
         * Typical applications might not subscribe to topics they
         * publish to, so this is included here for illustration.
         */
        cr = Thingstream_Client_register(client, EXAMPLE_TOPIC, &topic);
        CHECK("register", cr == CLIENT_SUCCESS);
        exampleTopicId = topic.topicId;

        Thingstream_Client_setSubscribeCallback(client, Thingstream_Application_subscribeCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        cr = Thingstream_Client_subscribeName(client, EXAMPLE_TOPIC, ThingstreamQOS1, NULL);
        CHECK("subscribe", cr == CLIENT_SUCCESS);

        char *msg = "Hello from STM32 SDK2";
        cr = Thingstream_Client_publish(client, topic, ThingstreamQOS1, false, (uint8_t*) msg, strlen(msg));
        CHECK("publish", cr == CLIENT_SUCCESS);

        while (!done)
        {
            /* poll for incoming messages */
            Thingstream_Client_run(client, 1000);
        }
        cr = Thingstream_Client_disconnect(client, 0);
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
 * * Returns the ThingstreamClient that has been configured
 */
ThingstreamClient* setupTSStack(UART_HandleTypeDef *modem_uart, UART_HandleTypeDef *debug_uart)
{
    debug_output = debug_uart;  /* setup uart handle for debug output */

    ThingstreamTransport* transport = serial_transport_create(modem_uart);
    CHECK("serial", transport != NULL);

    transport = Thingstream_createRingBufferTransport(transport, ringBuf,
                                                        sizeof(ringBuf));
    CHECK("ringbuf", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = Thingstream_createModemLogger(transport, Thingstream_Util_printf, 0xFF);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    ThingstreamTransport* modem = Thingstream_createModem2Transport(transport,
                                                0,
                                                modemBuf, sizeof(modemBuf),
                                                THINGSTREAM_MODEM_INIT,
                                                Thingstream_Util_printf);
    CHECK("modem", modem != NULL);

    transport = Thingstream_createBase64CodecTransport(modem);
    CHECK("base64", transport != NULL);

    transport = Thingstream_createProtocolTransport(transport, NULL, 0);
    CHECK("thingstream", transport != NULL);

#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
    transport = Thingstream_createClientLogger(transport, Thingstream_Util_printf, 0xFF);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */

    ThingstreamClient* client = Thingstream_createClient(transport);
    CHECK("client", client != NULL);

    ThingstreamClientResult cr = Thingstream_Client_init(client);
    CHECK("client init", cr == CLIENT_SUCCESS);

    while(client!=NULL)
    {
        cr = Thingstream_Client_connect(client, true, 0, NULL);
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
 * @param p_client A ThingstreamClient instance that was configured in a previous function
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 * Returns the Topic that has been registered and subscribed to
 */
ThingstreamTopic subscribeTopic(ThingstreamClient* p_client, char* p_topicName)
{
    ThingstreamTopic topic;

    if (p_client != NULL)
    {
        ThingstreamClientResult cr;

        /* Registration is redundant here, since subscribeName can
         * also return the Id.
         * Typical applications might not subscribe to topics they
         * publish to, so this is included here for illustration.
         */
        cr = Thingstream_Client_register(p_client, p_topicName, &topic);
        CHECK("register", cr == CLIENT_SUCCESS);
        exampleTopicId = topic.topicId;

        Thingstream_Client_setSubscribeCallback(p_client, Thingstream_Application_subscribeCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        cr = Thingstream_Client_subscribeName(p_client, p_topicName, ThingstreamQOS1, NULL);
        CHECK("subscribe", cr == CLIENT_SUCCESS);

    }
    return topic;

error:
    return;
}

/*
 * Use the Thingstream stack to publish a message.
 * @param p_client A ThingstreamClient instance that was configured in a previous function
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 */
void publishMessage(ThingstreamClient* p_client, ThingstreamTopic p_Topic, char* p_msg)
{

    if (p_client != NULL)
    {
        //Topic topic;
        ThingstreamClientResult cr;

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

        //char *msg = "Hello from STM32 SDK2";
        cr = Thingstream_Client_publish(p_client, p_Topic, ThingstreamQOS1, false, (uint8_t*) p_msg, strlen(p_msg));
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
 * @param p_client A ThingstreamClient instance that was configured in a previous function
 */
void waitForMessage(ThingstreamClient* p_client)
{

    if (p_client != NULL)
    {
        while (!done)
        {
            /* poll for incoming messages */
            Thingstream_Client_run(p_client, 10000);
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
void disconnectClient(ThingstreamClient* p_client)
{

    if (p_client != NULL)
    {
        ClientResult cr;

        cr = Thingstream_Client_disconnect(p_client, 0);
        CHECK("disconnect", cr == CLIENT_SUCCESS);
    }

error:
    return;
}

#if defined(__cplusplus)
}
#endif
