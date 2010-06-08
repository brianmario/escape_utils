# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils do
  it "should respond to escape_html" do
    EscapeUtils.should respond_to(:escape_html)
  end

  context "escape_html" do
    it "should escape a basic html tag" do
      EscapeUtils.escape_html("<some_tag/>").should eql("&lt;some_tag/&gt;")
    end

    it "should escape double-quotes" do
      EscapeUtils.escape_html("<some_tag some_attr=\"some value\"/>").should eql("&lt;some_tag some_attr=&quot;some value&quot;/&gt;")
    end

    it "should escape single-quotes" do
      EscapeUtils.escape_html("<some_tag some_attr='some value'/>").should eql("&lt;some_tag some_attr=&#39;some value&#39;/&gt;")
    end

    it "should escape the & character" do
      EscapeUtils.escape_html("<b>Bourbon & Branch</b>").should eql("&lt;b&gt;Bourbon &amp; Branch&lt;/b&gt;")
    end
  end
end