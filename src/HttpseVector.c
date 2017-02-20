#include "HttpseVector.h"

HttpseVector *
HttpseVector_init(void)
{
	HttpseVector *ctx = calloc(1, sizeof(*ctx));
	return ctx;
}

HttpseVector *
HttpseVector_init1(const void *ptr, size_t size, size_t nmemb)
{
	HttpseVector *ctx = HttpseVector_init();
	if(ctx && HttpseVector_append(ptr, size, nmemb, ctx) == size * nmemb)
	{
		return ctx;
	}
	return HttpseVector_cleanup(ctx);
}

size_t
HttpseVector_append(const void *ptr, size_t size, 
	size_t nmemb, HttpseVector *ctx)
{
	/* FIXME: Undocumented behavior */
	if(!ctx || !(size * nmemb))
	{
		return size * nmemb;
	}

	if(!ptr)
	{
		return 0L;
	}

	if(ctx->size + size * (nmemb + 1) > ctx->max_size)
	{
		size_t msz = MAX((ctx->max_size + size * (nmemb + 1)) * 2L, 1024L);
		void *data = malloc(msz);

		if(!data)
		{
			return 0L;
		}

		data = memcpy(data, ctx->data, ctx->size);
		free(ctx->data);

		ctx->max_size = msz;
		ctx->c_str = ctx->data = data;
	}

	memcpy(ctx->data + ctx->size, ptr, size * nmemb);
	memset(ctx->data + ctx->size + size * nmemb, 0, size);
	ctx->size += (size * nmemb);
	return size * nmemb;
}

size_t
HttpseVector_append1(const char *ptr, HttpseVector *ctx)
{
	return HttpseVector_append(ptr, sizeof(*ptr), strlen(ptr), ctx);
}

HttpseVector *
HttpseVector_cleanup(HttpseVector *ctx)
{
	if(ctx)
	{
		if(ctx->data)
		{
			free(ctx->data);
			ctx->data = NULL;
		}

		if(ctx->c_str)
		{
			ctx->c_str = NULL;
		}

		ctx->size = 0L;
		ctx->max_size = 0L;
		free(ctx);
	}
	return NULL;
}
