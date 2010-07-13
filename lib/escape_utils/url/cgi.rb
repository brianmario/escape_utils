# encoding: utf-8

class CGI
  def self.escape(s)
    EscapeUtils.escape_url(s)
  end
  def self.unescape(s)
    EscapeUtils.unescape_url(s)
  end
end