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
    it "return value should be in original string's encoding" do
      str = "<b>Bourbon & Branch</b>".encode('us-ascii')
      EscapeUtils.escape_xml(str).encoding.should eql(Encoding.find('us-ascii'))
      str = "<b>Bourbon & Branch</b>".encode('utf-8')
      EscapeUtils.escape_xml(str).encoding.should eql(Encoding.find('utf-8'))
    end
  end
end
