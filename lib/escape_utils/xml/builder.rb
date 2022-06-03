module Builder
  class XmlBase
    private
    def _escape(text)
      ::EscapeUtils.escape_xml(text.to_s)
    end
  end
end
