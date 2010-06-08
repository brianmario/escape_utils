# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'rack'
require 'erb'
require 'cgi'
require 'haml'
require 'escape_utils'

module HamlBench
  extend Haml::Helpers
end

times = 100
url = "http://maps.google.com"
html = `curl -s #{url}`
puts "Escaping #{html.bytesize} bytes of html from #{url}"

Benchmark.bmbm do |x|
  x.report do
    puts "Rack::Utils.escape_html"
    times.times do
      Rack::Utils.escape_html(html)
    end
  end

  x.report do
    puts "ERB::Util.html_escape"
    times.times do
      ERB::Util.html_escape(html)
    end
  end

  x.report do
    puts "CGI.escapeHTML"
    times.times do
      CGI.escapeHTML(html)
    end
  end

  x.report do
    puts "Haml::Helpers.html_escape"
    times.times do
      HamlBench.html_escape(html)
    end
  end

  x.report do
    puts "EscapeUtils.escape_html"
    times.times do
      EscapeUtils.escape_html(html)
    end
  end
end