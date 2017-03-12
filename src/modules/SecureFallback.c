#include "gumbo.h"
#include "SecureFallback.h"

int
httpse_check_secure_fallback1(const GumboNode *node, const char *urlp)
{
	if(!node)
	{
		return 0;
	}

	if(node->type != GUMBO_NODE_ELEMENT)
	{
		return 0;
	}

	do
	{
		char mformat1[HTTPSE_XDATA_BUFSZ];
		char mformat2[HTTPSE_XDATA_BUFSZ];

		const GumboAttribute *http_equiv = NULL; 
		const GumboAttribute *content = NULL;

		if(node->v.element.tag != GUMBO_TAG_META)
		{
			break;
		}

		http_equiv = gumbo_get_attribute(&node->v.element
			.attributes, "http-equiv");

		content = gumbo_get_attribute(&node->v.element
			.attributes, "content");

		if(!http_equiv || !http_equiv->value || 
			0 != strcasecmp(http_equiv->value, "refresh"))
		{
			break;
		}

		if(!content || !content->value)
		{
			break;
		}

		snprintf(mformat1, HTTPSE_XDATA_BUFSZ, "url=%s", urlp);
		snprintf(mformat2, HTTPSE_XDATA_BUFSZ, "url=\'%s\'", urlp);

		if(NULL != strcasestr(content->value, mformat1))
		{
			return 1;
		}

		if(NULL != strcasestr(content->value, mformat2))
		{
			return 1;
		}
	} while(0);

	size_t i, children_len = node->v.element.children.length;
	const GumboNode **children = (void *) node->v.element.children.data;

	for(i = 0; i < children_len; ++i)
	{
		if(1 == httpse_check_secure_fallback1(children[i], urlp))
		{
			return 1;
		}
	}
	return 0;
}

HttpseCode
httpse_check_secure_fallback(const HttpseTData *tdata)
{
	/* Variables */
	int fallback = 0;

	const char *url  = NULL;
	const char *html = NULL;

	GumboOutput *output = NULL;


	/* Remark: Check 301/302 Redirects */
	curl_easy_getinfo(tdata->rs->curl, CURLINFO_EFFECTIVE_URL, &url);
	if(url && 0 == strncmp(tdata->urlp, url, strlen(tdata->urlp)))
	{
		return HTTPSE_SECURE_FALLBACK;
	}

	/* Remark: Check Meta Redirects */
	html = tdata->rs->userp->size ? tdata->rs->userp->c_str : "";
	output = gumbo_parse(html);

	if(output && output->root)
	{
		fallback = httpse_check_secure_fallback1(output->root, tdata->urlp);
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	return fallback ? HTTPSE_SECURE_FALLBACK : HTTPSE_OK;
}
