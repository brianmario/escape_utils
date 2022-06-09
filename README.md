# escape_utils

`EscapeUtils` used to provide optimized escaping function to replace the slow methods
provided by Ruby. Since Ruby 2.5, the various `CGI` escape methods have been severely optimized
and most `EscapeUtils` methods became irrelevant and were deprecated.

It however still provide fast escaping and unescaping methods for URL (RFC 3986), Javascript, XML, as well as an "escape HTML once" method.

It has monkey-patches for Rack::Utils, URI and ERB::Util so you can drop this in and have your app start escaping fast as balls in no time

## Installing

Compatible with Ruby 2.5+

``` sh
gem install escape_utils
```

## Warning: UTF-8 only

escape_utils assumes all input is encoded as valid UTF-8. If you are dealing with other encodings do your best to transcode the string into a UTF-8 byte stream before handing it to escape_utils.


``` ruby
utf8_string = non_utf8_string.encode(Encoding::UTF_8)
```

## Usage

### HTML

As of `escape_utils 1.3.0`, regular HTML escaping methods are deprecated. Ruby 2.5 introduced C implementations for `CGI.escapeHTML` and `CGI.unescapeHTML` which are respectively faster and almost as fast as `EscapeUtils`. Use that instead.

To avoid double-escaping HTML entities, use `EscapeUtils.escape_html_once`.

#### Monkey Patches

Since historically, `HTML` monkey patches changed the return value for `ActiveSupport::SafeBuffer` instances, they are conserved for that purpose only, but they should be considered as deprecated as well.

``` ruby
require 'escape_utils/html/cgi' # to patch CGI
```

### URL

Use `escape_uri` and `unescape` to get RFC 3986 compliant escaping (like PHP `rawurlencode` or `ERB::Util.url_encode`).

The difference with `CGI.escape` is that spaces (` `) are encoded as `%20` instead of `+`.

#### Escaping

``` ruby
url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mcEA~!!#*YH*>@!U"
escaped_url = EscapeUtils.escape_uri(url)
```

#### Unescaping

``` ruby
url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mcEA~!!#*YH*>@!U"
escaped_url = EscapeUtils.escape_uri(url)
EscapeUtils.unescape_uri(escaped_uri) == url # => true
```

#### Monkey Patches

``` ruby
require 'escape_utils/url/erb' # to patch ERB::Util
require 'escape_utils/url/uri' # to patch URI
```

Note that `URI.escape` and `URI.unescape` were removed in Ruby 3.0. `'escape_utils/url/uri'` is a noop on Ruby 3+.

### XML

```ruby
xml = `curl -s 'https://raw.githubusercontent.com/darcyliu/google-styleguide/master/cppguide.xml'`
escaped_xml = EscapeUtils.escape_xml(xml)
```

### Javascript

#### Escaping

``` ruby
javascript = `curl -s http://code.jquery.com/jquery-1.4.2.js`
escaped_javascript = EscapeUtils.escape_javascript(javascript)
```

#### Unescaping

``` ruby
javascript = `curl -s http://code.jquery.com/jquery-1.4.2.js`
escaped_javascript = EscapeUtils.escape_javascript(javascript)
EscapeUtils.unescape_javascript(escaped_javascript) == javascript # => true
```

#### Monkey Patches

``` ruby
require 'escape_utils/javascript/action_view' # to patch ActionView::Helpers::JavaScriptHelper
```

## Benchmarks

Escaping URL following RFC 3986 is 13-32x faster than the methods provided by Ruby.

Escaping Javascript is around 13x faster than Rails `escape_javascript`.

`EscapeUtils.escape_html_once` is about 17x faster than Rails `escape_once`.

This output is from my laptop using the benchmark scripts in the benchmarks folder.

### Javascript

#### Escaping

```
EscapeUtils.escape_javascript:                               1567.5 i/s
ActionView::Helpers::JavaScriptHelper#escape_javascript:      116.8 i/s - 13.42x  (± 0.00) slower
```

#### Unescaping

```
EscapeUtils.escape_javascript:    2.089k (± 3.0%) i/s -     10.530k in   5.044615s
```

I didn't look that hard, but I'm not aware of another ruby library that does Javascript unescaping to benchmark against. Anyone know of any?

### URL

#### Escaping

```
EscapeUtils.escape_uri:       4019359.2 i/s
fast_xs_extra#fast_xs_url:    2435949.2 i/s - 1.65x  (± 0.00) slower
URI::DEFAULT_PARSER.escape:   288800.8 i/s - 13.92x  (± 0.00) slower
ERB::Util.url_encode:         122373.5 i/s - 32.85x  (± 0.00) slower
```

#### Unescaping

```
EscapeUtils.unescape_uri:    3866774.5 i/s
fast_xs_extra#fast_uxs_url:  2438900.7 i/s - 1.59x  (± 0.00) slower
```

### HTML

#### Escape once

```
EscapeUtils.escape_html_once:                   2831.5 i/s
ActionView::Helpers::TagHelper#escape_once:      161.4 i/s - 17.55x  (± 0.00) slower
```
