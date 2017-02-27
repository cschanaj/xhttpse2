/* Copyright 2017 Pasu Chan <cschanaj@connect.ust.hk>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file HttpseCode.h
 */

#include "curl/curl.h"

#ifndef HTTPSE_CODE_H
#define HTTPSE_CODE_H

/**
 * Error code.
 */ 
typedef enum HttpseCode
{
	/**
	 * Everything was OK 
	 */
	HTTPSE_OK = CURLE_OK,

	/**
	 * Invalid characters in URL
	 */
	HTTPSE_URL_MALFORMAT = CURLE_URL_MALFORMAT,

	/**
	 * Never return, never use.
	 */
	HTTPSE_OBSOLETE = CURL_LAST,

	/**
	 * HTTP(s) give different HTTP status code.
	 */
	HTTPSE_STATUS_CODE_MISMATCH,

	/**
	 * HTTP(s) return 4xx HTTP status code.
	 */
	HTTPSE_STATUS_CODE_4XX,

	/**
	 * HTTP(s) return 5xx HTTP status code.
	 */
	HTTPSE_STATUS_CODE_5XX,

	/**
	 * HTTP(s) return non-2xx/3xx/4xx/5xx HTTP status code.
	 */
	HTTPSE_STATUS_CODE_OTHERS,

	/**
	 * Secure connection redirects to HTTP.
	 */
	HTTPSE_SECURE_FALLBACK,

	/**
	 * Different content served over HTTP(s).
	 */
	HTTPSE_DIFFERENT_CONTENT,

	/**
	 * Secure connection use plain text resources.
	 */
	HTTPSE_MIXED_CONTENT,

	/**
	 * Unknown error.
	 */
	HTTPSE_ERROR_UNKNOWN,

	/**
	 * Never return, never use.
	 */
	HTTPSE_LAST
} HttpseCode;

/**
 * Convert error code to character string
 */
const char *
HttpseCode_strerror(HttpseCode code);

#endif /* HTTPSE_CODE_H */
