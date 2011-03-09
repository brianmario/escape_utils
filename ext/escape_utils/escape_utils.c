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

#define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
#define UNHEX(c) (c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : c - 'a' + 10)

#define ALPHANUM(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
#define URL_SAFE(c) (ALPHANUM(c) || c == '-' || c == '_' || c == '.')

/* from uri/common.rb */
#define UNRESERVED(c) (ALPHANUM(c) || c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*' || c == '\'' || c == '(' || c == ')')
#define RESERVED(c) (c == ';' || c == '/' || c == '?' || c == ':' || c == '@' || c== '&' || c == '=' || c == '+' || c == '$' || c == ',' || c == '[' || c == ']')
#define URI_SAFE(c) (URL_SAFE(c) || UNRESERVED(c) || RESERVED(c))

static size_t escape_html(unsigned char *out, const unsigned char *in, size_t in_len, int secure) {
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
      if (*in == 'l' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '<';
        total-=3;
        in+=3;
        len-=3;
      } else if (*in == 'g' && *(in+1) == 't' && *(in+2) == ';') {
        *out++ = '>';
        total-=3;
        in+=3;
        len-=3;
      } else if (*in == 'a' && *(in+1) == 'm' && *(in+2) == 'p' && *(in+3) == ';') {
        *out++ = '&';
        total-=4;
        in+=4;
        len-=4;
      } else if (*in == '#' && *(in+1) == '3' && *(in+2) == '9' && *(in+3) == ';') {
        *out++ = '\'';
        total-=4;
        in+=4;
        len-=4;
      } else if (*in == '#' && *(in+1) == '4' && *(in+2) == '7' && *(in+3) == ';') {
        *out++ = '/';
        total-=4;
        in+=4;
        len-=4;
      } else if (*in == 'q' && *(in+1) == 'u' && *(in+2) == 'o' && *(in+3) == 't' && *(in+4) == ';') {
        *out++ = '\"';
        total-=5;
        in+=5;
        len-=5;
      } else {
        /* incomplete tag, pass it through */
        *out++ = curChar;
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

static size_t escape_url(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0;
  unsigned char curChar, hex[2];
  const unsigned char hexChars[16] = "0123456789ABCDEF";

  total = in_len;
  while (in_len) {
    curChar = *in++;
    if (curChar == ' ') {
      *out++ = '+';
    } else if (URL_SAFE(curChar)) {
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

static size_t unescape_url(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t total = 0, len = in_len;
  unsigned char curChar, *start;

  start = (unsigned char *)&in[0];
  total = in_len;
  while (len) {
    curChar = *in++;
    if (curChar == '%') {
      if (IS_HEX(*in) && IS_HEX(*(in+1))) {
        *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
        in+=2;
        len-=2;
        total-=2;
      } else {
        /* incomplete escape, pass it through */
        *out++ = curChar;
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
      if (IS_HEX(*in) && IS_HEX(*(in+1))) {
        *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
        in+=2;
        len-=2;
        total-=2;
      } else {
        /* incomplete escape, pass it through */
        *out++ = curChar;
      }
    } else {
      *out++ = curChar;
    }
    len--;
  }

  return total;
}

static VALUE rb_escape_html(int argc, VALUE * argv, VALUE self) {
  VALUE str, rb_secure;
  int secure = html_secure;
  VALUE rb_output_buf;
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  if (rb_scan_args(argc, argv, "11", &str, &rb_secure) == 2) {
    if (rb_secure == Qfalse) {
      secure = 0;
    }
  }

  Check_Type(str, T_STRING);

  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str);

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  new_len = sizeof(unsigned char)*(len*5);

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = escape_html(outBuf, inBuf, len, secure);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  VALUE rb_output_buf;
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

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
  new_len = unescape_html(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
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
  VALUE rb_output_buf;
  unsigned char *inBuf, *outBuf;
  size_t len, new_len;

  Check_Type(str, T_STRING);

  inBuf = (unsigned char*)RSTRING_PTR(str);
  len = RSTRING_LEN(str);

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  new_len = sizeof(unsigned char)*(len*3);

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = escape_url(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_url(VALUE self, VALUE str) {
  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str);

  // this is the max size the string could be
  // TODO: we could be more intelligent about this, but probably not
  size_t new_len = sizeof(unsigned char) * len;
  unsigned char *outBuf;

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);

  // perform our escape, returning the new string's length
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);
  new_len = unescape_url(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_escape_uri(VALUE self, VALUE str) {
  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str);
  unsigned char *outBuf;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  size_t new_len = sizeof(unsigned char)*(len*3);

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = escape_uri(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
}

static VALUE rb_unescape_uri(VALUE self, VALUE str) {
  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
  unsigned char *inBuf = (unsigned char*)RSTRING_PTR(str);
  size_t len = RSTRING_LEN(str);
  unsigned char *outBuf;

  // this is the max size the string could be
  // TODO: we should try to be more intelligent about this
  size_t new_len = sizeof(unsigned char)*len;

  // create our new ruby string
  rb_output_buf = rb_str_new(NULL, new_len);
  outBuf = (unsigned char *)RSTRING_PTR(rb_output_buf);

  // perform our escape, returning the new string's length
  new_len = unescape_uri(outBuf, inBuf, len);

  // shrink our new ruby string
  rb_str_resize(rb_output_buf, new_len);

#ifdef HAVE_RUBY_ENCODING_H
  rb_enc_copy(rb_output_buf, str);
#endif
  return rb_output_buf;
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

  rb_define_singleton_method(mEscapeUtils, "html_secure",          rb_s_get_html_secure, 0);
  rb_define_singleton_method(mEscapeUtils, "html_secure=",         rb_s_set_html_secure, 1);

  rb_html_secure = rb_intern("@@html_secure");
}

