#include "HttpseRequest.h"

#ifndef HTTPSE_CURL_SETOPT
#define HTTPSE_CURL_SETOPT(c, opt, val)                           \
    do {                                                          \
        CURLcode res = curl_easy_setopt(c, CURLOPT_ ## opt, val); \
        if(CURLE_OK != res) {                                     \
            fprintf(stderr, "%s:%d libcurl error: %s\n",          \
                __FILE__, __LINE__, curl_easy_strerror(res));     \
            abort();                                              \
        }                                                         \
    } while(0)
#endif

HttpseRequest *
HttpseRequest_init(const char *url, const HttpseRequestOptions *options)
{
	HttpseRequest *rq = calloc(1, sizeof(*rq));
	if(!rq)
	{
		return HttpseRequest_cleanup(rq);
	}

	rq->url   = strdup(url);
	rq->curl  = curl_easy_init();
	rq->userp = HttpseVector_init();
	rq->hd    = HttpseVector_init();

	if(!rq->url || !rq->curl || !rq->userp || !rq->hd)
	{
		return HttpseRequest_cleanup(rq);
	}

	/* FIXME: Undocumented behaviors */
	HTTPSE_CURL_SETOPT(rq->curl, URL, rq->url);
	HTTPSE_CURL_SETOPT(rq->curl, FOLLOWLOCATION, 1L);
	HTTPSE_CURL_SETOPT(rq->curl, SSL_VERIFYPEER, 1L);
	HTTPSE_CURL_SETOPT(rq->curl, WRITEDATA, rq->userp);
	HTTPSE_CURL_SETOPT(rq->curl, WRITEFUNCTION, HttpseVector_append);
	HTTPSE_CURL_SETOPT(rq->curl, HEADERDATA, rq->hd);
	HTTPSE_CURL_SETOPT(rq->curl, HEADERFUNCTION, HttpseVector_append);
	
	/* Remark: See https://curl.haxx.se/libcurl/c/multithread.html */
	HTTPSE_CURL_SETOPT(rq->curl, NOSIGNAL, 1L);

	/* Remark: curl_easy_setopt() */
	HTTPSE_CURL_SETOPT(rq->curl, ACCEPT_ENCODING, 
		options->ac_encoding ? "" : NULL);

	HTTPSE_CURL_SETOPT(rq->curl, CAPATH, options->capath);
	HTTPSE_CURL_SETOPT(rq->curl, CONNECTTIMEOUT, options->connect_timeout);
	HTTPSE_CURL_SETOPT(rq->curl, TIMEOUT, options->timeout);
	HTTPSE_CURL_SETOPT(rq->curl, TRANSFER_ENCODING, options->tr_encoding);
	HTTPSE_CURL_SETOPT(rq->curl, USERAGENT, options->useragent);
	HTTPSE_CURL_SETOPT(rq->curl, VERBOSE, options->verbose);
	return rq;
}

CURLcode
HttpseRequest_perform(HttpseRequest *rq)
{
	if(rq)
	{
		return curl_easy_perform(rq->curl);
	}
	return CURLE_BAD_FUNCTION_ARGUMENT;
}

HttpseRequest *
HttpseRequest_cleanup(HttpseRequest *rq)
{
	if(rq)
	{
		if(rq->url)
		{
			free(rq->url);
			rq->url = NULL;
		}

		if(rq->curl)
		{
			curl_easy_cleanup(rq->curl);
			rq->curl = NULL;
		}

		if(rq->userp)
		{
			HttpseVector_cleanup(rq->userp);
			rq->userp = NULL;
		}
		
		if(rq->hd)
		{
			HttpseVector_cleanup(rq->hd);
			rq->hd = NULL;
		}
		free(rq);
	}
	return NULL;
}
