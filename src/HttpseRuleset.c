#include "HttpseRuleset.h"

/* Forward declaration */
HttpseRuleset *
HttpseRuleset_perform1(HttpseRuleset *ru);

/* Forward declaration */
HttpseRuleset *
HttpseRuleset_perform2(HttpseRuleset *ru);

HttpseRuleset *
HttpseRuleset_init(const HttpseRulesetOptions *roptions)
{
	HttpseRuleset *ru = calloc(1, sizeof(*ru));
	if(!ru)
	{
		return NULL;
	}

	ru->roptions = roptions;
	ru->_xdata   = HttpseVector_init();
	ru->ctx      = HttpseVector_init();

	if(!ru->roptions || !ru->roptions->path || !ru->_xdata)
	{
		return HttpseRuleset_cleanup(ru);
	}


	char ptr[1024];
	size_t ptrlen = 0, index = 0;
	FILE *infile = fopen(ru->roptions->path, "rb");
	HttpseVector *infile_ctx = HttpseVector_init();

	if(!infile || !infile_ctx)
	{
		infile_ctx = HttpseVector_cleanup(infile_ctx);
		return HttpseRuleset_cleanup(ru);
	}

	while(!feof(infile) && (ptrlen = fread(ptr, 1, 1024, infile)) > 0)
	{
		if(ptrlen != HttpseVector_append(ptr, 1, ptrlen, infile_ctx))
		{
			infile_ctx = HttpseVector_cleanup(infile_ctx);
			return HttpseRuleset_cleanup(ru);
		}
	}

	fclose(infile);

	char *pch = strtok(infile_ctx->c_str, "\r\n");
	while(pch != NULL)
	{
		if(strlen(pch) ==  0)
		{
			continue;
		}

		HttpseXData xdata = {
			.options = &(ru->roptions->options),
			.index = index++,
			.error = HTTPSE_ERROR_UNKNOWN
		};

		strncpy(xdata.host, pch, HTTPSE_XDATA_BUFSZ);
		HttpseVector_append(&xdata, sizeof(xdata), 1, ru->_xdata);
		pch = strtok(NULL, "\r\n");
	}

	infile_ctx = HttpseVector_cleanup(infile_ctx);


	/* Remark: Perform test against all target, then sort ru->_xdata
	 */
	ru = HttpseRuleset_perform1(ru);

	/* Remark: Write the ruleset to ru->ctx
	 */
	ru = HttpseRuleset_perform2(ru);
	return ru;
}

HttpseRuleset *
HttpseRuleset_perform1(HttpseRuleset *ru)
{
	if(!ru)
	{
		return NULL;
	}

	/* Remark: pthread variables */
	size_t i, j;
	size_t num_threads = ru->roptions->num_threads;

	pthread_t tids[num_threads];

	/* Remark: variables */
	HttpseXData *xdata = ru->_xdata->data;
	size_t xdatalen = ru->_xdata->size/ sizeof(HttpseXData);

	for(i = 0; i < xdatalen; i += j)
	{
		for(j = 0; j < num_threads && i + j < xdatalen; ++j)
		{
			int rc = pthread_create(&tids[j], NULL, 
				HttpseXData_perform, &xdata[i + j]);
		}

		for(j = 0; j < num_threads && i + j < xdatalen; ++j)
		{
			int rc = pthread_join(tids[j], NULL);
		}
	}

	qsort(xdata, xdatalen, sizeof(HttpseXData), HttpseXData_compare);
	return ru;
}

HttpseRuleset *
HttpseRuleset_perform2(HttpseRuleset *ru)
{
	if(!ru)
	{
		return NULL;
	}

	static const char *rformats[] = {
		"<ruleset name=\"%s\">\n",
		"<ruleset name=\"%s\" platform=\"%s\">\n",
		"\t<target host=\"%s\" />\n",
		"\n\t<securecookie host=\".+\" name=\".+\" />\n",
		"\n\t<rule from=\"^http:\" to=\"https:\" />\n",
		"</ruleset>\n"
	};

	static const char *cformats[] = {
		"<!--\n\tNon-functional hosts",
		"\n\t\t%s:\n",
		"\t\t\t - %s\n",
		"-->\n"
	};

	/* Remark: variables */
	size_t i = 0;
	char lbuf[HTTPSE_XDATA_BUFSZ];

	const HttpseXData *xdata = ru->_xdata->data;
	size_t xdatalen = ru->_xdata->size/ sizeof(HttpseXData);

	/* xml content */
	HttpseVector *v1 = HttpseVector_init();
	/* comment content */
	HttpseVector *v2 = HttpseVector_init();

	if(!v1 || !v2)
	{
		v1 = HttpseVector_cleanup(v1);
		v2 = HttpseVector_cleanup(v2);
		return HttpseRuleset_cleanup(ru);
	}

	/* Xml content: Part 1
	 */
	do
	{
		const char *name = ru->roptions->name ? ru->roptions->name : "";
		if(!ru->roptions->platform)
		{
			snprintf(lbuf, HTTPSE_XDATA_BUFSZ, rformats[0], name);
		}
		else
		{
			snprintf(lbuf, HTTPSE_XDATA_BUFSZ, rformats[1], name, 
				ru->roptions->platform);
		}
		HttpseVector_append1(lbuf, v1);
	} while(0);


	/* Xml content: Part 2
	 */
	for(i = 0; i < xdatalen && HTTPSE_OK == xdata[i].error; ++i)
	{
		snprintf(lbuf, HTTPSE_XDATA_BUFSZ, rformats[2], xdata[i].host);
		HttpseVector_append1(lbuf, v1);
	}


	/* Xml content: Part 3 (done)
	 */
	do
	{
		const char *sc = ru->roptions->securecookie ? rformats[3] : "";
		snprintf(lbuf, HTTPSE_XDATA_BUFSZ, "%s%s%s", sc, 
			rformats[4], rformats[5]);

		HttpseVector_append1(lbuf, v1);
	} while(0);


	/* Comment content
	 */
	if(i < xdatalen)
	{
		HttpseVector_append1(cformats[0], v2);
		while(i < xdatalen)
		{
			const char *msg = HttpseCode_strerror(xdata[i].error);

			snprintf(lbuf, HTTPSE_XDATA_BUFSZ, cformats[1], msg);
			HttpseVector_append1(lbuf, v2);

			snprintf(lbuf, HTTPSE_XDATA_BUFSZ, cformats[2], xdata[i].host);
			HttpseVector_append1(lbuf, v2);


			while(i + 1 < xdatalen && xdata[i].error == xdata[i + 1].error)
			{
				snprintf(lbuf, HTTPSE_XDATA_BUFSZ, cformats[2], 
					xdata[i + 1].host);

				HttpseVector_append1(lbuf, v2);
				++i;
			}
			++i;
		}
		HttpseVector_append1(cformats[3], v2);
	}

	HttpseVector_append(v2->data, 1, v2->size, ru->ctx);
	HttpseVector_append(v1->data, 1, v1->size, ru->ctx);

	v1 = HttpseVector_cleanup(v1);
	v2 = HttpseVector_cleanup(v2);
	return ru;
}

HttpseRuleset *
HttpseRuleset_cleanup(HttpseRuleset *ru)
{
	if(ru)
	{
		if(ru->_xdata)
		{
			HttpseVector_cleanup(ru->_xdata);
			ru->_xdata = NULL;
		}

		if(ru->ctx)
		{
			HttpseVector_cleanup(ru->ctx);
			ru->ctx = NULL;
		}
		free(ru);
	}
	return NULL;
}
