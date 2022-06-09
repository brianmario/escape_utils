// tell rbx not to use it's caching compat layer
// by doing this we're making a promise to RBX that
// we'll never modify the pointers we get back from RSTRING_PTR
#define RSTRING_NOT_MODIFIED

#include <ruby.h>
#include <ruby/encoding.h>
#include "houdini.h"

static VALUE rb_eEncodingCompatibilityError;

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

typedef int (*houdini_cb)(gh_buf *, const uint8_t *, size_t);

/**
 * Generic template
 */
static VALUE
rb_eu__generic(VALUE str, houdini_cb do_escape)
{
	gh_buf buf = GH_BUF_INIT;

	if (NIL_P(str))
		return rb_utf8_str_new("", 0);

	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	if (do_escape(&buf, (const uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str))) {
		VALUE result = rb_utf8_str_new(buf.ptr, buf.size);
		gh_buf_free(&buf);
		return result;
	}

	return str;
}


/**
 * HTML methods
 */

static VALUE rb_eu_escape_html_once(VALUE self, VALUE str)
{
	gh_buf buf = GH_BUF_INIT;
	Check_Type(str, T_STRING);
	check_utf8_encoding(str);

	if (houdini_escape_html_once(&buf, (const uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str))) {
		VALUE result = rb_utf8_str_new(buf.ptr, buf.size);
		gh_buf_free(&buf);
		return result;
	}

	return str;
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
	return rb_eu__generic(rb_obj_as_string(str), &houdini_escape_js);
}

static VALUE rb_eu_unescape_js(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_js);
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
 * URI component methods
 */
static VALUE rb_eu_escape_uri_component(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_escape_uri_component);
}

static VALUE rb_eu_unescape_uri_component(VALUE self, VALUE str)
{
	return rb_eu__generic(str, &houdini_unescape_uri_component);
}

/**
 * Ruby Extension initializer
 */
__attribute__((visibility("default")))
void Init_escape_utils()
{
	rb_eEncodingCompatibilityError = rb_const_get(rb_cEncoding, rb_intern("CompatibilityError"));

	VALUE rb_mEscapeUtils = rb_define_module("EscapeUtils");
	rb_define_method(rb_mEscapeUtils, "escape_html_once", rb_eu_escape_html_once, 1);
	rb_define_method(rb_mEscapeUtils, "escape_xml", rb_eu_escape_xml, 1);
	rb_define_method(rb_mEscapeUtils, "escape_javascript", rb_eu_escape_js, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_javascript", rb_eu_unescape_js, 1);
	rb_define_method(rb_mEscapeUtils, "escape_uri", rb_eu_escape_uri, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_uri", rb_eu_unescape_uri, 1);
	rb_define_method(rb_mEscapeUtils, "escape_uri_component", rb_eu_escape_uri_component, 1);
	rb_define_method(rb_mEscapeUtils, "unescape_uri_component", rb_eu_unescape_uri_component, 1);
}

