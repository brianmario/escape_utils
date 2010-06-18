# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'rack'
require 'erb'
require 'cgi'
require 'haml'
require 'fast_xs_extra'
require 'faster_html_escape'
require 'escape_utils'

module HamlBench
  extend Haml::Helpers
end

times = 100
url = "http://en.wikipedia.org/wiki/Line_of_succession_to_the_British_throne"
html = `curl -s #{url}`
puts "Escaping #{html.bytesize} bytes of html #{times} times, from #{url}"

Benchmark.bmbm do |x|
  x.report do
    puts "Rack::Utils.escape_html"
    times.times do
      Rack::Utils.escape_html(html)
    end
  end

  x.report do
    puts "Haml::Helpers.html_escape"
    times.times do
      HamlBench.html_escape(html)
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
    puts "FasterHTMLEscape.html_escape"
    times.times do
      FasterHTMLEscape.html_escape(html)
    end
  end

  x.report do
    puts "fast_xs_extra#fast_xs_html"
    times.times do
      html.fast_xs_html
    end
  end

  x.report do
    puts "EscapeUtils.escape_html"
    times.times do
      EscapeUtils.escape_html(html)
    end
  end
end