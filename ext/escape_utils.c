#include <ruby.h>

#define APPEND_BUFFER(escape, len, scoot)           \
  memcpy(&dst_buf[total], &buf[offset], i-offset);  \
  total += i-offset;                                \
  offset = i+scoot;                                 \
  memcpy(&dst_buf[total], escape, len);             \
  total += len;                                     \
  break;                                            \

static VALUE rb_escape_html(VALUE self, VALUE str) {
  char *buf = (char*)RSTRING_PTR(str);
  size_t i = 0, len = RSTRING_LEN(str), offset = 0, total = 0;
  // max size the string could be; we should try to be more intelligent about this
  char *dst_buf = (char *)malloc(sizeof(char *)*(len*5));
  VALUE dst_str;

  for(;i<len;i++) {
    switch(buf[i]) {
      case '&':  APPEND_BUFFER("&amp;",  5, 1);
      case '<':  APPEND_BUFFER("&lt;",   4, 1);
      case '>':  APPEND_BUFFER("&gt;",   4, 1);
      case '\'': APPEND_BUFFER("&#39;",  5, 1);
      case '\"': APPEND_BUFFER("&quot;", 6, 1);
    }
  }

  // append the rest of the buffer
  memcpy(&dst_buf[total], &buf[offset], i-offset);
  total += i-offset;
  dst_str = rb_str_new(dst_buf, total);
  free(dst_buf);
  return dst_str;
}

static VALUE rb_unescape_html(VALUE self, VALUE str) {
  char *buf = (char*)RSTRING_PTR(str);
  size_t i = 0, len = RSTRING_LEN(str), offset = 0, total = 0;
  char *dst_buf = (char *)malloc(sizeof(char *)*len);
  VALUE dst_str;

  for(;i<len;i++) {
    switch(buf[i]) {
      case '&':
        if (i+5 <= len) {
          if (memcmp(&buf[i], "&amp;", 5) == 0) {
            APPEND_BUFFER("&", 1, 5);
          } else if (memcmp(&buf[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&buf[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          } else if (memcmp(&buf[i], "&#39;", 5) == 0) {
            APPEND_BUFFER("\'", 1, 5);
          } else if (memcmp(&buf[i], "&quot;", 6) == 0) {
            APPEND_BUFFER("\"", 1, 6);
          }
        } else if (i+4 <= len) {
          if (memcmp(&buf[i], "&amp;", 5) == 0) {
            APPEND_BUFFER("&", 1, 5);
          } else if (memcmp(&buf[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&buf[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          } else if (memcmp(&buf[i], "&#39;", 5) == 0) {
            APPEND_BUFFER("\'", 1, 5);
          }
        } else if (i+3 <= len) {
          if (memcmp(&buf[i], "&lt;", 4) == 0) {
            APPEND_BUFFER("<", 1, 4);
          } else if (memcmp(&buf[i], "&gt;", 4) == 0) {
            APPEND_BUFFER(">", 1, 4);
          }
        }
      break;
    }
  }

  // append the rest of the buffer
  memcpy(&dst_buf[total], &buf[offset], i-offset);
  total += i-offset;
  dst_str = rb_str_new(dst_buf, total);
  free(dst_buf);
  return dst_str;
}

/* Ruby Extension initializer */
void Init_escape_utils_ext() {
  VALUE mEscape = rb_define_module("EscapeUtils");
  rb_define_method(mEscape,           "escape_html",    rb_escape_html,   1);
  rb_define_module_function(mEscape,  "escape_html",    rb_escape_html,   1);
  rb_define_method(mEscape,           "unescape_html",  rb_unescape_html, 1);
  rb_define_module_function(mEscape,  "unescape_html",  rb_unescape_html, 1);
}