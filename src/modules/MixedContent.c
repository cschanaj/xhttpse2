#include "gumbo.h"
#include "MixedContent.h"

int
httpse_check_mixed_content1(const GumboNode *node)
{
	int mixed = 0;

	const GumboAttribute *attr = NULL;
	const GumboAttribute *rel  = NULL;


	if(!node)
	{
		return 0;
	}

	if(node->type != GUMBO_NODE_ELEMENT)
	{
		return 0;
	}


	switch(node->v.element.tag)
	{
	case GUMBO_TAG_SCRIPT:
	case GUMBO_TAG_IFRAME:
		/* <script>, <iframe> (src attribute) */
		attr = gumbo_get_attribute(&node->v.element.attributes, "src");

		if(attr && 0 == strncmp("http://", attr->value, 7))
		{
			return 1;
		}
		break;

	case GUMBO_TAG_LINK:
		/* <link> (href attribute) */
		/* Remark: See https://www.w3schools.com/tags/att_link_rel.asp */
		rel  = gumbo_get_attribute(&node->v.element.attributes, "rel");
		attr = gumbo_get_attribute(&node->v.element.attributes, "href");

		if(rel && NULL != strcasestr(rel->value, "icon|stylesheet"))
		{
			if(attr && 0 == strncmp("http://", attr->value, 7))
			{
				return 1;
			}
		}
		break;
	
	case GUMBO_TAG_OBJECT:
		/* <object> (data attribute) */
		attr = gumbo_get_attribute(&node->v.element.attributes, "data");

		if(attr && 0 == strncmp("http://", attr->value, 7))
		{
			return 1;
		}
		break;
	
	default:
		break;
	}


	size_t i, children_len = node->v.element.children.length;
	const GumboNode **children = (void *) node->v.element.children.data;

	for(i = 0; i < children_len && !mixed; ++i)
	{
		mixed = httpse_check_mixed_content1(children[i]);
	}
	return mixed ? HTTPSE_MIXED_CONTENT : HTTPSE_OK;
}

HttpseCode
httpse_check_mixed_content(const HttpseTData *tdata)
{
	int mixed = 0;

	const char *url  = NULL;
	const char *html = NULL;

	GumboOutput *output = NULL;

	/* Remark: Effective URL is over HTTP, Mixed Content never happen
	 */
	curl_easy_getinfo(tdata->rs->curl, CURLINFO_EFFECTIVE_URL, &url);
	if(url && url == strstr(url, "http://"))
	{
		return HTTPSE_OK;
	}

	html = tdata->rs->userp->size ? tdata->rs->userp->c_str : "";
	output = gumbo_parse(html);

	if(output && output->root)
	{
		mixed = httpse_check_mixed_content1(output->root);
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	return mixed ? HTTPSE_MIXED_CONTENT : HTTPSE_OK;
}
