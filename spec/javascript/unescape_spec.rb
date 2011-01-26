# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "unescape_javascript" do
  it "should respond to unescape_javascript" do
    EscapeUtils.should respond_to(:unescape_javascript)
  end

  # these are from the ActionView tests
  it "should return an empty string if passed nil" do
    EscapeUtils.unescape_javascript(nil).should eql("")
  end

  it "should unescape quotes and newlines" do
    EscapeUtils.unescape_javascript(%(This \\"thing\\" is really\\n netos\\n\\n\\')).should eql(%(This "thing" is really\n netos\n\n'))
  end

  it "should unescape backslashes" do
    EscapeUtils.unescape_javascript(%(backslash\\\\test)).should eql(%(backslash\\test))
  end

  it "should unescape closed html tags" do
    EscapeUtils.unescape_javascript(%(dont <\\/close> tags)).should eql(%(dont </close> tags))
  end

  it "should pass through standalone '\'" do
    EscapeUtils.unescape_javascript("\\").should eql("\\")
  end

  if RUBY_VERSION =~ /^1.9/
    it "should default to the original string's encoding if Encoding.default_internal is nil" do
      Encoding.default_internal = nil
      str = %(dont <\\/close> tags)
      str = str.encode('us-ascii')
      EscapeUtils.unescape_javascript(str).encoding.should eql(Encoding.find('us-ascii'))
    end

    it "should use Encoding.default_internal" do
      Encoding.default_internal = Encoding.find('utf-8')
      EscapeUtils.unescape_javascript(%(dont <\\/close> tags)).encoding.should eql(Encoding.default_internal)
      Encoding.default_internal = Encoding.find('us-ascii')
      EscapeUtils.unescape_javascript(%(dont <\\/close> tags)).encoding.should eql(Encoding.default_internal)
    end
  end
end