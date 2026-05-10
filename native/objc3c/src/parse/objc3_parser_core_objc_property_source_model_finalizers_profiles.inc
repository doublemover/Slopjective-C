  std::string BuildExecutablePropertyAttributeProfile(
      const Objc3PropertyDecl &property) {
    std::vector<std::string> attributes;
    attributes.reserve(property.attributes.size());
    for (const auto &attribute : property.attributes) {
      std::string rendered = attribute.name;
      if (attribute.has_value) {
        rendered += "=" + TrimAsciiWhitespaceCopy(attribute.value);
      }
      attributes.push_back(std::move(rendered));
    }
    std::sort(attributes.begin(), attributes.end());

    std::ostringstream out;
    out << "readonly=" << (property.is_readonly ? 1 : 0)
        << ";readwrite=" << (property.is_readwrite ? 1 : 0)
        << ";atomic=" << (property.is_atomic ? 1 : 0)
        << ";nonatomic=" << (property.is_nonatomic ? 1 : 0)
        << ";copy=" << (property.is_copy ? 1 : 0)
        << ";retain=" << (property.is_retain ? 1 : 0)
        << ";strong=" << (property.is_strong ? 1 : 0)
        << ";weak=" << (property.is_weak ? 1 : 0)
        << ";unowned=" << (property.is_unowned ? 1 : 0)
        << ";unsafe_unretained=" << (property.is_unsafe_unretained ? 1 : 0)
        << ";assign=" << (property.is_assign ? 1 : 0)
        << ";nullable=" << (property.is_nullable ? 1 : 0)
        << ";nonnull=" << (property.is_nonnull ? 1 : 0)
        << ";null_resettable=" << (property.is_null_resettable ? 1 : 0)
        << ";class=" << (property.is_class ? 1 : 0)
        << ";direct=" << (property.is_direct ? 1 : 0)
        << ";attributes=";
    for (std::size_t i = 0; i < attributes.size(); ++i) {
      if (i != 0u) {
        out << ",";
      }
      out << attributes[i];
    }
    return out.str();
  }

  std::string BuildExecutableAccessorOwnershipProfile(
      const Objc3PropertyDecl &property) {
    std::ostringstream out;
    out << "getter=" << property.effective_getter_selector
        << ";setter_available=" << (property.effective_setter_available ? 1 : 0)
        << ";setter=";
    if (property.effective_setter_available) {
      out << property.effective_setter_selector;
    } else {
      out << "<none>";
    }
    out << ";ownership_lifetime=" << property.ownership_lifetime_profile
        << ";runtime_hook=" << property.ownership_runtime_hook_profile;
    return out.str();
  }
