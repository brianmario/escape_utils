require 'cgi'
require 'escape_utils/escape_utils'
require 'escape_utils/version' unless defined? EscapeUtils::VERSION

module EscapeUtils
  extend self

  def html_secure
    warn "EscapeUtils.html_secure is deprecated"
    false
  end

  def html_secure=(val)
    warn "EscapeUtils.html_secure is deprecated"
  end

  # Default String class to return from HTML escaping
  attr_reader :html_safe_string_class

  def html_safe_string_class=(klass)
    unless String >= klass
      raise ArgumentError, "EscapeUtils.html_safe_string_class must inherit from ::String"
    end
    @html_safe_string_class = klass
  end

  self.html_safe_string_class = String

  autoload :HtmlSafety, 'escape_utils/html_safety'

  def self.escape_html_once_as_html_safe(html)
    escaped = escape_html_once(html)
    if String == @html_safe_string_class
      escaped
    else
      escaped = @html_safe_string_class.new(escaped)
      escaped.instance_variable_set(:@html_safe, true)
      escaped
    end
  end

  def self.escape_html(html, secure = false)
    warn "EscapeUtils.escape_html is deprecated. Use GCI.escapeHTML instead, it's faster"
    CGI.escapeHTML(html)
  end

  def self.escape_html_as_html_safe(html)
    warn "EscapeUtils.escape_html_as_html_safe is deprecated. Use GCI.escapeHTML(str).html_safe instead, it's faster"

    escaped = CGI.escapeHTML(html)
    if String == @html_safe_string_class
      escaped
    else
      escaped = @html_safe_string_class.new(escaped)
      escaped.instance_variable_set(:@html_safe, true)
      escaped
    end
  end

  def self.unescape_html(html)
    warn "EscapeUtils.unescape_html is deprecated. Use GCI.unescapeHTML instead, performance is similar"
    CGI.unescapeHTML(html)
  end

  def self.escape_url(string)
    warn "EscapeUtils.escape_url is deprecated. Use CGI.escape instead, performance is similar"
    CGI.escape(string)
  end

  def self.unescape_url(string)
    warn "EscapeUtils.unescape_url is deprecated. Use CGI.unescape instead, performance is similar"
    CGI.unescape(string)
  end
end
