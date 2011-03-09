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
    EscapeUtils.escape_javascript(%(This "thing" is really\n netos\r\n\n')).should eql(%(This \\"thing\\" is really\\n netos\\n\\n\\'))
  end

  it "should escape backslashes" do
    EscapeUtils.escape_javascript(%(backslash\\test)).should eql(%(backslash\\\\test))
  end

  it "should escape closed html tags" do
    EscapeUtils.escape_javascript(%(keep <open>, but dont </close> tags)).should eql(%(keep <open>, but dont <\\/close> tags))
  end

  if RUBY_VERSION =~ /^1.9/
    it "return value should be in original string's encoding" do
      str = "dont </close> tags".encode('us-ascii')
      EscapeUtils.escape_javascript(str).encoding.should eql(Encoding.find('us-ascii'))
      str = "dont </close> tags".encode('utf-8')
      EscapeUtils.escape_javascript(str).encoding.should eql(Encoding.find('utf-8'))
    end
  end
end

