#include "SecureFallback.h"

HttpseCode
httpse_check_secure_fallback(const HttpseTData *tdata)
{
	char *url = NULL;
	curl_easy_getinfo(tdata->rs->curl, CURLINFO_EFFECTIVE_URL, &url);

	if(url && 0 == strncmp(tdata->urlp, url, strlen(tdata->urlp)))
	{
		return HTTPSE_SECURE_FALLBACK;
	}
	return HTTPSE_OK;
}
