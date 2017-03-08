#include "../HttpseXData.h"

#ifndef HTTPSE_MIXED_CONTENT_H
#define HTTPSE_MIXED_CONTENT_H

/* See https://developer.mozilla.org/en-US/docs/Web/Security/Mixed_content
 * for more detail.
 */
HttpseCode
httpse_check_mixed_content(const HttpseTData *tdata);

#endif /* HTTPSE_MIXED_CONTENT_H */
