  bool ApplyObjcPropertyStorageOwnershipAttribute(
      Objc3PropertyDecl &property, const Objc3PropertyAttributeDecl &attribute) {
    if (attribute.name == "readonly") {
      property.is_readonly = true;
    } else if (attribute.name == "readwrite") {
      property.is_readwrite = true;
    } else if (attribute.name == "atomic") {
      property.is_atomic = true;
    } else if (attribute.name == "nonatomic") {
      property.is_nonatomic = true;
    } else if (attribute.name == "copy") {
      property.is_copy = true;
    } else if (attribute.name == "retain") {
      property.is_retain = true;
    } else if (attribute.name == "strong") {
      property.is_strong = true;
    } else if (attribute.name == "weak") {
      property.is_weak = true;
    } else if (attribute.name == "unowned") {
      property.is_unowned = true;
    } else if (attribute.name == "unsafe_unretained") {
      property.is_unsafe_unretained = true;
    } else if (attribute.name == "assign") {
      property.is_assign = true;
    } else if (attribute.name == "nullable") {
      property.is_nullable = true;
    } else if (attribute.name == "nonnull") {
      property.is_nonnull = true;
    } else if (attribute.name == "null_resettable") {
      property.is_null_resettable = true;
    } else if (attribute.name == "class") {
      property.is_class = true;
    } else if (attribute.name == "direct") {
      property.is_direct = true;
    } else {
      return false;
    }
    return true;
  }
