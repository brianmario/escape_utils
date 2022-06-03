# Unreleased

# 1.2.2

- Update EscapeUtils.escape_javascript to match Rails `escape_javascript`
  Now escapes, Backquotes (```), Dollar (`$`), `U+2000` and `U+2001`
- Make the Rack monkey patch a noop as it's no longer correct since circa 2011.
- Require Ruby 2.5+
- Stop escaping `~` like `CGI.escape` does since Ruby 2.5

# 1.2.1

- Historical version