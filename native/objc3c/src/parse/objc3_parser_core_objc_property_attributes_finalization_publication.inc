  void ApplyObjcPropertyAttributes(Objc3PropertyDecl &property) {
    for (const auto &attribute : property.attributes) {
      if (ApplyObjcPropertyStorageOwnershipAttribute(property, attribute)) {
        continue;
      }
      ApplyObjcPropertyNamingBehaviorAttribute(property, attribute);
    }
  }
