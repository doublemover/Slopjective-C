  bool ApplyObjcPropertyNamingBehaviorAttribute(
      Objc3PropertyDecl &property, const Objc3PropertyAttributeDecl &attribute) {
    if (attribute.name == "getter") {
      property.has_getter = true;
      property.getter_selector = attribute.value;
    } else if (attribute.name == "setter") {
      property.has_setter = true;
      property.setter_selector = attribute.value;
    } else if (attribute.name == "behavior") {
      property.property_behavior_declared = attribute.has_value;
      property.property_behavior_name = attribute.value;
    } else {
      return false;
    }
    return true;
  }
