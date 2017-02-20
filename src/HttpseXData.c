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
			break;
		}

		if(HTTPSE_OK != (retval = HttpseRequest_perform(tdata->rp)))
		{
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

