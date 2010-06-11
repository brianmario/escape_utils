# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "escape_javascript" do
  it "should respond to escape_javascript" do
    EscapeUtils.should respond_to(:escape_javascript)
  end

  # these are from the ActionView tests
  it "should return an empty string if passed nil" do
    EscapeUtils.escape_javascript(nil).should eql("")
  end

  it "should escape quotes and newlines" do
    EscapeUtils.escape_javascript(%(This "thing" is really\n netos\r\n')).should eql(%(This \\"thing\\" is really\\n netos\\n\\'))
  end

  it "should escape backslashes" do
    EscapeUtils.escape_javascript(%(backslash\\test)).should eql(%(backslash\\\\test))
  end

  it "should escape closed html tags" do
    EscapeUtils.escape_javascript(%(dont </close> tags)).should eql(%(dont <\\/close> tags))
  end

  if RUBY_VERSION =~ /^1.9/
    it "should default to utf-8 if Encoding.default_internal is nil" do
      Encoding.default_internal = nil
      EscapeUtils.escape_javascript(%(dont </close> tags)).encoding.should eql(Encoding.find('utf-8'))
    end

    it "should use Encoding.default_internal" do
      Encoding.default_internal = Encoding.find('utf-8')
      EscapeUtils.escape_javascript(%(dont </close> tags)).encoding.should eql(Encoding.default_internal)
      Encoding.default_internal = Encoding.find('us-ascii')
      EscapeUtils.escape_javascript(%(dont </close> tags)).encoding.should eql(Encoding.default_internal)
    end
  end
end