#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "houdini.h"

#define hex2c(c) ((c | 32) % 39 - 9)

static int
unescape(gh_buf *ob, const uint8_t *src, size_t size)
{
	size_t  i = 0, org;

	while (i < size) {
		org = i;
		while (i < size && src[i] != '%')
			i++;

		if (likely(i > org)) {
			if (unlikely(org == 0)) {
				if (i >= size)
					return 0;

				gh_buf_grow(ob, HOUDINI_UNESCAPED_SIZE(size));
			}

			gh_buf_put(ob, src + org, i - org);
		}

		/* escaping */
		if (i >= size)
			break;

		i++;
		if (i + 1 < size && _isxdigit(src[i]) && _isxdigit(src[i + 1])) {
			unsigned char new_char = (hex2c(src[i]) << 4) + hex2c(src[i + 1]);
			gh_buf_putc(ob, new_char);
			i += 2;
		} else {
			gh_buf_putc(ob, '%');
		}
	}

	return 1;
}

int
houdini_unescape_uri(gh_buf *ob, const uint8_t *src, size_t size)
{
	return unescape(ob, src, size);
}

int
houdini_unescape_uri_component(gh_buf *ob, const uint8_t *src, size_t size)
{
	return unescape(ob, src, size);
}

