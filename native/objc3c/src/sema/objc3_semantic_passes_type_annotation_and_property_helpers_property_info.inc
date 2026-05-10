static Objc3PropertyInfo BuildPropertyInfo(const Objc3PropertyDecl &property,
                                           const std::string &owner_name,
                                           const std::string &owner_kind,
                                           bool arc_mode_enabled,
                                           std::vector<std::string> &diagnostics) {
  const auto property_has_attribute_named =
      [&property](std::string_view name) {
        for (const auto &attribute : property.attributes) {
          if (attribute.name == name) {
            return true;
          }
        }
        return false;
      };
  const auto property_is_object_like = [](const Objc3PropertyInfo &info) {
    return info.id_spelling || info.instancetype_spelling ||
           info.object_pointer_type_spelling || info.type == ValueType::ObjCId ||
           info.type == ValueType::ObjCClass ||
           info.type == ValueType::ObjCInstancetype ||
