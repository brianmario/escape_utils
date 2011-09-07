require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "unescape_html" do
  it "should respond to unescape_html" do
    EscapeUtils.should respond_to(:unescape_html)
  end

  it "should unescape a basic html tag" do
    EscapeUtils.unescape_html("&lt;some_tag&#47;&gt;").should eql("<some_tag/>")
  end

  it "should unescape double-quotes" do
    EscapeUtils.unescape_html("&lt;some_tag some_attr=&quot;some value&quot;&#47;&gt;").should eql("<some_tag some_attr=\"some value\"/>")
  end

  it "should unescape single-quotes" do
    EscapeUtils.unescape_html("&lt;some_tag some_attr=&#39;some value&#39;&#47;&gt;").should eql("<some_tag some_attr='some value'/>")
  end

  it "should unescape the & character" do
    EscapeUtils.unescape_html("&lt;b&gt;Bourbon &amp; Branch&lt;&#47;b&gt;").should eql("<b>Bourbon & Branch</b>")
  end

  it "should pass through incompletely escaped tags" do
    EscapeUtils.unescape_html("&").should eql("&")
    EscapeUtils.unescape_html("&lt").should eql("&lt")
  end

  if RUBY_VERSION =~ /^1.9/
    it "return value should be in original string's encoding" do
      str = "&lt;b&gt;Bourbon &amp; Branch&lt;/b&gt;".encode('us-ascii')
      EscapeUtils.unescape_html(str).encoding.should eql(Encoding.find('us-ascii'))
      str = "&lt;b&gt;Bourbon &amp; Branch&lt;/b&gt;".encode('utf-8')
      EscapeUtils.unescape_html(str).encoding.should eql(Encoding.find('utf-8'))
    end
  end
end