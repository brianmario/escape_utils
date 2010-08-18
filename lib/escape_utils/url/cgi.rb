# encoding: utf-8

class CGI
  def self.escape(s)
    EscapeUtils.variable_position(s.to_s)
  end
  def self.unescape(s)
    EscapeUtils.unvariable_position(s.to_s)
  end
end