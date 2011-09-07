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
    it "return value should be in original string's encoding" do
      str = "dont <\\/close> tags".encode('us-ascii')
      EscapeUtils.unescape_javascript(str).encoding.should eql(Encoding.find('us-ascii'))
      str = "dont <\\/close> tags".encode('utf-8')
      EscapeUtils.unescape_javascript(str).encoding.should eql(Encoding.find('utf-8'))
    end
  end
end