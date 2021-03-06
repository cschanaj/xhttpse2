#include "StatusCode.h"

HttpseCode
httpse_check_status_code(const HttpseTData *tdata)
{
	long cp = 0;
	long cs = 0;


	curl_easy_getinfo(tdata->rp->curl, CURLINFO_RESPONSE_CODE, &cp);
	curl_easy_getinfo(tdata->rs->curl, CURLINFO_RESPONSE_CODE, &cs);


	if(cs == cp)
	{
		switch(cp/ 100)
		{
		/* Remark: unable to retrieve HTTP response code, skip tests */
		case 0:

		/* Remark: everything is OK */
		case 2:
		case 3:
			return HTTPSE_OK;

		case 4:
			return HTTPSE_STATUS_CODE_4XX;

		case 5:
			return HTTPSE_STATUS_CODE_5XX;
		
		default:
			return HTTPSE_STATUS_CODE_OTHERS;
		}
	}
	return HTTPSE_STATUS_CODE_MISMATCH;
}
