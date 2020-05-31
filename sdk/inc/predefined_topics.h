/*
 * Copyright 2018 Thingstream AG
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
 * @brief Defines predefined topics.
 *
 * This file is not used directly by the Thingstream SDK but is provided for
 * use (or modification) by customers.
 */

#ifndef INC_PREDEFINED_TOPICS_H_
#define INC_PREDEFINED_TOPICS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "client_api.h"

#if defined(__cplusplus)
}
#endif

/** See predefined_topics.c */
extern const Topic PredefinedSelfTopic;


#endif /* INC_PREDEFINED_TOPICS_H_ */
