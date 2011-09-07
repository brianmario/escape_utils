// tell rbx not to use it's caching compat layer
// by doing this we're making a promize to RBX that
// we'll never modify the pointers we get back from RSTRING_PTR
#define RSTRING_NOT_MODIFIED

#include <ruby.h>
#if RB_CVAR_SET_ARITY == 4
#  define rb_cvar_set(a,b,c) rb_cvar_set(a,b,c,0)
#endif
#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
#endif

#include "houdini.h"

typedef void (*houdini_cb)(struct buf *, const uint8_t *, size_t);

static VALUE rb_mEscapeUtils;

/**
 * html_secure instance variable
 */
static ID rb_html_secure;
static int g_html_secure = 1;

static VALUE rb_eu_get_html_secure(VALUE self)
{
	return rb_cvar_get(self, rb_html_secure);
}

static VALUE rb_eu_set_html_secure(VALUE self, VALUE val)
{
	g_html_secure = RTEST(val);
	rb_cvar_set(self, rb_html_secure, val);
	return val;
}


/**
 * Generic template
 */
static VALUE
rb_eu__generic(
	VALUE self, VALUE str,
	houdini_cb callback,
	size_t chunk_size)
{
	VALUE result;
	struct buf *out_buf;

	if (NIL_P(str))
		return rb_str_new2("");

	Check_Type(str, T_STRING);
	out_buf = bufnew(chunk_size);

	callback(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
	result = rb_str_new((char *)out_buf->data, out_buf->size);
	bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
	rb_enc_copy(result, str);
#endif

	return result;
}


/**
 * HTML methods
 */
static VALUE rb_eu_escape_html(int argc, VALUE *argv, VALUE self)
{
	VALUE rb_out_buf, str, rb_secure;
	struct buf *out_buf;
	int secure = g_html_secure;

	if (rb_scan_args(argc, argv, "11", &str, &rb_secure) == 2) {
		if (rb_secure == Qfalse) {
			secure = 0;
		}
	}

	Check_Type(str, T_STRING);
	out_buf = bufnew(128);

	houdini_escape_html(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str), secure);

	rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
	bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
	rb_enc_copy(rb_out_buf, str);
#endif

	return rb_out_buf;
}

static VALUE rb_eu_unescape_html(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_unescape_html, 128);
}


/**
 * JavaScript methods
 */
static VALUE rb_eu_escape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_escape_js, 128);
}

static VALUE rb_eu_unescape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_unescape_js, 128);
}


/**
 * URL methods
 */
static VALUE rb_eu_escape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_escape_url, 32);
}

static VALUE rb_eu_unescape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_unescape_url, 32);
}


/**
 * URI methods
 */
static VALUE rb_eu_escape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_escape_uri, 32);
}

static VALUE rb_eu_unescape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(self, str, &houdini_unescape_uri, 32);
}


/**
 * Ruby Extension initializer
 */
void Init_escape_utils()
{
	rb_mEscapeUtils = rb_define_module("EscapeUtils");
	rb_define_method(rb_mEscapeUtils, "escape_html", rb_eu_escape_html, -1);
	rb_define_method(rb_mEscapeUtils, "unescape_html", rb_eu_unescape_html, 1);
	rb_define_method(rb_mEscapeUtils, "escape_javascript", rb_eu_escape_js, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_javascript", rb_eu_unescape_js, 1);
	rb_define_method(rb_mEscapeUtils, "escape_url", rb_eu_escape_url, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_url", rb_eu_unescape_url, 1);
	rb_define_method(rb_mEscapeUtils, "escape_uri", rb_eu_escape_uri, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_uri", rb_eu_unescape_uri, 1);

	rb_define_singleton_method(rb_mEscapeUtils, "html_secure", rb_eu_get_html_secure, 0);
	rb_define_singleton_method(rb_mEscapeUtils, "html_secure=", rb_eu_set_html_secure, 1);

	rb_html_secure = rb_intern("@@html_secure");
}

