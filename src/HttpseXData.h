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
 * \file HttpseXData.h
 */

#include "HttpseCode.h"
#include "HttpseRequest.h"

#include "pthread.h"

#ifndef HTTPSE_XDATA_H
#define HTTPSE_XDATA_H

#ifndef HTTPSE_XDATA_BUFSZ
#define HTTPSE_XDATA_BUFSZ 1024
#endif

typedef struct HttpseTData
{
	/**
	 * Plain text URL, e.g. http://www.example.com
	 */
	char urlp[HTTPSE_XDATA_BUFSZ];

	/**
	 * Secure URL, e.g. https://www.example.com
	 */
	char urls[HTTPSE_XDATA_BUFSZ];

	/**
	 * Pointer to HTTP session, after HttpseRequest_perform().
	 * Remark: do not free().
	 */
	HttpseRequest *rp;

	/**
	 * Pointer to HTTPS session, after HttpseRequest_perform().
	 * Remark: do not free().
	 */
	HttpseRequest *rs;

} HttpseTData;

typedef struct HttpseXData
{
	/**
	 * Target host to be tested against, e.g. www.example.com.
	 */
	char host[HTTPSE_XDATA_BUFSZ];

	/**
	 * libcurl options to be used.
	 */
	const HttpseRequestOptions *options;

	/**
	 * Alphabetical order.
	 */
	size_t index;

	/**
	 * Error code
	 */
	HttpseCode error;

} HttpseXData;

/**
 * Compare callback to pass to qsort()
 */
int
HttpseXData_compare(const void *xdatap1, const void *xdatap2);

/**
 * This callback is passed to pthread_create() and perform 
 * all the necessary tests against HttpseXData#host
 *
 * \return NULL
 */
void *
HttpseXData_perform(void *xdatap);

#endif /* HTTPSE_XDATA_H */
