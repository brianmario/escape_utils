require File.expand_path("../../helper", __FILE__)

require 'active_support'
require 'active_support/json'
require 'action_view'
require 'action_view/helpers'

class JavascriptEscapeTest < Minitest::Test
  ActiveSupport.escape_html_entities_in_json = true

  module ActionViewHelper
    include ActionView::Helpers::JavaScriptHelper
    extend self
  end

  def test_returns_empty_string_if_nil_passed
    assert_compatible nil
  end

  def test_quotes_and_newlines
    assert_compatible %(This "thing" is really\n netos\r\n\n')
  end

  def test_backslashes
    assert_compatible %(backslash\\test)
  end

  def test_closed_html_tags
    assert_compatible %(keep <open>, but dont </close> tags)
  end

  def test_escape_javascript
    assert_compatible 123
    assert_compatible :en
    assert_compatible false
    assert_compatible true
    assert_compatible %(don't </close> tags)
    assert_compatible (+%(unicode \342\200\250 newline)).force_encoding(Encoding::UTF_8).encode!
    assert_compatible (+%(unicode \342\200\251 newline)).force_encoding(Encoding::UTF_8).encode!
    assert_compatible %(don't </close> tags)
  end

  def test_escape_backtick
    assert_compatible "`"
  end

  def test_escape_dollar_sign
    assert_compatible "$"
  end

  def test_input_must_be_utf8_or_ascii
    str = "dont </close> tags"

    str.force_encoding Encoding::ISO_8859_1
    assert_raises Encoding::CompatibilityError do
      EscapeUtils.escape_javascript(str)
    end

    str.force_encoding Encoding::UTF_8
    begin
      EscapeUtils.escape_javascript(str)
    rescue Encoding::CompatibilityError => e
      assert_nil e, "#{e.class.name} raised, expected not to"
    end
  end

  def test_return_value_is_tagged_as_utf8
    str = "dont </close> tags"
    assert_equal Encoding::UTF_8, EscapeUtils.escape_javascript(str).encoding
  end

  private

  def assert_compatible(src)
    assert_equal(
      ActionViewHelper.escape_javascript(src),
      EscapeUtils.escape_javascript(src),
    )
  end
end
