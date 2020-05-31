/*
 * Copyright 2017 Thingstream AG
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
 * @brief Base64 codec implemented as a Transport instance
 */

#ifndef INC_BASE64_CODEC_TRANSPORT_H_
#define INC_BASE64_CODEC_TRANSPORT_H_

#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create a base64 codec instance
 * @param inner the transport instance the codec should wrap
 * @return the new Transport instance
 */
extern Transport* base64_codec_create(Transport* inner);

#if defined(__cplusplus)
}
#endif

#endif /* INC_BASE64_CODEC_TRANSPORT_H_ */
