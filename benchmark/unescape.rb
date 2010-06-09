# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'cgi'
require 'haml'
require 'escape_utils'

module HamlBench
  extend Haml::Helpers
end

times = 100
url = "http://maps.google.com"
html = `curl -s #{url}`
escaped_html = EscapeUtils.escape_html(html)
puts "Unescaping #{escaped_html.bytesize} bytes of escaped html from #{url}"

Benchmark.bmbm do |x|
  x.report do
    puts "CGI.unescapeHTML"
    times.times do
      CGI.unescapeHTML(escaped_html)
    end
  end

  x.report do
    puts "EscapeUtils.unescape_html"
    times.times do
      EscapeUtils.unescape_html(escaped_html)
    end
  end
end