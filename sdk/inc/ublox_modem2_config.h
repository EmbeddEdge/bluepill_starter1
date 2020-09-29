/*
 * Copyright 2019-2020 Thingstream AG
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
 * @addtogroup udp_modems
 * @{
 */

#ifndef INC_UBLOX_MODEM2_CONFIG_H
#define INC_UBLOX_MODEM2_CONFIG_H

#include "modem2_transport.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Pass #Thingstream_uBloxLaraR2Init to Thingstream_createModem2Transport()
 * to configure the modem driver to match the u-blox LARA-R2xx hardware.
 */
extern ThingstreamModem2UdpInit Thingstream_uBloxLaraR2Init;

/**
 * Pass #Thingstream_uBloxSaraU2Init to Thingstream_createModem2Transport()
 * to configure the modem driver to match the u-blox SARA-U2xx hardware.
 */
extern ThingstreamModem2UdpInit Thingstream_uBloxSaraU2Init;

/**
 * Pass #Thingstream_uBloxSaraR4Init to Thingstream_createModem2Transport()
 * to configure the modem driver to match the u-blox SARA-R4xx hardware.
 */
extern ThingstreamModem2UdpInit Thingstream_uBloxSaraR4Init;

#if defined(__cplusplus)
}
#endif

#endif /* INC_UBLOX_MODEM2_CONFIG_H */
/** @} */
