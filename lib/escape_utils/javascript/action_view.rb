# encoding: utf-8

module ActionView
  module Helpers
    module JavaScriptHelper
      def escape_javascript(s)
        EscapeUtils.escape_javascript(s)
      end
    end
  end
end
