# encoding: utf-8

class CGI
  def self.escape(s)
    EscapeUtils.escape_url(s)
  end
end