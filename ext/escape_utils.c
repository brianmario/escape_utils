#include <ruby.h>

#define APPEND_BUFFER(escape, len, scoot_by)  \
  memcpy(&out[total], &in[offset], i-offset); \
  total += i-offset;                          \
  offset = i+scoot_by;                        \
  memcpy(&out[total], escape, len);           \
  total += len;                               \
  break;                                      \

static size_t escape_html(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t i = 0, offset = 0, total = 0;

  for(;i<in_len;i++) {
    switch(in[i]) {
      case '&':  APPEND_BUFFER("&amp;",  5, 1);
      case '<':  APPEND_BUFFER("&lt;",   4, 1);
      case '>':  APPEND_BUFFER("&gt;",   4, 1);
      case '\'': APPEND_BUFFER("&#39;",  5, 1);
      case '\"': APPEND_BUFFER("&quot;", 6, 1);
    }
  }

  // append the rest of the buffer
  memcpy(&out[total], &in[offset], i-offset);

  return total + (i-offset);
}

static size_t unescape_html(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t i = 0, offset = 0, total = 0;
  
  for(;i<in_len;i++) {
    switch(in[i]) {
      case '&':
        if (i+5 <= in_len) {
          if (memcmp(&in[i], "&amp;", 5) == 0) {
            APPEND_BUFFER("&", 1, 5);
          } else if (memcmp(&in[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&in[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          } else if (memcmp(&in[i], "&#39;", 5) == 0) {
            APPEND_BUFFER("\'", 1, 5);
          } else if (memcmp(&in[i], "&quot;", 6) == 0) {
            APPEND_BUFFER("\"", 1, 6);
          }
        } else if (i+4 <= in_len) {
          if (memcmp(&in[i], "&amp;", 5) == 0) {
            APPEND_BUFFER("&", 1, 5);
          } else if (memcmp(&in[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&in[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          } else if (memcmp(&in[i], "&#39;", 5) == 0) {
            APPEND_BUFFER("\'", 1, 5);
          }
        } else if (i+3 <= in_len) {
          if (memcmp(&in[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&in[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          }
        }
      break;
    }
  }

  // append the rest of the buffer
  memcpy(&out[total], &in[offset], i-offset);

  return total + (i-offset);
}

static size_t escape_javascript(unsigned char *out, const unsigned char *in, size_t in_len) {
  size_t i = 0, offset = 0, total = 0;

  for(;i<in_len;i++) {
    switch(in[i]) {
      case '\\':  APPEND_BUFFER("\\\\", 2, 1);
      case '<':
        if (i+1 <= in_len && in[i+1] == '/') {
          APPEND_BUFFER("<\\/", 3, 2);
        }
        break;
      case '\r':
        if (i+1 <= in_len && in[i+1] == '\n') {
          APPEND_BUFFER("\\n", 2, 1);
        } else {
          APPEND_BUFFER("\\n", 2, 1);
        }
        break;
      case '\n': APPEND_BUFFER("\\n", 2, 1);
      case '\"': APPEND_BUFFER("\\\"", 2, 1);
      case '\'': APPEND_BUFFER("\\'", 2, 1);
    }
  }

  // append the rest of the buffer
  memcpy(&out[total], &in[offset], i-offset);

  return total + (i-offset);
}

static VALUE rb_escape_html(VALUE self, VALUE str) {
  VALUE rb_output_buf;
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

  return rb_output_buf;
}

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  VALUE rb_output_buf;
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

  return rb_output_buf;
}

static VALUE rb_escape_javascript(VALUE self, VALUE str) {
  if (str == Qnil) {
    return rb_str_new2("");
  }

  Check_Type(str, T_STRING);

  VALUE rb_output_buf;
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
}