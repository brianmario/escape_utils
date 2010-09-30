# encoding: utf-8

module URI
  def self.escape(s, unsafe=nil)
    EscapeUtils.escape_url(s.to_s)
  end
  def self.unescape(s)
    EscapeUtils.unescape_url(s.to_s)
  end
end