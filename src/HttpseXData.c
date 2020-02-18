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
	if(!xdatap)
	{
		return NULL;
	}

	HttpseCode retval  = HTTPSE_OK;
	HttpseTData *tdata = malloc(sizeof(*tdata));
	HttpseXData *xdata = xdatap;	
	
	if(!tdata)
	{
		xdata->error = HTTPSE_ERROR_UNKNOWN;
		return NULL;
	}

	if (!xdata->options->skip_hsts_check) {
		/* HSTS preload check */
		HttpseTData *hsts = malloc(sizeof(*hsts));
		snprintf(hsts->urls, HTTPSE_XDATA_BUFSZ, "https://hstspreload.org/api/v2/status?domain=%s", xdata->host);

		hsts->rs = HttpseRequest_init(hsts->urls, xdata->options);
		HttpseRequest_perform(hsts->rs);

		if (strstr(hsts->rs->userp->c_str, "\"status\": \"preloaded\"") != NULL) {
			xdata->error = HTTPSE_HSTS_PRELOADED;

			hsts->rs = HttpseRequest_cleanup(hsts->rs);
			free(hsts);
			return NULL;
		}

		hsts->rs = HttpseRequest_cleanup(hsts->rs);
		free(hsts);
	}

	snprintf(tdata->urlp, HTTPSE_XDATA_BUFSZ, "http://%s", xdata->host);
	snprintf(tdata->urls, HTTPSE_XDATA_BUFSZ, "https://%s", xdata->host);

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
		if(HTTPSE_OK != (retval = HttpseRequest_perform(tdata->rs)))
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
			break;
		}

		if(HTTPSE_OK != (retval = HttpseRequest_perform(tdata->rp)))
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

