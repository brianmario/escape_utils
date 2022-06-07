module Rack
  module Utils
    def escape_html(html)
      ::EscapeUtils::HtmlSafety.escape_once(html) { |s| CGI.escapeHTML(s) }
    end
    module_function :escape_html
  end
end
