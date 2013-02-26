// tell rbx not to use it's caching compat layer
// by doing this we're making a promise to RBX that
// we'll never modify the pointers we get back from RSTRING_PTR
#define RSTRING_NOT_MODIFIED

#include <ruby.h>
#include "houdini.h"

#if RB_CVAR_SET_ARITY == 4
#  define rb_cvar_set(a,b,c) rb_cvar_set(a,b,c,0)
#endif

#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
static VALUE rb_eEncodingCompatibilityError;

static VALUE eu_new_str(const char *str, size_t len)
{
	return rb_enc_str_new(str, len, rb_utf8_encoding());
}

static void check_utf8_encoding(VALUE str)
{
	static rb_encoding *_cached[3] = {NULL, NULL, NULL};
	rb_encoding *enc;

	if (_cached[0] == NULL) {
		_cached[0] = rb_utf8_encoding();
		_cached[1] = rb_usascii_encoding();
		_cached[2] = rb_ascii8bit_encoding();
	}

	enc = rb_enc_get(str);
	if (enc != _cached[0] && enc != _cached[1] && enc != _cached[2]) {
		rb_raise(rb_eEncodingCompatibilityError,
			"Input must be UTF-8 or US-ASCII, %s given", rb_enc_name(enc));
	}
}
#else
static VALUE eu_new_str(const char *str, size_t len)
{
	return rb_str_new(str, len);
}

static void check_utf8_encoding(VALUE str) {}
#endif

typedef int (*houdini_cb)(gh_buf *, const uint8_t *, size_t);

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
rb_eu__generic(VALUE str, houdini_cb do_escape)
{
	gh_buf buf = GH_BUF_INIT;

	if (NIL_P(str))
		return eu_new_str("", 0);

	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	if (do_escape(&buf, (const uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str))) {
		VALUE result = eu_new_str(buf.ptr, buf.size);
		gh_buf_free(&buf);
		return result;
	}

	return str;
}


/**
 * HTML methods
 */
static VALUE rb_eu_escape_html(int argc, VALUE *argv, VALUE self)
{
	VALUE str, rb_secure;
	gh_buf buf = GH_BUF_INIT;
	int secure = g_html_secure;

	if (rb_scan_args(argc, argv, "11", &str, &rb_secure) == 2) {
		if (rb_secure == Qfalse) {
			secure = 0;
		}
	}

	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	if (houdini_escape_html0(&buf, (const uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str), secure)) {
		VALUE result = eu_new_str(buf.ptr, buf.size);
		gh_buf_free(&buf);
		return result;
	}

	return str;
}

static VALUE rb_eu_unescape_html(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_html);
}


/**
 * XML methods
 */
static VALUE rb_eu_escape_xml(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_xml);
}


/**
 * JavaScript methods
 */
static VALUE rb_eu_escape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_js);
}

static VALUE rb_eu_unescape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_js);
}


/**
 * URL methods
 */
static VALUE rb_eu_escape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_url);
}

static VALUE rb_eu_unescape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_url);
}


/**
 * URI methods
 */
static VALUE rb_eu_escape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_uri);
}

static VALUE rb_eu_unescape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_uri);
}


/**
 * Ruby Extension initializer
 */
void Init_escape_utils()
{
#ifdef HAVE_RUBY_ENCODING_H
	VALUE rb_cEncoding = rb_const_get(rb_cObject, rb_intern("Encoding"));
	rb_eEncodingCompatibilityError = rb_const_get(rb_cEncoding, rb_intern("CompatibilityError"));
#endif

	rb_mEscapeUtils = rb_define_module("EscapeUtils");

	rb_define_method(rb_mEscapeUtils, "escape_html", rb_eu_escape_html, -1);
	rb_define_method(rb_mEscapeUtils, "unescape_html", rb_eu_unescape_html, 1);
	rb_define_method(rb_mEscapeUtils, "escape_xml", rb_eu_escape_xml, 1);
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

