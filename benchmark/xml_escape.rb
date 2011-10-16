# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'builder'
require 'escape_utils'

times = 100
url = "http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml"
xml = `curl -s #{url}`
xml = xml.force_encoding('binary') if xml.respond_to?(:force_encoding)
puts "Escaping #{xml.bytesize} bytes of xml #{times} times, from #{url}"

Benchmark.bmbm do |x|
  x.report "Builder::String.to_xs" do
    times.times do
      xml.to_xs
    end
  end

  x.report "EscapeUtils.escape_xml" do
    times.times do
      EscapeUtils.escape_xml(xml)
    end
  end
end
