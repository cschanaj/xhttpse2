#include "HttpseCode.h"

const char *
HttpseCode_strerror(HttpseCode code)
{
	switch(code)
	{
	/* Remark: handle non curl-specified error first */
	case HTTPSE_HSTS_PRELOADED:
		return "HSTS preloaded";

	case HTTPSE_STATUS_CODE_MISMATCH:
		return "Status code mismatch";
	
	case HTTPSE_SSL_INCOMPLETE_CERT_CHAIN:
		return "Incomplete certificate chain error";
	
	case HTTPSE_SSL_SELF_SIGNED_CERT_CHAIN:
		return "Self-signed certificate chain error";

	case HTTPSE_SSL_WEAK_ENCRYPTION:
		return "Site uses weak encryption (TLSv1.1 or earlier)";
	
	case HTTPSE_STATUS_CODE_4XX:
		return "4xx client error";
	
	case HTTPSE_STATUS_CODE_5XX:
		return "5xx server error";
	
	case HTTPSE_STATUS_CODE_OTHERS:
		return "Non-200/3xx HTTP status code";

	case HTTPSE_SECURE_FALLBACK:
		return "Redirects to HTTP";
	
	case HTTPSE_DIFFERENT_CONTENT:
		return "Different content";
	
	case HTTPSE_MIXED_CONTENT:
		return "Mixed content blocking (MCB) triggered";

	case HTTPSE_ERROR_UNKNOWN:
		return "Unknown error";


	/* Remark: handle curl specified error */
	case CURLE_PEER_FAILED_VERIFICATION:
		/* See https://github.com/EFForg/https-everywhere/pull/8973 */
		return "SSL peer certificate was not OK";

	default:
		return curl_easy_strerror((CURLcode) code);
	}

	/* Remark: Unreachable code */
	return NULL;
}
