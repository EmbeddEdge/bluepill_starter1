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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <transport_api.h>
#include <serial_transport.h>

#include <client_platform.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* UART callback */
static TransportResult serial_init(Transport* self, uint16_t version);
static TransportResult serial_shutdown(Transport* self);
static TransportResult serial_get_buffer(Transport* self, uint8_t** buffer, uint16_t* len);
static const char* serial_get_client_id(Transport* self);
static TransportResult serial_send(Transport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t timeout);
static TransportResult serial_register_callback(Transport* self, Transport_callback callback, void* cookie);
static TransportResult serial_deregister_callback(Transport* self);
static TransportResult serial_run(Transport* self, uint32_t millis);

typedef struct {
    Transport_callback callback;
    void* cookie;
    UART_HandleTypeDef* huart;
    uint8_t isr_received_data;
    uint8_t isr_rx_restart;
} SerialState;

static SerialState _serial_transport_state;

static const Transport _serial_instance = {
    (TransportState*)&_serial_transport_state,
    serial_init,
    serial_shutdown,
    serial_get_buffer,
    serial_get_client_id,
    serial_send,
    serial_register_callback,
    serial_deregister_callback,
    serial_run
};

static void try_enable_rx_irq(SerialState *state)
{
    state->isr_rx_restart = 0;
    if (HAL_UART_Receive_IT(state->huart, &state->isr_received_data, 1) == HAL_BUSY)
    {
        /* We cannot re-enable interrupts now, since there is probably a TX in
         * progress. Set a flag and re-enable at the first opportunity.
         */
        state->isr_rx_restart = 1;
    }
}

static void serial_RxCpltCallback(UART_HandleTypeDef *huart)
{
    SerialState *state = &_serial_transport_state;

    if (state->callback != NULL)
    {
        state->callback(state->cookie, &state->isr_received_data, 1);
    }
    try_enable_rx_irq(state);
}

static void serial_ErrorCallback(UART_HandleTypeDef *huart)
{
    SerialState *state = &_serial_transport_state;

    /* Clear any errors and restart the interrupt transfer */
    HAL_UART_Abort_IT(huart);
    try_enable_rx_irq(state);
    /* TODO: Report the error to the Transport stack */
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/* Define per-uart callbacks */

HAL_StatusTypeDef uart_register_callbacks(UART_HandleTypeDef* huart)
{
    if ((HAL_UART_RegisterCallback(huart, HAL_UART_ERROR_CB_ID,
                           serial_ErrorCallback) == HAL_OK) &&
        (HAL_UART_RegisterCallback(huart, HAL_UART_RX_COMPLETE_CB_ID,
                           serial_RxCpltCallback) == HAL_OK))
    {
        return HAL_OK;
    }
    else
        return HAL_ERROR;
}
#else
/* Per-uart callbacks not available so override the
 * default callbacks instead.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    SerialState *state = &_serial_transport_state;

    if (huart == state->huart)
    {
        serial_RxCpltCallback(huart);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    SerialState *state = &_serial_transport_state;

    if (huart == state->huart)
    {
        serial_ErrorCallback(huart);
    }
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

Transport* serial_transport_create(UART_HandleTypeDef* port)
{
    Transport *self = (Transport*)&_serial_instance;
    SerialState *state = (SerialState*)self->_state;
    state->huart = port;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    if (uart_register_callbacks(state->huart) != HAL_OK)
    {
        return NULL;
    }
#endif

    return self;
}

static TransportResult serial_init(Transport* self, uint16_t version)
{
    SerialState *state = (SerialState*)self->_state;

    if (version != TRANSPORT_VERSION)
    {
        return TRANSPORT_VERSION_MISMATCH;
    }

    (void) HAL_UART_Abort_IT(state->huart);
    try_enable_rx_irq(state);
    return (state->isr_rx_restart == 0 ? TRANSPORT_SUCCESS : TRANSPORT_ERROR);
}

static TransportResult serial_shutdown(Transport* self)
{
    SerialState *state = (SerialState*)self->_state;
    (void) HAL_UART_Abort_IT(state->huart);
    return TRANSPORT_SUCCESS;
}

static TransportResult serial_get_buffer(Transport* self, uint8_t** buffer, uint16_t* len)
{
    /* This will not be called when using the mandatory line buffer transport.
     * When called in any other context returns an error.
     */
    *len = 0;
    return TRANSPORT_ERROR;
}

static const char* serial_get_client_id(Transport* self)
{
    return "stm32-client";
}

static TransportResult serial_send(Transport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t timeout)
{
    SerialState *state = (SerialState*)self->_state;
    TransportResult tRes = TRANSPORT_SUCCESS;
    if (HAL_OK == HAL_UART_Transmit_IT(state->huart, data, len))
    {
        /* Wait for the Transmit to complete */
        uint32_t limit = Platform_getTimeMillis() + timeout;
        UART_HandleTypeDef* huart = state->huart;
        while ((HAL_UART_GetState(huart) & HAL_UART_STATE_BUSY_TX)
                                        == HAL_UART_STATE_BUSY_TX)
        {
            if (TIME_COMPARE(Platform_getTimeMillis(), >=, limit))
            {
                HAL_UART_AbortTransmit_IT(state->huart);
                tRes = TRANSPORT_SEND_TIMEOUT;
                break;
            }
            __WFI();
        }
    }
    else
    {
        tRes = TRANSPORT_ERROR;
    }
    if (state->isr_rx_restart)
    {
        try_enable_rx_irq(state);
    }
    return tRes;
}

static TransportResult serial_register_callback(Transport* self, Transport_callback callback, void* cookie)
{
    SerialState *state = (SerialState*)self->_state;
    state->callback = callback;
    state->cookie = cookie;
    return TRANSPORT_SUCCESS;
}

static TransportResult serial_deregister_callback(Transport* self)
{
    SerialState *state = (SerialState*)self->_state;
    state->callback = NULL;
    return TRANSPORT_SUCCESS;
}

static TransportResult serial_run(Transport* self, uint32_t millis)
{
    /* This will be called by the line buffer transport when waiting for data.
     * It is difficult to safely wait for the next complete line (which may
     * have occurred between the test in line buffer transport and any test we
     * do here) so just wait for the next interrupt and return and re-test in
     * the line buffer transport.
     * We do restart the RX interrupt if needed.
     */
    SerialState *state = (SerialState*)self->_state;
    if (state->isr_rx_restart)
    {
        try_enable_rx_irq(state);
    }

    __WFI();

    return TRANSPORT_SUCCESS;
}

#if defined(__cplusplus)
}
#endif
