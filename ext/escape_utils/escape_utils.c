#include "houdini.h"

#include <ruby.h>
#if RB_CVAR_SET_ARITY == 4
#  define rb_cvar_set(a,b,c) rb_cvar_set(a,b,c,0)
#endif
#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
#endif

static VALUE mEscapeUtils;
static ID rb_html_secure;
static int html_secure = 1;

static size_t escape_javascript(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar;

  total = in_len;
  while (in_len) {
    curChar = *in++;
    switch (curChar) {
    case '\\':
      *out++ = '\\'; *out++ = '\\';
      total++;
      break;
    case '<':
      *out++ = '<';
      if (*in == '/') {
        *out++ = '\\'; *out++ = '/';
        in++; in_len--;
        total++;
      }
      break;
    case '\r':
      if (*in == '\n') {
        *out++ = '\\'; *out++ = 'n';
        in++; in_len--;
      } else {
        *out++ = '\\'; *out++ = 'n';
        total++;
      }
      break;
    case '\n':
      *out++ = '\\'; *out++ = 'n';
      total++;
      break;
    case '\'':
      *out++ = '\\'; *out++ = '\'';
      total++;
      break;
    case '\"':
      *out++ = '\\'; *out++ = '\"';
      total++;
      break;
    default:
      *out++ = curChar;
      break;
    }
    in_len--;
  }

  return total;
}

static size_t unescape_javascript(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar;

  total = in_len;
  while (in_len) {
    curChar = *in++;
    if (curChar == '\\') {
      if (*in == 'n') {
        *out++ = '\n';
        total--;
      } else if (*in == '\\') {
        *out++ = '\\';
        total--;
      } else if (*in == '\'') {
        *out++ = '\'';
        total--;
      } else if (*in == '\"') {
        *out++ = '\"';
        total--;
      } else if (*in == '/') {
        *out++ = '/';
        total--;
      } else {
        /* incomplete escape, pass it through */
        *out++ = curChar;
        continue;
      }
      in++; in_len--;
    } else {
      *out++ = curChar;
    }
    in_len--;
  }

  return total;
}

static VALUE rb_escape_html(int argc, VALUE *argv, VALUE self) {
  VALUE rb_out_buf, str, rb_secure;
  struct buf *out_buf;
  int secure = html_secure;

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

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  VALUE rb_out_buf;
  struct buf *out_buf;

  Check_Type(str, T_STRING);
  out_buf = bufnew(128);

  houdini_unescape_html(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
  rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
  bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_out_buf, str);
#endif

  return rb_out_buf;
}

static VALUE rb_escape_javascript(VALUE self, VALUE str) {
  VALUE rb_output_buf;
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str);

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  new_len = sizeof(unsigned char)*(len*2);

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = escape_javascript(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_javascript(VALUE self, VALUE str) {
  VALUE rb_output_buf;
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str);

  // this is the max size the string could be
  // TODO: we could be more intelligent about this, but probably not
  new_len = sizeof(unsigned char) * len;

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = unescape_javascript(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_escape_url(VALUE self, VALUE str) {
  VALUE rb_out_buf;
  struct buf *out_buf;

  Check_Type(str, T_STRING);
  out_buf = bufnew(32);

  houdini_escape_url(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
  rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
  bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_out_buf, str);
#endif

  return rb_out_buf;
}

static VALUE rb_unescape_url(VALUE self, VALUE str) {
  VALUE rb_out_buf;
  struct buf *out_buf;

  Check_Type(str, T_STRING);
  out_buf = bufnew(32);

  houdini_unescape_url(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
  rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
  bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_out_buf, str);
#endif

  return rb_out_buf;
}

static VALUE rb_escape_uri(VALUE self, VALUE str) {
  VALUE rb_out_buf;
  struct buf *out_buf;

  Check_Type(str, T_STRING);
  out_buf = bufnew(32);

  houdini_escape_uri(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
  rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
  bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_out_buf, str);
#endif

  return rb_out_buf;
}

static VALUE rb_unescape_uri(VALUE self, VALUE str) {
  VALUE rb_out_buf;
  struct buf *out_buf;

  Check_Type(str, T_STRING);
  out_buf = bufnew(32);

  houdini_unescape_uri(out_buf, (uint8_t *)RSTRING_PTR(str), RSTRING_LEN(str));
  rb_out_buf = rb_str_new((char *)out_buf->data, out_buf->size);
  bufrelease(out_buf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_out_buf, str);
#endif

  return rb_out_buf;
}

static VALUE rb_s_get_html_secure(VALUE self) {
  return rb_cvar_get(self, rb_html_secure);
}

static VALUE rb_s_set_html_secure(VALUE self, VALUE val) {
  html_secure = RTEST(val);
  rb_cvar_set(self, rb_html_secure, val);

  return val;
}

/* Ruby Extension initializer */
void Init_escape_utils() {
  mEscapeUtils = rb_define_module("EscapeUtils");
  rb_define_method(mEscapeUtils, "escape_html", rb_escape_html, -1);
  rb_define_method(mEscapeUtils, "unescape_html", rb_unescape_html, 1);
  rb_define_method(mEscapeUtils, "escape_javascript", rb_escape_javascript, 1);
  rb_define_method(mEscapeUtils, "unescape_javascript", rb_unescape_javascript, 1);
  rb_define_method(mEscapeUtils, "escape_url", rb_escape_url, 1);
  rb_define_method(mEscapeUtils, "unescape_url", rb_unescape_url, 1);
  rb_define_method(mEscapeUtils, "escape_uri", rb_escape_uri, 1);
  rb_define_method(mEscapeUtils, "unescape_uri", rb_unescape_uri, 1);

  rb_define_singleton_method(mEscapeUtils, "html_secure", rb_s_get_html_secure, 0);
  rb_define_singleton_method(mEscapeUtils, "html_secure=", rb_s_set_html_secure, 1);

  rb_html_secure = rb_intern("@@html_secure");
}

