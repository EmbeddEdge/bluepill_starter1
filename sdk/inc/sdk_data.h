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

#ifndef _SDK_DATA_H_
#define _SDK_DATA_H_

// If changing the structure regenerate the md5sum of lines between <<< and >>>
// <<< md5sum SDK_DATA_AT_CREG_NAME
/** The AT+CREG results (when available) from the modem are passed to the
 * thingstream layer via this global structure.
 */
struct at_creg_s
{
    /** Network status 0..5 */
    uint8_t stat;

    /** location code (4 or 8 chars representing a 16/32-bit hex number) */
    uint8_t lac[8];

    /** cell id (4 or 8 chars representing a 16/32-bit hex number) */
    uint8_t cid[8];
};
// >>> md5sum SDK_DATA_AT_CREG_NAME
// The real name of the structure has a md5sum of the structure definition so
// that we can force link-time errors when the structure contents change.
#define SDK_DATA_AT_CREG_NAME sdk_data_at_creg_5c0b62280a27e1cfa599b79c5cd77b6b
extern struct at_creg_s SDK_DATA_AT_CREG_NAME;
#define SDK_DATA_AT_CREG(field) SDK_DATA_AT_CREG_NAME.field

// If changing the structure regenerate the md5sum of lines between <<< and >>>
#ifndef THINGSTREAM_BEARER_NAME_MAX_SIZE
#define THINGSTREAM_BEARER_NAME_MAX_SIZE  (15)
#endif /* THINGSTREAM_BEARER_NAME_MAX_SIZE */
// <<< md5sum SDK_DATA_GSM_BEARER_NAME
/** The AT+CSQ/AT+COPS? results (when available) from the modem are passed to
 * the thingstream layer via this global structure.
 */
typedef struct GsmBearer_s
{
    /** The strength of the GSM connection, e.g. AT+CSQ result */
    uint8_t strength;
    /** The size (in bytes) of the bearerName array and this
     * must be equal to the \#define THINGSTREAM_BEARER_NAME_MAX_SIZE
     */
    uint8_t bearerNameSize;
    /** The first chars of the GSM connection name, e.g. AT+COPS? result */
    uint8_t bearerName[THINGSTREAM_BEARER_NAME_MAX_SIZE];
} GsmBearer;
// >>> md5sum SDK_DATA_GSM_BEARER_NAME
// The real name of the structure has a md5sum of the structure definition so
// that we can force link-time errors when the structure contents change.
#define SDK_DATA_GSM_BEARER_NAME sdk_data_gsm_bearer_2b7a20792060a26a50fdbfffac6a8739
extern GsmBearer SDK_DATA_GSM_BEARER_NAME;
#define SDK_DATA_GSM_BEARER(field) SDK_DATA_GSM_BEARER_NAME.field

#endif /* _SDK_DATA_H_ */
