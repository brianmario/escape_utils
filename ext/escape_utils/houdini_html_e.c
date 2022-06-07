#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "houdini.h"

/**
 * According to the OWASP rules:
 *
 * & --> &amp;
 * < --> &lt;
 * > --> &gt;
 * " --> &quot;
 * ' --> &#x27;     &apos; is not recommended
 * / --> &#x2F;     forward slash is included as it helps end an HTML entity
 *
 */
static const char HTML_ESCAPE_TABLE[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const char *HTML_ESCAPES[] = {
	"",
	"&quot;",
	"&amp;",
	"&#39;",
	"&lt;",
	"&gt;"
};

static const int HTML_ESCAPES_LENGTHS[] = {
	0,
	6,
	5,
	5,
	4,
	4
};

static int
is_entity(const uint8_t *src, size_t size)
{
	size_t i = 0;

	if (size == 0 || src[0] != '&')
		return false;

	if (size > 16)
		size = 16;

	if (size >= 4 && src[1] == '#') {
		if (_isdigit(src[2])) {
			for (i = 3; i < size && _isdigit(src[i]); ++i);
		}
		else if ((src[2] == 'x' || src[2] == 'X') && _isxdigit(src[3])) {
			for (i = 4; i < size && _isxdigit(src[i]); ++i);
		}
		else return false;
	}
	else {
		for (i = 1; i < size && _isasciialpha(src[i]); ++i);
		if (i == 1) return false;
	}

	return i < size && src[i] == ';';
}

int
houdini_escape_html_once(gh_buf *ob, const uint8_t *src, size_t size)
{
	size_t  i = 0, org, esc = 0;

	while (i < size) {
		org = i;
		while (i < size) {
			esc = HTML_ESCAPE_TABLE[src[i]];
			if (unlikely(esc != 0) && !is_entity(src + i, size - i))
				break;
			i++;
		}

		if (i > org) {
			if (unlikely(org == 0)) {
				if (i >= size)
					return 0;

				gh_buf_grow(ob, HOUDINI_ESCAPED_SIZE(size));
			}

			gh_buf_put(ob, src + org, i - org);
		}

		/* escaping */
		if (unlikely(i >= size))
			break;

		gh_buf_put(ob, HTML_ESCAPES[esc], HTML_ESCAPES_LENGTHS[esc]);

		i++;
	}

	return 1;
}
