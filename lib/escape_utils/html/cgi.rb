# encoding: utf-8

class CGI
  extend ::EscapeUtils::HtmlSafety

  class << self
    alias escapeHTML _escape_html
  end
end