# encoding: utf-8

require 'escape_utils_ext'

EscapeUtils.send(:extend, EscapeUtils)
module EscapeUtils
  VERSION = "0.1.6"

  autoload :HtmlSafety, 'escape_utils/html_safety'
end