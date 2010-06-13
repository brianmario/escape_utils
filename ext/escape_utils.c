#include <ruby.h>
#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
static rb_encoding *utf8Encoding;
#endif

#define APPEND_BUFFER(escape, len, scoot_by)  \
  memcpy(&out[total], &in[offset], i-offset); \
  total += i-offset;                          \
  offset = i+scoot_by;                        \
  memcpy(&out[total], escape, len);           \
  total += len;                               \

static size_t escape_html(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar;

  total = in_len;
  while (in_len) {
    curChar = *in++;
    switch (curChar) {
    case '<':
      *out++ = '&'; *out++ = 'l'; *out++ = 't'; *out++ = ';';
      total += 3;
      break;
    case '>':
      *out++ = '&'; *out++ = 'g'; *out++ = 't'; *out++ = ';';
      total += 3;
      break;
    case '&':
      *out++ = '&'; *out++ = 'a'; *out++ = 'm'; *out++ = 'p'; *out++ = ';';
      total += 4;
      break;
    case '\'':
      *out++ = '&'; *out++ = '#'; *out++ = '3'; *out++ = '9'; *out++ = ';';
      total += 4;
      break;
    case '\"':
      *out++ = '&'; *out++ = 'q'; *out++ = 'u'; *out++ = 'o'; *out++ = 't'; *out++ = ';';
      total += 5;
      break;
    default:
      *out++ = curChar;
      break;
    }
    in_len--;
  }

  return total;
}

static size_t unescape_html(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar;

  total = in_len;
  while (in_len) {
    curChar = *in++;
    if (curChar == '&') {
      if (*in == 'l' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '<';
        total-=3;
        in+=3;
        in_len-=3;
      } else if (*in == 'g' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '>';
        total-=3;
        in+=3;
        in_len-=3;
      } else if (*in == 'a' && *(in+1) == 'm' && *(in+2) == 'p' && *(in+3) == ';') {
        *out++ = '&';
        total-=4;
        in+=4;
        in_len-=4;
      } else if (*in == '#' && *(in+1) == '3' && *(in+2) == '9' && *(in+3) == ';') {
        *out++ = '\'';
        total-=4;
        in+=4;
        in_len-=4;
      } else if (*in == 'q' && *(in+1) == 'u' && *(in+2) == 'o' && *(in+3) == 't' && *(in+4) == ';') {
        *out++ = '\"';
        total-=5;
        in+=5;
        in_len-=5;
      }
    } else {
      *out++ = curChar;
    }
    in_len--;
  }

  return total;
}

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
      if (*in == '/') {
        *out++ = '<'; *out++ = '\\'; *out++ = '/';
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
        *out++ = curChar;
      }
      in++; in_len--;
    } else {
      *out++ = curChar;
    }
    in_len--;
  }

  return total;
}

static VALUE rb_escape_html(VALUE self, VALUE str) {
  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc = rb_default_internal_encoding();
  rb_encoding *original_encoding = rb_enc_get(str);
#endif
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  unsigned char *outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*5));

  // perform our escape, returning the new string's length
  new_len = escape_html(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  } else {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, utf8Encoding);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc = rb_default_internal_encoding();
  rb_encoding *original_encoding = rb_enc_get(str);
#endif
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  unsigned char *outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*len);

  // perform our escape, returning the new string's length
  new_len = unescape_html(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  } else {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, utf8Encoding);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_escape_javascript(VALUE self, VALUE str) {
  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc = rb_default_internal_encoding();
  rb_encoding *original_encoding = rb_enc_get(str);
#endif
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  unsigned char *outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*2));

  // perform our escape, returning the new string's length
  new_len = escape_javascript(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  } else {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, utf8Encoding);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_javascript(VALUE self, VALUE str) {
  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc = rb_default_internal_encoding();
  rb_encoding *original_encoding = rb_enc_get(str);
#endif
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  unsigned char *outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*len);

  // perform our escape, returning the new string's length
  new_len = unescape_javascript(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  } else {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, utf8Encoding);
  }
#endif
  return rb_output_buf;
}

/* Ruby Extension initializer */
void Init_escape_utils_ext() {
  VALUE mEscape = rb_define_module("EscapeUtils");
  rb_define_method(mEscape,           "escape_html",    rb_escape_html,   1);
  rb_define_module_function(mEscape,  "escape_html",    rb_escape_html,   1);
  rb_define_method(mEscape,           "unescape_html",  rb_unescape_html, 1);
  rb_define_module_function(mEscape,  "unescape_html",  rb_unescape_html, 1);
  rb_define_method(mEscape,           "escape_javascript",  rb_escape_javascript, 1);
  rb_define_module_function(mEscape,  "escape_javascript",  rb_escape_javascript, 1);
  rb_define_method(mEscape,           "unescape_javascript",  rb_unescape_javascript, 1);
  rb_define_module_function(mEscape,  "unescape_javascript",  rb_unescape_javascript, 1);

#ifdef HAVE_RUBY_ENCODING_H
  utf8Encoding = rb_utf8_encoding();
#endif
}