# encoding: utf-8

class URI
  def self.escape(s)
    EscapeUtils.escape_uri(s)
  end
  def self.unescape(s)
    EscapeUtils.unescape_uri(s)
  end
end