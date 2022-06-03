# encoding: utf-8
require File.expand_path("../../helper", __FILE__)

class HtmlUnescapeTest < Minitest::Test
  def test_basic_html
    assert_equal "<some_tag/>", EscapeUtils.unescape_html("&lt;some_tag&#47;&gt;")
  end

  def test_double_quotes
    assert_equal "<some_tag some_attr=\"some value\"/>", EscapeUtils.unescape_html("&lt;some_tag some_attr=&quot;some value&quot;&#47;&gt;")
  end

  def test_single_quotes
    assert_equal "<some_tag some_attr='some value'/>", EscapeUtils.unescape_html("&lt;some_tag some_attr=&#39;some value&#39;&#47;&gt;")
  end

  def test_amperstand
    assert_equal "<b>Bourbon & Branch</b>", EscapeUtils.unescape_html("&lt;b&gt;Bourbon &amp; Branch&lt;&#47;b&gt;")
  end

  def test_passes_through_incompletely_escaped_tags
    assert_equal "&", EscapeUtils.unescape_html("&")
    assert_equal "&lt", EscapeUtils.unescape_html("&lt")
  end

  def test_return_value_is_tagged_as_utf8
    escaped = EscapeUtils.escape_html("<b>Bourbon & Branch</b>")
    assert_equal Encoding.find('UTF-8'), EscapeUtils.unescape_html(escaped).encoding
  end
end
