/*
 * Copyright 2018-2019 Thingstream AG
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
 * @brief Deprecated - the connection timeout is now passed as a parameter to Thingstream_client_connect()
 */

#include <stdint.h>
#include <client_api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * @deprecated This is used by the compatabilty macro Client_connect() but
 * not when Thingstream_Client_connect() is used.
 */
uint16_t Thingstream_Client_connectionTimeout = THINGSTREAM_DEFAULT_CONNECT_KEEPALIVE;

#if defined(__cplusplus)
}
#endif
