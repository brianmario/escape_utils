# Changelog

## 0.1.5 (September 6th, 2010)
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