# encoding: utf-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'benchmark'

require 'rack'
require 'erb'
require 'cgi'
require 'url_escape'
require 'fast_xs_extra'
require 'escape_utils'

times = 10_000
url = "https://www.yourmom.com/cgi-bin/session.cgi?sess_args=mYHcEA  dh435dqUs0moGHeeAJTSLLbdbcbd9ef----,574b95600e9ab7d27eb0bf524ac68c27----"
puts "Escaping a #{url.bytesize} byte URL #{times} times"

Benchmark.bmbm do |x|
  x.report do
    puts "ERB::Util.url_encode"
    times.times do
      ERB::Util.url_encode(url)
    end
  end

  x.report do
    puts "Rack::Utils.escape"
    times.times do
      Rack::Utils.escape(url)
    end
  end

  x.report do
    puts "CGI.escape"
    times.times do
      CGI.escape(url)
    end
  end

  x.report do
    puts "URLEscape#escape"
    times.times do
      URLEscape.escape(url)
    end
  end

  x.report do
    puts "fast_xs_extra#fast_xs_url"
    times.times do
      url.fast_xs_url
    end
  end

  x.report do
    puts "EscapeUtils.escape_url"
    times.times do
      EscapeUtils.escape_url(url)
    end
  end
end