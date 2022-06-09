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
url = url.force_encoding('us-ascii')
puts "Escaping a #{url.bytesize} byte URL times"

Benchmark.ips do |x|
  x.report "EscapeUtils.escape_url" do
    EscapeUtils.escape_url(url)
  end

  x.report "Rack::Utils.escape" do
    Rack::Utils.escape(url)
  end

  x.report "CGI.escape" do
    CGI.escape(url)
  end

  x.report "URLEscape#escape" do
    URLEscape.escape(url)
  end

  x.report "fast_xs_extra#fast_xs_cgi" do
    url.fast_xs_cgi
  end

  x.compare!(order: :baseline)
end
