# encoding: utf-8

require 'rubygems'
require 'bundler/setup'
require 'benchmark/ips'

require 'rack'
require 'erb'
require 'cgi'
require 'url_escape'
require 'fast_xs_extra'
require 'escape_utils'

url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mYHcEA  dh435dqUs0moGHeeAJTSLLbdbcbd9ef----,574b95600e9ab7d27eb0bf524ac68c27----"
puts "Escaping a #{url.bytesize} byte URL times"

Benchmark.ips do |x|
  x.report "EscapeUtils.escape_uri" do
    EscapeUtils.escape_uri(url)
  end

  x.report "ERB::Util.url_encode" do |times|
    times.times do
      ERB::Util.url_encode(url)
    end
  end

  x.report "fast_xs_extra#fast_xs_url" do
    url.fast_xs_url
  end

  x.compare!(order: :baseline)
end
