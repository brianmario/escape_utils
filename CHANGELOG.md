# Changelog

## 0.3.2 (February 28th, 2013)
* fix rbx compatibility
* add EscapeUtils.html_safe_string_class
* add EscapeUtils.escape_html_as_html_safe

## 0.3.1 (February 26th, 2013)
* fixed compilation on Windows

## 0.3.0 (February 26th, 2013)
* add xml escaping support
* in Ruby 1.9 - escape_utils now requires the input string be UTF-8
* update upstream houdini to pull in some speed optimizations
* a couple of other perf tweaks
* switched to minitest

## 0.2.4 (September 7th, 2011)
* swap out custom escaping routines for houdini - https://github.com/tanoku/houdini
* add RSTRING_NOT_MODIFIED define for a Rubinius speedup

## 0.2.3 (March 9th, 2011)
* change encoding strategy to simply return strings in the encoding the input string was in, not taking into account Encoding.default_internal

## 0.2.2 (February 25th, 2011)
* minor fix for Rubinius compatibility

## 0.2.1 (February 21st, 2011)
* fix buffer over read in unescape_url and unescape_uri

## 0.2.0 (February 8th, 2011)
* fixed a couple of compilation warnings on 1.9.3
* moved to rspec2
* remove hard-conversion to utf-8 to preserve the string's original encoding
* moved to rake-compiler, Bundler
* pass through incompletely escaped data on unescaping
* added tilde to escape_{uri,url}specs (It's a difference between CGI.escape and URI.escape)
* escape_uri and escape_url now match their Ruby counterparts
** escape_uri is used where URI.escape is, and escape_url is used where CGI.escape is used.
* performance and memory usage optimizations

## 0.1.9 (October 15th, 2010)
* add a flag as an optional 2nd parameter to EscapeUtils.escape_html to disable/enable the escaping of the '/' character. Defaults to the new flag EscapeUtils.html_secure

## 0.1.8 (September 29th, 2010)
* fix URI escaping one last time ;)

## 0.1.7 (September 29th, 2010)
* fix URI escaping to act according to the RFC
* add specs for URL escaping

## 0.1.6 (September 6th, 2010)
* support for URI escaping added (thanks to @joshbuddy)
* bugfix to ensure we don't drop opening tags during escape_javascript (thanks to @nagybence)

## 0.1.5 (July 13th, 2010)
* add URL escaping and unescaping
* major refactor of HTML and Javascript escaping and unescaping logic for a decent speed up
* HTML escaping now takes html_safe? into account (for Rails/ActiveSupport users) - thanks yury!

## 0.1.4 (June 9th, 2010)
* ensure strings are passed in from monkey-patches

## 0.1.3 (June 9th, 2010)
* cleaned some code up, removing duplication
* moved to a more flexible character encoding scheme using Encoding.defaut_internal for 1.9 users

## 0.1.2 (June 8th, 2010)
* forgot to add the ActionView monkey patch for JS escaping ;)

## 0.1.1 (June 8th, 2010)
* added javascript escaping

## 0.1.0 (June 8th, 2010)
* initial release