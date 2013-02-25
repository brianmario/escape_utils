// tell rbx not to use it's caching compat layer
// by doing this we're making a promise to RBX that
// we'll never modify the pointers we get back from RSTRING_PTR
#define RSTRING_NOT_MODIFIED

#include <ruby.h>
#if RB_CVAR_SET_ARITY == 4
#  define rb_cvar_set(a,b,c) rb_cvar_set(a,b,c,0)
#endif
#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
static VALUE rb_eEncodingCompatibilityError;
static VALUE eu_new_str(const char *str, size_t len) {
	return rb_enc_str_new(str, len, rb_utf8_encoding());
}
#else
static VALUE eu_new_str(const char *str, size_t len) {
	return rb_str_new(str, len);
}
#endif

static inline void check_utf8_encoding(VALUE str) {
#ifdef HAVE_RUBY_ENCODING_H
	rb_encoding *enc;

	enc = rb_enc_get(str);
	if (enc != rb_utf8_encoding() && enc != rb_usascii_encoding()) {
		rb_raise(rb_eEncodingCompatibilityError, "Input must be UTF-8 or US-ASCII, %s given", rb_enc_name(enc));
	}
#endif
}

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
static inline VALUE
rb_eu__generic(VALUE str, houdini_cb callback, size_t chunk_size)
{
	VALUE result;
	struct buf out_buf;

	if (NIL_P(str))
		return eu_new_str("", 0);

	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	bufinit(&out_buf, chunk_size);

	callback(&out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
	result = eu_new_str((const char *)out_buf.data, out_buf.size);
	bufrelease(&out_buf);

	return result;
}


/**
 * HTML methods
 */
static VALUE rb_eu_escape_html(int argc, VALUE *argv, VALUE self)
{
	VALUE rb_out_buf, str, rb_secure;
	struct buf out_buf;
	int secure = g_html_secure;

	if (rb_scan_args(argc, argv, "11", &str, &rb_secure) == 2) {
		if (rb_secure == Qfalse) {
			secure = 0;
		}
	}

	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	bufinit(&out_buf, 128);

	houdini_escape_html0(&out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str), secure);

	rb_out_buf = eu_new_str((const char *)out_buf.data, out_buf.size);

	bufrelease(&out_buf);

	return rb_out_buf;
}

static VALUE rb_eu_unescape_html(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_html, 128);
}


/**
 * XML methods
 */
static VALUE rb_eu_escape_xml(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_xml, 128);
}


/**
 * JavaScript methods
 */
static VALUE rb_eu_escape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_js, 128);
}

static VALUE rb_eu_unescape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_js, 128);
}


/**
 * URL methods
 */
static VALUE rb_eu_escape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_url, 32);
}

static VALUE rb_eu_unescape_url(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_url, 32);
}


/**
 * URI methods
 */
static VALUE rb_eu_escape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_uri, 32);
}

static VALUE rb_eu_unescape_uri(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_uri, 32);
}


/**
 * Ruby Extension initializer
 */
void Init_escape_utils()
{
	rb_mEscapeUtils = rb_define_module("EscapeUtils");

#ifdef HAVE_RUBY_ENCODING_H
	VALUE rb_cEncoding = rb_const_get(rb_cObject, rb_intern("Encoding"));
	rb_eEncodingCompatibilityError = rb_const_get(rb_cEncoding, rb_intern("CompatibilityError"));
#endif

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

