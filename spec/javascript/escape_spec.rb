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
    EscapeUtils.escape_javascript(%(This "thing" is really\n netos')).should eql(%(This \\"thing\\" is really\\n netos\\'))
  end

  it "should escape backslashes" do
    EscapeUtils.escape_javascript(%(backslash\\test)).should eql(%(backslash\\\\test))
  end

  it "should escape closed html tags" do
    EscapeUtils.escape_javascript(%(dont </close> tags)).should eql(%(dont <\\/close> tags))
  end
end