# encoding: utf-8

module Rack
  module Utils
    def escape(url)
      EscapeUtils.escape_url(url)
    end
    module_function :escape
  end
end
