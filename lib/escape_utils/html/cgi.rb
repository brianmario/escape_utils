# encoding: utf-8

class CGI
  def self.escapeHTML(s)
    EscapeUtils.escape_html(s)
  end
end