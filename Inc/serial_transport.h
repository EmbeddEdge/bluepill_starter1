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
 * @brief An interface to communicate over a serial port.
 */
#ifndef INC_SERIAL_API_H_
#define INC_SERIAL_API_H_

#include "main.h"
/* If the generated main.h does not include the target
 * specific HAL header then include it here, e.g.
#include "stm32f4xx_hal.h"
 */

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <transport_api.h>

/**
 * Create a Serial instance that transfers bytes over a serial port.
 * @param port A handle to the serial port to use.
 * @return an instance of Serial
 */
extern Transport* serial_transport_create(UART_HandleTypeDef* port);

#if defined(__cplusplus)
}
#endif

#endif /* INC_SERIAL_API_H_ */
