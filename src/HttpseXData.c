#include "HttpseXData.h"
#include "HttpseModules.h"

int
HttpseXData_compare(const void *xdatap1, const void *xdatap2)
{
	const HttpseXData *xdata1 = xdatap1;
	const HttpseXData *xdata2 = xdatap2;

	int derror = (int) xdata1->error - xdata2->error;
	int dindex = (int) xdata1->index - xdata2->index;

	return (0 == derror ? dindex : derror);
}

void *
HttpseXData_perform(void *xdatap)
{
	HttpseCode retval  = HTTPSE_OK;
	HttpseXData *xdata = xdatap;
	HttpseTData *tdata = NULL;

	if(NULL == xdata)
	{
		return NULL;
	}

	if (!xdata->options->skip_hsts_check) {
		/* HSTS preload check */
		HttpseVector *hstspreload_org_endpoint_url = HttpseVector_init();
		HttpseVector_append1("https://hstspreload.org/api/v2/status?domain=", hstspreload_org_endpoint_url);
		HttpseVector_append1(xdata->host, hstspreload_org_endpoint_url);

		HttpseRequest *hsts_check_request = HttpseRequest_init(hstspreload_org_endpoint_url->c_str, xdata->options);

		if (
			(HTTPSE_OK == HttpseRequest_perform(hsts_check_request)) &&
			(NULL != strstr(hsts_check_request->userp->c_str, "\"status\": \"preloaded\""))
		) {
			// domain is preloaded
			xdata->error = HTTPSE_HSTS_PRELOADED;
		}

		/* cleanup */
		hsts_check_request = HttpseRequest_cleanup(hsts_check_request);
		hstspreload_org_endpoint_url = HttpseVector_cleanup(hstspreload_org_endpoint_url);
		
		if (HTTPSE_HSTS_PRELOADED == xdata->error) {
			return NULL;
		}
	}

	if(NULL == (tdata = malloc(sizeof(HttpseTData))))
	{
		xdata->error = HTTPSE_ERROR_UNKNOWN;
		return NULL;
	}

	do {
		// construct test URLs safely
		// http://example.com/
		HttpseVector *http_url_with_host = HttpseVector_init();
		HttpseVector_append1("http://", http_url_with_host);
		HttpseVector_append1(xdata->host, http_url_with_host);

		strncpy(tdata->urlp, http_url_with_host->c_str, HTTPSE_XDATA_BUFSZ);
		http_url_with_host = HttpseVector_cleanup(http_url_with_host);

		// https://example.com/
		HttpseVector *https_url_with_host = HttpseVector_init();
		HttpseVector_append1("https://", https_url_with_host);
		HttpseVector_append1(xdata->host, https_url_with_host);

		strncpy(tdata->urls, https_url_with_host->c_str, HTTPSE_XDATA_BUFSZ);
		https_url_with_host = HttpseVector_cleanup(https_url_with_host);
	} while(0);

	tdata->rp = HttpseRequest_init(tdata->urlp, xdata->options);
	tdata->rs = HttpseRequest_init(tdata->urls, xdata->options);
	
	/* Remark: escape on error */
	if(!tdata->rp || !tdata->rs)
	{
		xdata->error = HTTPSE_ERROR_UNKNOWN;
		tdata->rp = HttpseRequest_cleanup(tdata->rp);
		tdata->rs = HttpseRequest_cleanup(tdata->rs);
		free(tdata);
		return NULL;
	}

	do
	{
		if(HTTPSE_OK != (retval = (HttpseCode) HttpseRequest_perform(tdata->rs)))
		{
			if(0 == strcmp(tdata->rs->errbuf, "SSL certificate problem: "
				"unable to get local issuer certificate"))
			{
				retval = HTTPSE_SSL_INCOMPLETE_CERT_CHAIN;
			}

			if(0 == strcmp(tdata->rs->errbuf, "SSL certificate problem: "
				"self signed certificate in certificate chain"))
			{
				retval = HTTPSE_SSL_SELF_SIGNED_CERT_CHAIN;
			}

			if(strstr(tdata->rs->errbuf, "SSL routines:ssl_choose_client_version:unsupported protocol"))
			{
				retval = HTTPSE_SSL_WEAK_ENCRYPTION;
			}
			break;
		}

		if(HTTPSE_OK != (retval = (HttpseCode) HttpseRequest_perform(tdata->rp)))
		{
			/* Remark: This host works on HTTPS only. */
			retval = HTTPSE_OK;
			break;
		}
	
		if(HTTPSE_OK != (retval = httpse_check_status_code(tdata)))
		{
			break;
		}

		if(HTTPSE_OK != (retval = httpse_check_secure_fallback(tdata)))
		{
			break;
		}
		
		/* Remark: Not Implemented. */
		if(HTTPSE_OK != (retval = httpse_check_different_content(tdata)))
		{
			break;
		}
		
		if(HTTPSE_OK != (retval = httpse_check_mixed_content(tdata)))
		{
			break;
		}
	} while(0);
	
	xdata->error = retval;

	/* Remark: cleanup */
	tdata->rp = HttpseRequest_cleanup(tdata->rp);
	tdata->rs = HttpseRequest_cleanup(tdata->rs);
	free(tdata);
	
	pthread_exit(NULL);
}

