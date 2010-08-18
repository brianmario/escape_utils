# encoding: utf-8

module URI
  def self.escape(s)
    EscapeUtils.escape_uri(s)
  end
  def self.unescape(s)
    EscapeUtils.unescape_uri(s)
  end
end