# xhttpse2
Automatically generate HTTPS Everywhere ruleset

## Notes

This project is marked read-only from May 2021 per EFF's announcement on HTTPS Everywhere End-of-Life (https://www.eff.org/deeplinks/2021/04/https-everywhere-now-uses-duckduckgos-smarter-encryption).

## Usage
    $ xhttpse --help
    Usage: xhttpse [OPTION] ... [FILE]
    Automatically generate HTTPS Everywhere ruleset.
    
    Options
         --name NAME                 Set NAME the output ruleset name
     -o, --output FILE               Write output to FILE instead of stdout
         --platform PLATFORM         Set PLATFORM the output ruleset platform
         --securecookie              Enable securecookie flag in the output
    
    Options (libcurl)
         --capath DIR                CA directory to verify peer against (SSL)
         --compressed                Request compressed response
         --connect-timeout SECONDS   Maximum time allowed for connection
     -m, --max-time SECONDS          Maximum time allowed for the transfer
         --tr-encoding               Request compressed transfer encoding
     -A, --useragent STRING          Send User-Agent STRING to server
     -v, --verbose                   Enable libcurl verbose mode
   
    Misc.
         --num-threads NUM           Use NUM threads whenever possible
     -h, --help                      Display this help and exit

## Dependencies
    CURL                https://github.com/curl/curl
    Google Gumbo Parser https://github.com/google/gumbo-parser
    OpenSSL v1.1.0+     https://github.com/openssl/openssl

## Apache License, Version 2.0
    Copyright 2017 Pasu Chan <cschanaj@connect.ust.hk>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
