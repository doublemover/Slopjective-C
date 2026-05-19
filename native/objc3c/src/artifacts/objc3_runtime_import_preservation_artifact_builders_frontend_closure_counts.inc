std::size_t CountProtocolReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &class_record : records.classes_lexicographic) {
    total += class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record : records.protocols_lexicographic) {
    total += protocol_record.inherited_protocols_lexicographic.size();
  }
  for (const auto &category_record : records.categories_lexicographic) {
    total += category_record.adopted_protocols_lexicographic.size();
  }
  return total;
}

std::size_t CountPropertyAccessorReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      ++total;
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      ++total;
    }
  }
  return total;
}

std::size_t CountPropertyIvarBindingReferences(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  std::size_t total = 0;
  for (const auto &property_record : records.properties_lexicographic) {
    if (!property_record.ivar_binding_symbol.empty()) {
      ++total;
    }
  }
  return total;
}
