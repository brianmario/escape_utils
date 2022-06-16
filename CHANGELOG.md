# Unreleased

# 1.3.0

- Deprecate `EscapeUtils.escape_url` and `EscapeUtils.unescape_url` given that Ruby 2.5 provides an optimized `CGI.escape` and `CGI.unescape` with mostly similar performance.
- Don't patch `URI.escape` and `URI.unescape` if they don't already exist.
- Add `EscapeUtils.escape_html_once` and `EscapeUtils.rb_eu_escape_html_once_as_html_safe` as faster implementations of Rails `escape_once` helper.
- Deprecate `escape_html` and `escape_html_as_html_safe` given that Ruby 2.5 optimized `GCI.escapeHTML` to be twice faster than the `EscapeUtils` implementation.
- Deprecate `unescape_html` given that Ruby 2.5 optimized `GCI.unescapeHTML` to be only 40% slower than th `EscapeUtils` implementation.
- Deprecate `escape_html_as_html_safe` as well.
- Deprecate `EscapeUtils.html_safe`, there's no reason to escape for slashes `/` in 2022.

# 1.2.2

- Update EscapeUtils.escape_javascript to match Rails `escape_javascript`
  Now escapes, Backquotes (```), Dollar (`$`), `U+2000` and `U+2001`
- Make the Rack monkey patch a noop as it's no longer correct since circa 2011.
- Require Ruby 2.5+
- Stop escaping `~` like `CGI.escape` does since Ruby 2.5

# 1.2.1

- Historical version