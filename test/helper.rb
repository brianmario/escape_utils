# Basic test environment.

module HideOwnWarnings
  def warn(message)
    unless message.include?("EscapeUtils")
      super
    end
  end
end
Warning.prepend(HideOwnWarnings)

require 'bundler/setup'
require 'escape_utils'

# bring in minitest
require 'minitest/autorun'

# put lib and test dirs directly on load path
$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
$LOAD_PATH.unshift File.expand_path('..', __FILE__)