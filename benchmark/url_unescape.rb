# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'rack'
require 'cgi'
require 'url_escape'
require 'fast_xs_extra'
require 'escape_utils'

times = 10_000
url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mYHcEA  dh435dqUs0moGHeeAJTSLLbdbcbd9ef----,574b95600e9ab7d27eb0bf524ac68c27----"
url = url.force_encoding('us-ascii') if url.respond_to?(:force_encoding)
escaped_url = EscapeUtils.escape_url(url)
puts "Escaping a #{url.bytesize} byte URL #{times} times"

Benchmark.bmbm do |x|
  x.report "Rack::Utils.unescape" do
    times.times do
      Rack::Utils.unescape(escaped_url)
    end
  end
  
  x.report "CGI.unescape" do
    times.times do
      CGI.unescape(escaped_url)
    end
  end
  
  x.report "URLEscape#unescape" do
    times.times do
      URLEscape.unescape(escaped_url)
    end
  end

  x.report "fast_xs_extra#fast_uxs_cgi" do
    times.times do
      url.fast_uxs_cgi
    end
  end

  x.report "EscapeUtils.unescape_url" do
    times.times do
      EscapeUtils.unescape_url(escaped_url)
    end
  end
end