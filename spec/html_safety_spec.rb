# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/spec_helper.rb')

class Object
  def html_safe?
    false
  end
end

class TestSafeBuffer < String
  def html_safe?
    true
  end

  def html_safe
    self
  end

  def to_s
    self
  end
end

class String
  def html_safe
    TestSafeBuffer.new(self)
  end
end

include EscapeUtils::HtmlSafety

describe EscapeUtils::HtmlSafety do

  it "should escape unsafe strings and make them safe" do
    escaped = _escape_html("<strong>unsafe</strong>")
    escaped.should eql("&lt;strong&gt;unsafe&lt;&#47;strong&gt;")
    escaped.should be_html_safe
  end

  it "shouldn't escape safe strings" do
    _escape_html("<p>safe string</p>".html_safe).should eql("<p>safe string</p>")
  end

  it "should work with non strings" do
    _escape_html(5).should eql("5")
    _escape_html(:hello).should eql("hello")
  end

end
