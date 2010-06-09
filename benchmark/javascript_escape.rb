# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'action_view'
require 'escape_utils'

class ActionPackBench
  extend ActionView::Helpers::JavaScriptHelper
end

times = 100
url = "http://code.jquery.com/jquery-1.4.2.js"
javascript = `curl -s #{url}`
puts "Escaping #{javascript.bytesize} bytes of javascript from #{url}"

puts ActionPackBench.escape_javascript(javascript).eql?(EscapeUtils.escape_javascript(javascript))

Benchmark.bmbm do |x|
  x.report do
    puts "ActionView::Helpers::JavaScriptHelper#escape_javascript"
    times.times do
      ActionPackBench.escape_javascript(javascript)
    end
  end

  x.report do
    puts "EscapeUtils.escape_javascript"
    times.times do
      EscapeUtils.escape_javascript(javascript)
    end
  end
end