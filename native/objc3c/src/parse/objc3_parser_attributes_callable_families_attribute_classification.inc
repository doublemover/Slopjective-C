  bool IsRetainableCFamilyPayloadCallableAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "objc_family_retain" ||
           attribute_name.text == "objc_family_release" ||
           attribute_name.text == "objc_family_autorelease";
  }

  bool IsRetainableCFamilyFlagCallableAttribute(
      const Token &attribute_name) const {
    return attribute_name.text == "os_returns_retained" ||
           attribute_name.text == "os_returns_not_retained" ||
           attribute_name.text == "os_consumed" ||
           attribute_name.text == "cf_returns_retained" ||
           attribute_name.text == "cf_returns_not_retained" ||
           attribute_name.text == "cf_consumed" ||
           attribute_name.text == "ns_returns_retained" ||
           attribute_name.text == "ns_returns_not_retained" ||
           attribute_name.text == "ns_consumed";
  }
