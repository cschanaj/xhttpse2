#include "gumbo.h"
#include "DifferentContent.h"

#include <math.h>

#ifndef TAG_VECTOR_SZ
#define TAG_VECTOR_SZ 160
#endif

#ifndef MATH_PI
#define MATH_PI 3.14159265
#endif

int 
httpse_check_different_content1(const GumboNode *node, double *v)
{
	if(!v)
	{
		return 0;
	}

	if(node && node->type == GUMBO_NODE_ELEMENT)
	{
		v[node->v.element.tag] += 1.0;

		size_t i, children_len = node->v.element.children.length;
		const GumboNode **children = (void *) node->v.element.children.data;

		for(i = 0; i < children_len; ++i)
		{
			if(0 == httpse_check_different_content1(children[i], v))
			{
				return 0;
			}
		}
	}
	return 1;
}

HttpseCode
httpse_check_different_content(const HttpseTData *tdata)
{
	/* Compare the effective URL(s) */
	const char *urls = NULL;
	const char *urlp = NULL;

	curl_easy_getinfo(tdata->rs->curl, CURLINFO_EFFECTIVE_URL, &urls);
	curl_easy_getinfo(tdata->rp->curl, CURLINFO_EFFECTIVE_URL, &urlp);

	if(urls && urlp && 0 == strcmp(urls, urlp))
	{
		return HTTPSE_OK;
	}


	/* Simple memcmp() of the response data */
	if(tdata->rs->userp->size == tdata->rp->userp->size)
	{
		if(0 == memcmp(tdata->rs->userp->data, tdata->rp->userp->data,
			tdata->rs->userp->size))
		{
			return HTTPSE_OK;
		}
	}


	/* Compute the cosine distance of the HTML tags */
	size_t i = 0; 
	const char *html = NULL;
	GumboOutput *output = NULL;

	double tag_vsl = 0.0;
	double tag_vpl = 0.0;

	double theta = 0.0;
	double distance = 0.0;

	double tag_vs[TAG_VECTOR_SZ];
	double tag_vp[TAG_VECTOR_SZ];

	for(i = 0; i < TAG_VECTOR_SZ; ++i)
	{
		tag_vs[i] = tag_vp[i] = 0.0;
	}

	do
	{
		html   = tdata->rs->userp->size ? tdata->rs->userp->c_str : NULL;
		output = html ? gumbo_parse(html) : NULL;

		if(output && output->root)
		{
			if(!httpse_check_different_content1(output->root, tag_vs))
			{
				return HTTPSE_ERROR_UNKNOWN;
			}
			gumbo_destroy_output(&kGumboDefaultOptions, output);
		}
	} while(0);

	do
	{
		html   = tdata->rp->userp->size ? tdata->rp->userp->c_str : NULL;
		output = html ? gumbo_parse(html) : NULL;

		if(output && output->root)
		{
			if(!httpse_check_different_content1(output->root, tag_vp))
			{
				return HTTPSE_ERROR_UNKNOWN;
			}
			gumbo_destroy_output(&kGumboDefaultOptions, output);
		}
	} while(0);

	for(i = 0; i < TAG_VECTOR_SZ; ++i)
	{
		tag_vsl += pow(tag_vs[i], 2.0);
		tag_vpl += pow(tag_vp[i], 2.0);
	}

	tag_vsl = sqrt(tag_vsl);
	tag_vpl = sqrt(tag_vpl);

	if(tag_vsl == 0.0 || tag_vpl == 0.0)
	{
		return HTTPSE_DIFFERENT_CONTENT;
	}

	for(i = 0; i < TAG_VECTOR_SZ; ++i)
	{
		double udiff = tag_vs[i]/ tag_vsl - tag_vp[i]/ tag_vpl;
		distance += pow(udiff, 2.0);
	}

	distance = sqrt(distance);
	theta = acos(distance);

	if(distance >= 0.5 || theta >= MATH_PI/ 4.0)
	{
		return HTTPSE_DIFFERENT_CONTENT;
	}
	return HTTPSE_OK;
}

#undef TAG_VECTOR_SZ
