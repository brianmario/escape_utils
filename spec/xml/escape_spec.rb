# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe EscapeUtils, "escape_xml" do
  it "should respond to escape_xml" do
    EscapeUtils.should respond_to(:escape_xml)
  end

  it "should escape a basic xml tag" do
    EscapeUtils.escape_xml("<some_tag/>").should eql("&lt;some_tag/&gt;")
  end

  it "should escape double-quotes" do
    EscapeUtils.escape_xml("<some_tag some_attr=\"some value\"/>").should eql("&lt;some_tag some_attr=&quot;some value&quot;/&gt;")
  end

  it "should escape single-quotes" do
    EscapeUtils.escape_xml("<some_tag some_attr='some value'/>").should eql("&lt;some_tag some_attr=&apos;some value&apos;/&gt;")
  end

  it "should escape the & character" do
    EscapeUtils.escape_xml("<b>Bourbon & Branch</b>").should eql("&lt;b&gt;Bourbon &amp; Branch&lt;/b&gt;")
  end

  it "should replace invalid characters" do
    # See http://www.w3.org/TR/REC-xml/#charsets for details.
    VALID = [
      (0x9..0xA), 0xD,
      (0x20..0xD7FF),
      (0xE000..0xFFFD),
      (0x10000..0x10FFFF)
    ]
    REPLACEMENT_CHAR = "?".unpack('U*').first
    VALID.each do |range|
      if range.kind_of? Range
        start = range.begin
        last = range.end
        last -= 1 if range.exclude_end?
      else
        start = last = range
      end
      input = [start.pred, start, last, last.next].pack('U*')
      expect = [REPLACEMENT_CHAR, start, last, REPLACEMENT_CHAR].pack('U*')
      EscapeUtils.escape_xml(input).should eql(expect)
    end
  end

  if RUBY_VERSION =~ /^1.9/
    it "input must be UTF-8 or US-ASCII" do
      str = "<some_tag/>"

      str.force_encoding 'ISO-8859-1'
      lambda {
        EscapeUtils.escape_xml(str)
      }.should raise_error(Encoding::CompatibilityError)

      str.force_encoding 'UTF-8'
      lambda {
        EscapeUtils.escape_xml(str)
      }.should_not raise_error(Encoding::CompatibilityError)
    end

    it "return value should be in UTF-8" do
      str = "<some_tag/>"
      EscapeUtils.escape_xml(str).encoding.should eql(Encoding.find('UTF-8'))
    end
  end
end
