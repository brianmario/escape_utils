# encoding: utf-8

require 'rubygems'
require 'bundler/setup'
require 'benchmark/ips'

require 'fast_xs'
require 'escape_utils'

url = "http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml"
xml = `curl -s #{url}`
xml = xml.force_encoding('binary')
puts "Escaping #{xml.bytesize} bytes of xml, from #{url}"

Benchmark.ips do |x|
  x.report "fast_xs" do
    xml.fast_xs
  end

  x.report "EscapeUtils.escape_xml" do
    EscapeUtils.escape_xml(xml)
  end

  x.compare!(order: :baseline)
end
