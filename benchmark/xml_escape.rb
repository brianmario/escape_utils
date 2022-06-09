# encoding: utf-8

require 'rubygems'
require 'bundler/setup'
require 'benchmark/ips'

require 'fast_xs'
require 'escape_utils'

url = "https://raw.githubusercontent.com/darcyliu/google-styleguide/master/cppguide.xml"
xml = `curl -s #{url}`
xml = xml.force_encoding('binary')
puts "Escaping #{xml.bytesize} bytes of xml, from #{url}"

Benchmark.ips do |x|
  x.report "EscapeUtils.escape_xml" do
    EscapeUtils.escape_xml(xml)
  end

  x.report "fast_xs" do
    xml.fast_xs
  end

  x.compare!(order: :baseline)
end
