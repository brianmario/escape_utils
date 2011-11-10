# encoding: UTF-8
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
    it "input must be UTF-8 or US-ASCII" do
      escaped = EscapeUtils.escape_html("<b>Bourbon & Branch</b>")

      escaped.force_encoding 'ISO-8859-1'
      lambda {
        EscapeUtils.unescape_html(escaped)
      }.should raise_error(Encoding::CompatibilityError)

      escaped.force_encoding 'UTF-8'
      lambda {
        EscapeUtils.unescape_html(escaped)
      }.should_not raise_error(Encoding::CompatibilityError)
    end

    it "return value should be in UTF-8" do
      escaped = EscapeUtils.escape_html("<b>Bourbon & Branch</b>")

      EscapeUtils.unescape_html(escaped).encoding.should eql(Encoding.find('UTF-8'))
    end
  end
end