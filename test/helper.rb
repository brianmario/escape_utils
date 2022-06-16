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

require 'active_support'
require 'active_support/json'
require "active_support/core_ext/string/output_safety"

require 'action_view'
require 'action_view/helpers'

# bring in minitest
require 'minitest/autorun'

# put lib and test dirs directly on load path
$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
$LOAD_PATH.unshift File.expand_path('..', __FILE__)