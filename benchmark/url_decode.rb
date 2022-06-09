# encoding: utf-8

require 'rubygems'
require 'bundler/setup'
require 'benchmark/ips'

require 'rack'
require 'cgi'
require 'url_escape'
require 'fast_xs_extra'
require 'escape_utils'

url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mYHcEA  dh435dqUs0moGHeeAJTSLLbdbcbd9ef----,574b95600e9ab7d27eb0bf524ac68c27----"
url = url.force_encoding('us-ascii')
escaped_url = EscapeUtils.escape_uri(url)
puts "Escaping a #{url.bytesize} byte URL"

Benchmark.ips do |x|
  x.report "EscapeUtils.unescape_uri" do
    EscapeUtils.unescape_url(escaped_url)
  end

  x.report "fast_xs_extra#fast_uxs_url" do
    url.fast_uxs_cgi
  end

  x.compare!(order: :baseline)
end
