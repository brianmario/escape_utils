# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "unescape_url" do
  it "should respond to unescape_url" do
    EscapeUtils.should respond_to(:unescape_url)
  end

  it "should unescape a basic url" do
    EscapeUtils.unescape_url("http%3A%2F%2Fwww.homerun.com%2F").should eql("http://www.homerun.com/")
  end

  # NOTE: from Rack's test suite
  it "should unescape a url containing tags" do
    EscapeUtils.unescape_url("fo%3Co%3Ebar").should eql("fo<o>bar")
  end

  # NOTE: from Rack's test suite
  it "should unescape a url with spaces" do
    EscapeUtils.unescape_url("a+space").should eql("a space")
    EscapeUtils.unescape_url("a+++sp+ace+").should eql("a   sp ace ")
  end

  # NOTE: from Rack's test suite
  it "should unescape a string of mixed characters" do
    EscapeUtils.unescape_url("q1%212%22%27w%245%267%2Fz8%29%3F%5C").should eql("q1!2\"'w$5&7/z8)?\\")
  end

  # NOTE: from Rack's test suite
  it "should unescape correctly for multibyte characters" do
    matz_name = "\xE3\x81\xBE\xE3\x81\xA4\xE3\x82\x82\xE3\x81\xA8" # Matsumoto
    EscapeUtils.unescape_url('%E3%81%BE%E3%81%A4%E3%82%82%E3%81%A8').should eql(matz_name)
    matz_name_sep = "\xE3\x81\xBE\xE3\x81\xA4 \xE3\x82\x82\xE3\x81\xA8" # Matsu moto
    EscapeUtils.unescape_url('%E3%81%BE%E3%81%A4+%E3%82%82%E3%81%A8').should eql(matz_name_sep)
  end

  if RUBY_VERSION =~ /^1.9/
    it "input must be UTF-8 or US-ASCII" do
      escaped = EscapeUtils.unescape_url("a+space")

      escaped.force_encoding 'ISO-8859-1'
      lambda {
        EscapeUtils.unescape_url(escaped)
      }.should raise_error(Encoding::CompatibilityError)

      escaped.force_encoding 'UTF-8'
      lambda {
        EscapeUtils.unescape_url(escaped)
      }.should_not raise_error(Encoding::CompatibilityError)
    end

    it "return value should be in UTF-8" do
      escaped = EscapeUtils.escape_url("a space")

      EscapeUtils.unescape_url(escaped).encoding.should eql(Encoding.find('UTF-8'))
    end
  end
end