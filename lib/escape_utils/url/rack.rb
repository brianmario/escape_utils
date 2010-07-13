# encoding: utf-8

module Rack
  module Utils
    def escape(url)
      EscapeUtils.escape_url(url)
    end
    def unescape(url)
      EscapeUtils.unescape_url(url)
    end
    module_function :escape
    module_function :unescape
  end
end
