# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "escape_html" do
  it "should respond to escape_html" do
    EscapeUtils.should respond_to(:escape_html)
  end

  it "should escape a basic html tag, also escaping the '/' character if the secure parameter is true" do
    EscapeUtils.escape_html("<some_tag/>").should eql("&lt;some_tag&#47;&gt;")
  end

  it "should escape a basic html tag, not escaping the '/' character if the secure parameter is false" do
    EscapeUtils.escape_html("<some_tag/>", false).should eql("&lt;some_tag/&gt;")
  end

  it "should escape a basic html tag, not escaping the '/' character if EscapeUtils.html_secure is false" do
    EscapeUtils.html_secure = false
    EscapeUtils.escape_html("<some_tag/>").should eql("&lt;some_tag/&gt;")
    EscapeUtils.html_secure = true
  end

  it "should escape double-quotes" do
    EscapeUtils.escape_html("<some_tag some_attr=\"some value\"/>").should eql("&lt;some_tag some_attr=&quot;some value&quot;&#47;&gt;")
  end

  it "should escape single-quotes" do
    EscapeUtils.escape_html("<some_tag some_attr='some value'/>").should eql("&lt;some_tag some_attr=&#39;some value&#39;&#47;&gt;")
  end

  it "should escape the & character" do
    EscapeUtils.escape_html("<b>Bourbon & Branch</b>").should eql("&lt;b&gt;Bourbon &amp; Branch&lt;&#47;b&gt;")
  end

  if RUBY_VERSION =~ /^1.9/
    it "should default to the original string's encoding if Encoding.default_internal is nil" do
      Encoding.default_internal = nil
      str = "<b>Bourbon & Branch</b>"
      str = str.encode('us-ascii')
      EscapeUtils.escape_html(str).encoding.should eql(Encoding.find('us-ascii'))
    end

    it "should use Encoding.default_internal" do
      Encoding.default_internal = Encoding.find('utf-8')
      EscapeUtils.escape_html("<b>Bourbon & Branch</b>").encoding.should eql(Encoding.default_internal)
      Encoding.default_internal = Encoding.find('us-ascii')
      EscapeUtils.escape_html("<b>Bourbon & Branch</b>").encoding.should eql(Encoding.default_internal)
    end
  end
end