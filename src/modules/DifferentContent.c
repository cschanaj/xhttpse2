#include "gumbo.h"
#include "DifferentContent.h"

#include <math.h>

/* Remark: Levenshtein distance should have been used. Yet,
 * due to the sizes of the response bodies, this is not really
 * practical. If you know any fast implementation of the distance
 * function, on 30KB+ text content, feel free to submit a PR to 
 * replace the current implementation.
 * 
 * Now we the Euclidean Distance of the tags.
 * FIXME: It is possible that http(s) serve different content, but
 * the secure version is working correctly (false-positive).
 */


#ifndef HTTPSE_TAG_VECTOR_SZ
#define HTTPSE_TAG_VECTOR_SZ 160
#endif

int
httpse_check_different_content1(const GumboNode *node, double *tag_pt)
{
	if(!tag_pt)
	{
		return 0;
	}

	/* Remark: root == NULL iff recursion ends */
	if(node && node->type == GUMBO_NODE_ELEMENT)
	{
		tag_pt[node->v.element.tag] += 1.0;

		size_t i, children_len = node->v.element.children.length;
		const GumboNode **children = (void *) node->v.element.children.data;

		for(i = 0; i < children_len; ++i)
		{
			httpse_check_different_content1(children[i], tag_pt);
		}
	}
	return 1;
}

HttpseCode
httpse_check_different_content(const HttpseTData *tdata)
{
	size_t i = 0;

	double tag_pts[HTTPSE_TAG_VECTOR_SZ];
	double tag_ptp[HTTPSE_TAG_VECTOR_SZ];

	double distance = 0.0;

	const char  *html   = NULL;
	GumboOutput *output = NULL;

	for(i = 0; i < HTTPSE_TAG_VECTOR_SZ; ++i)
	{
		tag_pts[i] = tag_ptp[i] = 0.0;
	}

	do
	{
		html   = tdata->rs->userp->size ? tdata->rs->userp->c_str : "";
		output = gumbo_parse(html);

		if(output && output->root)
		{
			if(!httpse_check_different_content1(output->root, tag_pts))
			{
				return HTTPSE_ERROR_UNKNOWN;
			}
			gumbo_destroy_output(&kGumboDefaultOptions, output);
		}
	} while(0);

	do
	{
		html   = tdata->rp->userp->size ? tdata->rp->userp->c_str : "";
		output = gumbo_parse(html);

		if(output && output->root)
		{
			if(!httpse_check_different_content1(output->root, tag_ptp))
			{
				return HTTPSE_ERROR_UNKNOWN;
			}
			gumbo_destroy_output(&kGumboDefaultOptions, output);
		}
	} while(0);

	/* Remark: vector length */
	double tag_pts_len = 0.0;
	double tag_ptp_len = 0.0;

	for(i = 0; i < HTTPSE_TAG_VECTOR_SZ; ++i)
	{
		tag_pts_len += pow(tag_pts[i], 2);
		tag_ptp_len += pow(tag_ptp[i], 2);
	}

	tag_pts_len = sqrt(tag_pts_len);
	tag_ptp_len = sqrt(tag_ptp_len);

	/* Remark: both pages do not have any node */
	if(tag_pts_len == 0.0 && tag_ptp_len == 0.0)
	{
		return HTTPSE_OK;
	}

	/* Remark: only 1 pages have node */
	if(tag_pts_len == 0.0 || tag_ptp_len == 0.0)
	{
		return HTTPSE_DIFFERENT_CONTENT;
	}

	/* Remark: both pages have some nodes */
	for(i = 0; i < HTTPSE_TAG_VECTOR_SZ; ++i)
	{
		tag_pts[i] /= tag_pts_len;
		tag_ptp[i] /= tag_ptp_len;
	}


	/* Remark: Compute distance between 2 vectors */
	for(i = 0; i < HTTPSE_TAG_VECTOR_SZ; ++i)
	{
		distance += pow(tag_pts[i] - tag_ptp[i], 2);
	}

	if(distance >= 0.5)
	{
		return HTTPSE_DIFFERENT_CONTENT;
	}
	return HTTPSE_OK;
}
