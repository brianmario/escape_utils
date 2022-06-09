require 'uri'

if URI.respond_to?(:escape) # Was removed in Ruby 3.0. Let's not bring it back
  module URI
    def self.escape(s, unsafe=nil)
      EscapeUtils.escape_uri(s.to_s)
    end
    def self.unescape(s)
      EscapeUtils.unescape_uri(s.to_s)
    end
  end
end
