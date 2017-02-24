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
 * \file HttpseVector.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HTTPSE_VECTOR_H
#define HTTPSE_VECTOR_H

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * Sequence container that encapsulates dynamic size array.
 */
typedef struct HttpseVector
{
	/**
	 * Pointer to the underlying array serving as element storage.
	 */
	void *data;

	/**
	 * Pointer to a null-terminated character array.
	 */
	char *c_str;

	/**
	 * Size of HttpseVector#data in bytes.
	 */
	size_t size;

	/**
	 * Number of bytes the container has currently allocated space for.
	 */
	size_t max_size;
} HttpseVector;

/**
 * Constructs empty container (zero size and unspecified capacity).
 */
HttpseVector *
HttpseVector_init(void);

/**
 * HttpseVector_init() + HttpseVector_append()
 */
HttpseVector *
HttpseVector_init1(const void *ptr, size_t size, size_t nmemb);

/**
 * Extends HttpseVector#data by appending additional data at the end.
 */
size_t
HttpseVector_append(const void *ptr, size_t size, size_t nmemb, 
	HttpseVector *ctx);

/**
 * See HttpseVector_append()
 */
size_t
HttpseVector_append1(const char *ptr, HttpseVector *ctx);

/**
 * Cleanup memory.
 */
HttpseVector *
HttpseVector_cleanup(HttpseVector *ctx);

#endif /* HTTPSE_VECTOR_H */
