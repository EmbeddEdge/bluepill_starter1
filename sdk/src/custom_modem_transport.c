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
 * @brief ThingstreamTransport implementation that communicates with a custom modem
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <custom_modem_transport.h>
#include <transport_api.h>

#include <sdk_data.h>

#ifdef USE_CUSTOM_MODEM_DRIVER

#if defined(__cplusplus)
extern "C" {
#endif


#ifndef THINGSTREAM_SHORTCODE
/** The THINGSTREAM_SHORTCODE is the USSD shortcode required to communicate
 * with the Thingstream USSD server.
 */
#define THINGSTREAM_SHORTCODE             "469"
#endif /* THINGSTREAM_SHORTCODE */

/**
 * The CustomModemState structure is used to store state for the custom modem
 * transport.
 */
typedef struct {
    /**
     * A sub-structure used to store the active callbacks and their associated
     * cookies.
     */
    struct {
        ThingstreamTransportCallback_t transport;
        void* tcookie;
        ThingstreamModemCallback_t modem;
        void* mcookie;
    } callback;

    /**
     * The buffer used to receive inbound USSD messages.
     */
    uint8_t buffer[THINGSTREAM_USSD_BUFFER_LEN];
} CustomModemState;

static CustomModemState _custom_modem_transport_state;

static ThingstreamTransportResult custom_modem_init(ThingstreamTransport* self, uint16_t version);
static ThingstreamTransportResult custom_modem_shutdown(ThingstreamTransport* self);
static ThingstreamTransportResult custom_modem_get_buffer(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len);
static const char* custom_modem_get_client_id(ThingstreamTransport* self);
static ThingstreamTransportResult custom_modem_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);
static ThingstreamTransportResult custom_modem_register_transport_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie);
static ThingstreamTransportResult custom_modem_run(ThingstreamTransport* self, uint32_t millis);

static const ThingstreamTransport _custom_modem_transport_instance = {
    (ThingstreamTransportState_t*)&_custom_modem_transport_state,
    custom_modem_init,
    custom_modem_shutdown,
    custom_modem_get_buffer,
    custom_modem_get_client_id,
    custom_modem_send,
    custom_modem_register_transport_callback,
    NULL,
    custom_modem_run
};


/**
 * Create an instance of the modem transport.
 * TODO: @param ?
 * @return the instance
 */
ThingstreamTransport* Thingstream_createCustom_modemTransport(/* porting specific options */)
{
    ThingstreamTransport *self = (ThingstreamTransport*)&_custom_modem_transport_instance;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO: save any porting specific options in state->xxx */

    return self;
}

static ThingstreamTransportResult custom_modem_init(ThingstreamTransport* self, uint16_t version)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    if (version != TRANSPORT_VERSION)
    {
        return TRANSPORT_VERSION_MISMATCH;
    }

    state->callback.transport = NULL;
    state->callback.modem = NULL;

    /* TODO: fill in initialisation details */

    return tRes;
}

static ThingstreamTransportResult custom_modem_shutdown(ThingstreamTransport* self)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    state->callback.transport = NULL;
    state->callback.modem = NULL;

    /* TODO:
     * Undo any initialisations performed when custom_modem_init() was called.
     */

    return tRes;
}

static ThingstreamTransportResult custom_modem_get_buffer(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len)
{
    CustomModemState* state = (CustomModemState*)self->_state;
    *buffer = state->buffer;
    *len = sizeof(state->buffer);
    return TRANSPORT_SUCCESS;
}

static const char* custom_modem_get_client_id(ThingstreamTransport* self)
{
    /* This API is currently unused */
    return "custom-modem-id";
}

static ThingstreamTransportResult custom_modem_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO:
     *
     * Send the data bytes to the Thingstream USSD server using
     * THINGSTREAM_SHORTCODE
     *
     * A Hayes compatible modem would use
     *   AT+CUSD=1,"#<shortcode>*<payload>#"
     *
     * If the flags contain the TSEND_USSD_SESSION_END bit then the USSD
     * session need to be terminated after sending the payload.
     *
     * A Hayes compatible modem would use
     *   AT+CUSD=2
     *
     * some modem hardware allows the termination to be combined with the
     * data payload...
     *   AT+CUSD=2,"#<shortcode>*<payload>#"
     *
     * The maximum len passed by the Thingstream transport stack is 145 bytes.
     */

    return tRes;
}

static ThingstreamTransportResult custom_modem_register_transport_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie)
{
    CustomModemState* state = (CustomModemState*)self->_state;
    state->callback.transport = callback;
    state->callback.tcookie = cookie;
    return TRANSPORT_SUCCESS;
}

static ThingstreamTransportResult custom_modem_run(ThingstreamTransport* self, uint32_t millis)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO:
     *
     * Check to see if there are any incoming USSD messages (a Hayes compatible
     * modem would see these as +CUSD: <n>,"<payload>") and use the registered
     * callback to send the <payload> to the Thingstream transport stack.
     * The callback must not be called unless custom_modem_run() is being
     * called by the Thingstream transport stack, or
     * Thingstream_Modem_sendLine() is being called which can contain
     * an internal call to custom_modem_run().
     *
     * If Thingstream_Modem_sendLine() and Thingstream_Modem_setCallback()
     * are supported then any unsolicited incoming responses that are
     * not USSD messages and are not expected by the custom modem
     * internals should be sent to the registered callback function.
     *
     * Wait no longer than 'millis' for incoming messages before returning.
     *
     * Unexpected "incoming" errors can be reported by returning a
     * #ThingstreamTransportResult error code.
     */

    return tRes;
}


/**
 * Set the function that will be called when the modem receives an
 * unrecognized response.
 * @param self the ThingstreamTransport instance of this custom modem
 * @param callback the ThingstreamModemCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 */
void Thingstream_Modem_setCallback(ThingstreamTransport* self, ThingstreamModemCallback_t callback, void* cookie)
{
    CustomModemState* state = (CustomModemState*)self->_state;
    state->callback.modem = callback;
    state->callback.mcookie = cookie;
}

/**
 * Send the line to the modem and wait for an OK response.
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param line a null-terminated line to send to the modem
 * @param millis the maximum number of milliseconds to run
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
ThingstreamTransportResult Thingstream_Modem_sendLine(ThingstreamTransport* self, const char* line, uint32_t millis)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO:
     *
     * Simulate the sending of the line (which will be a Hayes compatible AT+
     * command line) to the modem, wait for the equivalent of an OK response.
     *
     * Wait no longer than 'millis' for incoming messages before returning.
     */

    return tRes;
}


#if defined(__cplusplus)
}
#endif

#endif /* USE_CUSTOM_MODEM_DRIVER */
