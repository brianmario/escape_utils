#include <ruby.h>

#define APPEND_BUFFER(escape, len)                  \
  memcpy(&dst_buf[total], &buf[offset], i-offset);  \
  total += i-offset;                                \
  offset = i+1;                                     \
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
      case '&':  APPEND_BUFFER("&amp;",  5);
      case '<':  APPEND_BUFFER("&lt;",   4);
      case '>':  APPEND_BUFFER("&gt;",   4);
      case '\'': APPEND_BUFFER("&#39;",  5);
      case '\"': APPEND_BUFFER("&quot;", 6);
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
  rb_define_method(mEscape,           "escape_html", rb_escape_html, 1);
  rb_define_module_function(mEscape,  "escape_html", rb_escape_html, 1);
}