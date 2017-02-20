#include "gumbo.h"
#include "MixedContent.h"

int
httpse_check_mixed_content1(const GumboNode *node)
{
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
		{
			GumboAttribute *attr = gumbo_get_attribute(&node->v.element
				.attributes, "src");

			if(attr && 0 == strncmp("http://", attr->value, 7))
			{
				return 1;
			}
		}

	default:
		break;
	}

	size_t i, children_len = node->v.element.children.length;
	const GumboNode **children = (void *) node->v.element.children.data;

	for(i = 0; i < children_len; ++i)
	{
		if(1 == httpse_check_mixed_content1(children[i]))
		{
			return 1;
		}
	}
	return 0;
}

HttpseCode
httpse_check_mixed_content(const HttpseTData *tdata)
{
	int mixed = 0;
	char *html = tdata->rs->userp->size ? tdata->rs->userp->c_str : "";

	GumboOutput *output = gumbo_parse(html);
	if(output)
	{
		mixed = httpse_check_mixed_content1(output->root);
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	return mixed ? HTTPSE_MIXED_CONTENT : HTTPSE_OK;
}
