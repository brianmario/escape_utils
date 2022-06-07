# Unreleased

- Add `EscapeUtils.escape_html_once` and `EscapeUtils.rb_eu_escape_html_once_as_html_safe` as faster implementations of Rails `escape_once` helper.

# 1.2.2

- Update EscapeUtils.escape_javascript to match Rails `escape_javascript`
  Now escapes, Backquotes (```), Dollar (`$`), `U+2000` and `U+2001`
- Make the Rack monkey patch a noop as it's no longer correct since circa 2011.
- Require Ruby 2.5+
- Stop escaping `~` like `CGI.escape` does since Ruby 2.5

# 1.2.1

- Historical version