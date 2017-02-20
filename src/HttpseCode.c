#include "HttpseCode.h"

const char *
HttpseCode_strerror(HttpseCode code)
{
	if(code < HTTPSE_OBSOLETE)
	{
		return curl_easy_strerror((CURLcode) code);
	}

	switch(code)
	{
	case HTTPSE_STATUS_CODE_MISMATCH:
		return "Status code mismatch";
	
	case HTTPSE_STATUS_CODE_4XX:
		return "4xx client error";
	
	case HTTPSE_STATUS_CODE_5XX:
		return "5xx server error";
	
	case HTTPSE_STATUS_CODE_OTHERS:
		return "Non-200/3xx HTTP status code";

	case HTTPSE_SECURE_FALLBACK:
		return "Secure connection redirects to plaintext";
	
	case HTTPSE_DIFFERENT_CONTENT:
		return "Different content";
	
	case HTTPSE_MIXED_CONTENT:
		return "Mixed content blocking (MCB) tiggered";

	default:
		return "Unknown error";
	}

	/* Remark: Unreachable code */
	return NULL;
}
