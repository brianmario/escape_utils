# encoding: utf-8

module Haml
  module Helpers
    def html_escape(s)
      EscapeUtils.escape_html(s)
    end
  end
end