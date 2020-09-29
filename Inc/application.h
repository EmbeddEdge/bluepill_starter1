/*
 * Copyright 2019 Thingstream AG
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

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#define EXAMPLE_TOPIC "test/stm32/first"
#define DEF_TOPIC "events/embeddedge/manual"

#include "main.h"
/* If the generated main.h does not include the target
 * specific HAL header then include it here, e.g.
#include "stm32f4xx_hal.h"
 */

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Create and use the Thingstream stack to publish a message.
 * @param modem_uart A handle to the serial port to use for the modem
 * @param debug_uart A handle to the serial port to use for debug output.
 *                   If NULL, then no debug output
 */
extern void runApplication(UART_HandleTypeDef*, UART_HandleTypeDef*);
extern ThingstreamClient* setupTSStack(UART_HandleTypeDef*, UART_HandleTypeDef*);
extern ThingstreamTopic subscribeTopic(ThingstreamClient*, char*);
extern void publishMessage(ThingstreamClient*, ThingstreamTopic, char*);
extern void waitForMessage(ThingstreamClient*);
extern void disconnectClient(ThingstreamClient*);

#if defined(__cplusplus)
}
#endif

#endif /* INC_APPLICATION_H_ */
