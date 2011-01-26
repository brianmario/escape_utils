# encoding: utf-8
require 'escape_utils/escape_utils'
require 'escape_utils/version'

EscapeUtils.send(:extend, EscapeUtils)
module EscapeUtils
  # turn on/off the escaping of the '/' character during HTML escaping
  # Escaping '/' is recommended by the OWASP - http://www.owasp.org/index.php/XSS_(Cross_Site_Scripting)_Prevention_Cheat_Sheet#RULE_.231_-_HTML_Escape_Before_Inserting_Untrusted_Data_into_HTML_Element_Content
  # This is because quotes around HTML attributes are optional in most/all modern browsers at the time of writing (10/15/2010)
  @@html_secure = true

  def self.html_secure
    @@html_secure
  end
  def self.html_secure=(val)
    @@html_secure = val
  end

  autoload :HtmlSafety, 'escape_utils/html_safety'
end