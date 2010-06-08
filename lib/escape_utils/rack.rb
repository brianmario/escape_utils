# encoding: utf-8

module Rack
  module Utils
    def escape_html(s)
      EscapeUtils.escape_html(s)
    end
    def self.escape_html(s)
      EscapeUtils.escape_html(s)
    end
  end
end
