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
url = "http://en.wikipedia.org/wiki/Line_of_succession_to_the_British_throne"
html = `curl -s #{url}`
escaped_html = EscapeUtils.escape_html(html)
puts "Unescaping #{escaped_html.bytesize} bytes of escaped html #{times} times, from #{url}"

Benchmark.bmbm do |x|
  x.report "CGI.unescapeHTML" do
    times.times do
      CGI.unescapeHTML(escaped_html)
    end
  end

  x.report "EscapeUtils.unescape_html" do
    times.times do
      EscapeUtils.unescape_html(escaped_html)
    end
  end
end