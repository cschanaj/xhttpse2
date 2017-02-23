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
 * \file HttpseRuleset.h
 */

#include "pthread.h"
#include "HttpseXData.h"

#ifndef HTTPSE_RULEST_H
#define HTTPSE_RULEST_H

typedef struct HttpseRulesetOptions
{
	/**
	 * Path to Sublist3r output.
	 */
	const char *path;

	/**
	 * Ruleset name.
	 */
	const char *name;

	/**
	 * Ruleset platform.
	 */
	const char *platform;

	/**
	 * Enable securecookie flags.
	 */
	long securecookie;

	/**
	 * Output filename, defaults write to stdout.
	 */
	const char *output;

	/**
	 * Number of threads to use for testing, if possible.
	 */
	long num_threads;

	/**
	 * libcurl options to be used.
	 */
	HttpseRequestOptions options;
} HttpseRulesetOptions;

#ifndef kHttpseRulesetOptions
#define kHttpseRulesetOptions {       \
    .path = NULL,                     \
    .name = NULL,                     \
    .platform = NULL,                 \
    .securecookie = 0L,               \
    .output = NULL,                   \
    .options = kHttpseRequestOptions, \
    .num_threads = 4L,                \
}
#endif

/**
 * Storage of HTTPSE Ruleset.
 */
typedef struct HttpseRuleset
{
	/**
	 * Sorted array of HttpseXData.
	 */
	HttpseVector *_xdata;

	/**
	 * Actual ruleset.
	 */
	HttpseVector *ctx;

	/**
	 * Ruleset options to be used.
	 */
	const HttpseRulesetOptions *roptions;
} HttpseRuleset;

/**
 * Default constructor.
 */
HttpseRuleset *
HttpseRuleset_init(const HttpseRulesetOptions *roptions);

/**
 * Default destructor.
 */
HttpseRuleset *
HttpseRuleset_cleanup(HttpseRuleset *ru);

#endif /* HTTPSE_RULEST_H */
