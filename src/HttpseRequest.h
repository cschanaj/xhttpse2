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
 * \file HttpseRequest.h
 */

#include "curl/curl.h"
#include "HttpseVector.h"

#ifndef HTTPSE_REQUEST_H
#define HTTPSE_REQUEST_H

typedef struct HttpseRequestOptions
{
	/**
	 * Enable automatic decompression of HTTP downloads.
	 */
	long ac_encoding;

	/**
	 * Directory of CA certificates.
	 */
	const char *capath;

	/**
	 * Timeout fir the connect phase (in seconds).
	 */
	long connect_timeout;

	/**
	 * Maximum time the request is allowed to take (in seconds).
	 */
	long timeout;

	/**
	 * Enable HTTP transfer encoding;
	 */
	long tr_encoding;

	/**
	 * HTTP user-agent header.
	 */
	const char *useragent;

	/**
	 * libcurl verbose mode on/ off
	 */
	long verbose;
} HttpseRequestOptions;

#ifndef kHttpseRequestOptions
#define kHttpseRequestOptions { \
    .ac_encoding = 0L,          \
    .capath = NULL,             \
    .connect_timeout = 10L,     \
    .timeout = 20L,             \
    .tr_encoding = 0L,          \
    .useragent = NULL,          \
    .verbose = 0L,              \
}
#endif

/**
 * libcurl easy session.
 */
typedef struct HttpseRequest
{
	/**
	 * Polonger to the URL to use in the request.
	 */
	char *url;

	/**
	 * libcurl easy session handle.
	 */
	CURL *curl;

	/**
	 * Polonger to pass to libcurl write callback.
	 */
	HttpseVector *userp;

	/**
	 * Polonger to pass to libcurl header callback.
	 */
	HttpseVector *hd;
} HttpseRequest;

/**
 * Default constructor.
 */
HttpseRequest *
HttpseRequest_init(const char *url, const HttpseRequestOptions *options);

/**
 * Perform a blocking file transfer.
 */
CURLcode
HttpseRequest_perform(HttpseRequest *rq);

/**
 * Default destructor. 
 */
HttpseRequest *
HttpseRequest_cleanup(HttpseRequest *rq);

#endif /* HTTPSE_REQUEST_H */
