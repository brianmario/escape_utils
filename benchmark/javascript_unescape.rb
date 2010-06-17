# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'escape_utils'

times = 100
url = "http://ajax.googleapis.com/ajax/libs/dojo/1.4.3/dojo/dojo.xd.js.uncompressed.js"
javascript = `curl -s #{url}`
escaped_javascript = EscapeUtils.escape_javascript(javascript)
puts "Escaping #{escaped_javascript.bytesize} bytes of javascript #{times} times, from #{url}"

Benchmark.bmbm do |x|
  x.report do
    puts "EscapeUtils.escape_javascript"
    times.times do
      EscapeUtils.unescape_javascript(escaped_javascript)
    end
  end
end