# encoding: utf-8

require 'rubygems'
require 'bundler/setup'
require 'benchmark/ips'

require 'escape_utils'
require 'active_support/core_ext/string/output_safety'

url = "https://en.wikipedia.org/wiki/Succession_to_the_British_throne"
html = `curl -s #{url}`
html = html.force_encoding('utf-8') if html.respond_to?(:force_encoding)
puts "Escaping #{html.bytesize} bytes of html from #{url}"

Benchmark.ips do |x|
  x.report "ActionView escape_once" do # Rails expose it as ERB::Util.html_escape_once
    ERB::Util.html_escape_once(html)
  end

  x.report "EscapeUtils.escape_html_once" do
    EscapeUtils.escape_html_once(html)
  end

  x.compare!(order: :baseline)
end
