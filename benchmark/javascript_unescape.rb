# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'escape_utils'

times = 100
url = "http://ajax.googleapis.com/ajax/libs/dojo/1.4.3/dojo/dojo.xd.js.uncompressed.js"
javascript = `curl -s #{url}`
javascript = javascript.force_encoding('utf-8') if javascript.respond_to?(:force_encoding)
escaped_javascript = EscapeUtils.escape_javascript(javascript)
puts "Escaping #{escaped_javascript.bytesize} bytes of javascript #{times} times, from #{url}"

Benchmark.bmbm do |x|
  x.report "EscapeUtils.escape_javascript" do
    times.times do
      EscapeUtils.unescape_javascript(escaped_javascript)
    end
  end
end