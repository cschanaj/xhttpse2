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
 * \file Main.h
 */

#include "getopt.h"
#include "HttpseRuleset.h"

#ifndef HTTPSE_MAIN_H
#define HTTPSE_MAIN_H

int 
main(int argc, char **argv);

void
print_usage(const char *me);

int
httpse_getopt_long(int argc, char **argv, HttpseRulesetOptions *roptions);

#endif /* HTTPSE_MAIN_H */
