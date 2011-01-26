# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "unescape_uri" do
  it "should respond to unescape_uri" do
    EscapeUtils.should respond_to(:unescape_uri)
  end

  it "should unescape a basic url" do
    EscapeUtils.unescape_uri("http%3A%2F%2Fwww.homerun.com%2F").should eql("http://www.homerun.com/")
    EscapeUtils.unescape_uri("http://www.homerun.com/").should eql("http://www.homerun.com/")
  end

  it "should not be thrown by a standalone %" do
    EscapeUtils.unescape_url("%").should eql("%")
  end

  it "should not be thrown by a trailing %" do
    EscapeUtils.unescape_url("http%").should eql("http%")
  end

  # NOTE: from Rack's test suite
  it "should unescape a url containing tags" do
    EscapeUtils.unescape_uri("fo%3Co%3Ebar").should eql("fo<o>bar")
  end

  # NOTE: from Rack's test suite
  it "should unescape a url with spaces" do
    EscapeUtils.unescape_uri("a%20space").should eql("a space")
    EscapeUtils.unescape_uri("a%20%20%20sp%20ace%20").should eql("a   sp ace ")
    EscapeUtils.unescape_uri("a+space").should eql("a+space")
  end

  # NOTE: from Rack's test suite
  it "should unescape a string of mixed characters" do
    EscapeUtils.unescape_uri("q1%212%22%27w%245%267%2Fz8%29%3F%5C").should eql("q1!2\"'w$5&7/z8)?\\")
    EscapeUtils.unescape_uri("q1!2%22'w$5&7/z8)?%5C").should eql("q1!2\"'w$5&7/z8)?\\")
  end

  # NOTE: from Rack's test suite
  it "should unescape correctly for multibyte characters" do
    matz_name = "\xE3\x81\xBE\xE3\x81\xA4\xE3\x82\x82\xE3\x81\xA8".unpack("a*")[0] # Matsumoto
    matz_name.force_encoding("UTF-8") if matz_name.respond_to? :force_encoding
    EscapeUtils.unescape_uri('%E3%81%BE%E3%81%A4%E3%82%82%E3%81%A8').should eql(matz_name)
    matz_name_sep = "\xE3\x81\xBE\xE3\x81\xA4 \xE3\x82\x82\xE3\x81\xA8".unpack("a*")[0] # Matsu moto
    matz_name_sep.force_encoding("UTF-8") if matz_name_sep.respond_to? :force_encoding
    EscapeUtils.unescape_uri('%E3%81%BE%E3%81%A4%20%E3%82%82%E3%81%A8').should eql(matz_name_sep)
  end

  if RUBY_VERSION =~ /^1.9/
    it "should default to the original string's encoding if Encoding.default_internal is nil" do
      Encoding.default_internal = nil
      str = "http%3A%2F%2Fwww.homerun.com%2F"
      str = str.encode('us-ascii')
      EscapeUtils.unescape_uri(str).encoding.should eql(Encoding.find('us-ascii'))
    end

    it "should use Encoding.default_internal" do
      Encoding.default_internal = Encoding.find('utf-8')
      EscapeUtils.unescape_uri("http%3A%2F%2Fwww.homerun.com%2F").encoding.should eql(Encoding.default_internal)
      Encoding.default_internal = Encoding.find('us-ascii')
      EscapeUtils.unescape_uri("http%3A%2F%2Fwww.homerun.com%2F").encoding.should eql(Encoding.default_internal)
    end
  end
end