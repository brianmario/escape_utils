# encoding: utf-8

class ERB
  module Util
    def html_escape(s)
      EscapeUtils.escape_html(s.to_s)
    end

    alias h html_escape
    module_function :h
    module_function :html_escape
  end
end