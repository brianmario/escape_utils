#include <ruby.h>
#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
#endif

static VALUE mEscapeUtils;
static ID rb_html_secure;

#define IS_HEX(c) (c >= 48 || c <= 57) && (c >= 65 || c <= 70) && (c >= 97 || c <= 102)
#define NOT_HEX(c) (c < 48 || c > 57) && (c < 65 || c > 90) && (c < 97 || c > 122)
#define UNHEX(c) (c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : c - 'a' + 10)
#define URI_SAFE(c) (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c== 45 || c == 46 || c == 95 || c == 126
//                  ALPHA / DIGIT / "-" / "." / "_" / "~"

static size_t escape_html(unsigned char *out, const unsigned char *in, size_t in_len, unsigned short int secure) {
  size_t total = 0;
  unsigned char curChar;

  total = in_len;
  while (in_len) {
    curChar = *in++;

    if (curChar == '<') {
      *out++ = '&'; *out++ = 'l'; *out++ = 't'; *out++ = ';';
      total += 3;
    } else if (curChar == '>') {
      *out++ = '&'; *out++ = 'g'; *out++ = 't'; *out++ = ';';
      total += 3;
    } else if (curChar == '&') {
      *out++ = '&'; *out++ = 'a'; *out++ = 'm'; *out++ = 'p'; *out++ = ';';
      total += 4;
    } else if (curChar == '\'') {
      *out++ = '&'; *out++ = '#'; *out++ = '3'; *out++ = '9'; *out++ = ';';
      total += 4;
    } else if (curChar == '\"') {
      *out++ = '&'; *out++ = 'q'; *out++ = 'u'; *out++ = 'o'; *out++ = 't'; *out++ = ';';
      total += 5;
    } else if (secure && curChar == '/') {
      *out++ = '&'; *out++ = '#'; *out++ = '4'; *out++ = '7'; *out++ = ';';
      total += 4;
    } else {
      *out++ = curChar;
    }
    in_len--;
  }

  return total;
}

static size_t unescape_html(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0, len = in_len;
  unsigned char curChar, *start;

  start = (unsigned char *)&in[0];
  total = in_len;
  while (len) {
    curChar = *in++;
    if (curChar == '&') {
      if ((in-start)+2 <= in_len && *in == 'l' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '<';
        total-=3;
        in+=3;
        len-=3;
      } else if ((in-start)+2 <= in_len && *in == 'g' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '>';
        total-=3;
        in+=3;
        len-=3;
      } else if ((in-start)+3 <= in_len && *in == 'a' && *(in+1) == 'm' && *(in+2) == 'p' && *(in+3) == ';') {
        *out++ = '&';
        total-=4;
        in+=4;
        len-=4;
      } else if ((in-start)+3 <= in_len && *in == '#' && *(in+1) == '3' && *(in+2) == '9' && *(in+3) == ';') {
        *out++ = '\'';
        total-=4;
        in+=4;
        len-=4;
      } else if ((in-start)+3 <= in_len && *in == '#' && *(in+1) == '4' && *(in+2) == '7' && *(in+3) == ';') {
        *out++ = '/';
        total-=4;
        in+=4;
        len-=4;
      } else if ((in-start)+4 <= in_len && *in == 'q' && *(in+1) == 'u' && *(in+2) == 'o' && *(in+3) == 't' && *(in+4) == ';') {
        *out++ = '\"';
        total-=5;
        in+=5;
        len-=5;
      }
    } else {
      *out++ = curChar;
    }
    len--;
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

static size_t escape_url(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar, hex[2];
  const unsigned char hexChars[16] = "0123456789ABCDEF";

  total = in_len;
  while (in_len) {
    curChar = *in++;
    if (curChar == ' ') {
      *out++ = '+';
    } else if ((curChar != '_' && curChar != '.' && curChar != '-') && NOT_HEX(curChar)) {
      hex[1] = hexChars[curChar & 0x0f];
      hex[0] = hexChars[(curChar >> 4) & 0x0f];
      *out++ = '%'; *out++ = hex[0]; *out++ = hex[1];
      total += 2;
    } else {
      *out++ = curChar;
    }
    in_len--;
  }

  return total;
}

static size_t unescape_url(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0, len = in_len;
  unsigned char curChar, *start;

  start = (unsigned char *)&in[0];
  total = in_len;
  while (len) {
    curChar = *in++;
    if (curChar == '%') {
      if ((in-start)+2 <= in_len && IS_HEX(*in) && IS_HEX(*(in+1))) {
        *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
        in+=2;
        total-=2;
      }
    } else if (curChar == '+') {
      *out++ = ' ';
    } else {
      *out++ = curChar;
    }
    len--;
  }

  return total;
}

static size_t escape_uri(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar, hex[2];
  const unsigned char hexChars[16] = "0123456789ABCDEF";

  total = in_len;
  while (in_len) {
    curChar = *in++;
    if (URI_SAFE(curChar)) {
        *out++ = curChar;
    } else {
        hex[1] = hexChars[curChar & 0x0f];
        hex[0] = hexChars[(curChar >> 4) & 0x0f];
        *out++ = '%'; *out++ = hex[0]; *out++ = hex[1];
        total += 2;
    }
    in_len--;
  }

  return total;
}

static size_t unescape_uri(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0, len = in_len;
  unsigned char curChar, *start;

  start = (unsigned char *)&in[0];
  total = in_len;
  while (len) {
    curChar = *in++;
    if (curChar == '%') {
      if ((in-start)+2 <= in_len && IS_HEX(*in) && IS_HEX(*(in+1))) {
        *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
        in+=2;
        total-=2;
      }
    } else {
      *out++ = curChar;
    }
    len--;
  }

  return total;
}

static VALUE rb_escape_html(int argc, VALUE * argv, VALUE self) {
  VALUE str, rb_secure = rb_funcall(mEscapeUtils, rb_html_secure, 0);
  unsigned short secure = 1;
  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc;
  rb_encoding *original_encoding;
#endif
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;
  if (rb_secure == Qfalse) {
    secure = 0;
  }

  if (rb_scan_args(argc, argv, "11", &str, &rb_secure) == 2) {
    if (rb_secure == Qfalse) {
      secure = 0;
    }
  }

  Check_Type(str, T_STRING);

#ifdef HAVE_RUBY_ENCODING_H
  default_internal_enc = rb_default_internal_encoding();
  original_encoding = rb_enc_get(str);
#endif
  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*5));

  // perform our escape, returning the new string's length
  new_len = escape_html(outBuf, inBuf, len, secure);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc;
  rb_encoding *original_encoding;
#endif
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  Check_Type(str, T_STRING);
#ifdef HAVE_RUBY_ENCODING_H
  default_internal_enc = rb_default_internal_encoding();
  original_encoding = rb_enc_get(str);
#endif
  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*len);

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
  }
#endif
  return rb_output_buf;
}

static VALUE rb_escape_javascript(VALUE self, VALUE str) {
  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc;
  rb_encoding *original_encoding;
#endif
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

#ifdef HAVE_RUBY_ENCODING_H
  default_internal_enc = rb_default_internal_encoding();
  original_encoding = rb_enc_get(str);
#endif
  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*2));

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
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_javascript(VALUE self, VALUE str) {
  VALUE rb_output_buf;
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *default_internal_enc;
  rb_encoding *original_encoding;
#endif
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

#ifdef HAVE_RUBY_ENCODING_H
  default_internal_enc = rb_default_internal_encoding();
  original_encoding = rb_enc_get(str);
#endif
  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*len);

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
  }
#endif
  return rb_output_buf;
}

static VALUE rb_escape_url(VALUE self, VALUE str) {
  VALUE rb_output_buf;
  #ifdef HAVE_RUBY_ENCODING_H
    rb_encoding *default_internal_enc;
    rb_encoding *original_encoding;
  #endif
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  Check_Type(str, T_STRING);

#ifdef HAVE_RUBY_ENCODING_H
  default_internal_enc = rb_default_internal_encoding();
  original_encoding = rb_enc_get(str);
#endif
  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str), new_len = 0;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*3));

  // perform our escape, returning the new string's length
  new_len = escape_url(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_url(VALUE self, VALUE str) {
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
  new_len = unescape_url(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_escape_uri(VALUE self, VALUE str) {
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
  unsigned char *outBuf = (unsigned char *)malloc(sizeof(unsigned char *)*(len*3));

  // perform our escape, returning the new string's length
  new_len = escape_uri(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  }
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_uri(VALUE self, VALUE str) {
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
  new_len = unescape_uri(outBuf, inBuf, len);

  // create our new ruby string
  rb_output_buf = rb_str_new((char *)outBuf, new_len);

  // free the temporary C string
  free(outBuf);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_associate(rb_output_buf, original_encoding);
  if (default_internal_enc) {
    rb_output_buf = rb_str_export_to_enc(rb_output_buf, default_internal_enc);
  }
#endif
  return rb_output_buf;
}

/* Ruby Extension initializer */
void Init_escape_utils_ext() {
  mEscapeUtils = rb_define_module("EscapeUtils");
  rb_define_method(mEscapeUtils,           "escape_html",          rb_escape_html,   -1);
  rb_define_module_function(mEscapeUtils,  "escape_html",          rb_escape_html,   -1);
  rb_define_method(mEscapeUtils,           "unescape_html",        rb_unescape_html, 1);
  rb_define_module_function(mEscapeUtils,  "unescape_html",        rb_unescape_html, 1);
  rb_define_method(mEscapeUtils,           "escape_javascript",    rb_escape_javascript, 1);
  rb_define_module_function(mEscapeUtils,  "escape_javascript",    rb_escape_javascript, 1);
  rb_define_method(mEscapeUtils,           "unescape_javascript",  rb_unescape_javascript, 1);
  rb_define_module_function(mEscapeUtils,  "unescape_javascript",  rb_unescape_javascript, 1);
  rb_define_method(mEscapeUtils,           "escape_url",           rb_escape_url, 1);
  rb_define_module_function(mEscapeUtils,  "escape_url",           rb_escape_url, 1);
  rb_define_method(mEscapeUtils,           "unescape_url",         rb_unescape_url, 1);
  rb_define_module_function(mEscapeUtils,  "unescape_url",         rb_unescape_url, 1);
  rb_define_method(mEscapeUtils,           "escape_uri",           rb_escape_uri, 1);
  rb_define_module_function(mEscapeUtils,  "escape_uri",           rb_escape_uri, 1);
  rb_define_method(mEscapeUtils,           "unescape_uri",         rb_unescape_uri, 1);
  rb_define_module_function(mEscapeUtils,  "unescape_uri",         rb_unescape_uri, 1);

  rb_html_secure = rb_intern("html_secure");
}

