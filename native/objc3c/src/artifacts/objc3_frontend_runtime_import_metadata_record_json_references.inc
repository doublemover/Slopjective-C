class RuntimeMetadataReferenceJsonEmitter {
 public:
  explicit RuntimeMetadataReferenceJsonEmitter(std::ostringstream &out)
      : out_(out) {}

  bool HasReferences() const { return !first_reference_; }

  void Emit(const std::string &reference_kind, const std::string &owner_kind,
            const std::string &owner_name, const std::string &target_kind,
            const std::string &target_name, unsigned line, unsigned column) {
    if (!first_reference_) {
      out_ << ",\n";
    }
    first_reference_ = false;
    out_ << "    {\"reference_kind\":\"" << EscapeJsonString(reference_kind)
         << "\",\"owner_kind\":\"" << EscapeJsonString(owner_kind)
         << "\",\"owner_name\":\"" << EscapeJsonString(owner_name)
         << "\",\"target_kind\":\"" << EscapeJsonString(target_kind)
         << "\",\"target_name\":\"" << EscapeJsonString(target_name)
         << "\",\"line\":" << line << ",\"column\":" << column << "}";
  }

 private:
  std::ostringstream &out_;
  bool first_reference_ = true;
};

void AppendClassRuntimeMetadataReferencesJson(
    RuntimeMetadataReferenceJsonEmitter &reference_emitter,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      reference_emitter.Emit("class-superclass", class_record.record_kind,
                             class_record.name, "class",
                             class_record.super_name, class_record.line,
                             class_record.column);
    }
    for (const auto &protocol_name :
         class_record.adopted_protocols_lexicographic) {
      reference_emitter.Emit("class-adopted-protocol",
                             class_record.record_kind, class_record.name,
                             "protocol", protocol_name, class_record.line,
                             class_record.column);
    }
  }
}

void AppendProtocolRuntimeMetadataReferencesJson(
    RuntimeMetadataReferenceJsonEmitter &reference_emitter,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  for (const auto &protocol_record :
       runtime_metadata_source_records.protocols_lexicographic) {
    for (const auto &inherited_name :
         protocol_record.inherited_protocols_lexicographic) {
      reference_emitter.Emit("protocol-inherited-protocol", "protocol",
                             protocol_record.name, "protocol", inherited_name,
                             protocol_record.line, protocol_record.column);
    }
  }
}

void AppendCategoryRuntimeMetadataReferencesJson(
    RuntimeMetadataReferenceJsonEmitter &reference_emitter,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  for (const auto &category_record :
       runtime_metadata_source_records.categories_lexicographic) {
    for (const auto &protocol_name :
         category_record.adopted_protocols_lexicographic) {
      reference_emitter.Emit("category-adopted-protocol",
                             category_record.record_kind,
                             category_record.category_name, "protocol",
                             protocol_name, category_record.line,
                             category_record.column);
    }
  }
}

void AppendPropertyRuntimeMetadataReferencesJson(
    RuntimeMetadataReferenceJsonEmitter &reference_emitter,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  for (const auto &property_record :
       runtime_metadata_source_records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      reference_emitter.Emit("property-getter-selector",
                             property_record.owner_kind,
                             property_record.owner_name, "selector",
                             property_record.effective_getter_selector,
                             property_record.line, property_record.column);
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      reference_emitter.Emit("property-setter-selector",
                             property_record.owner_kind,
                             property_record.owner_name, "selector",
                             property_record.effective_setter_selector,
                             property_record.line, property_record.column);
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      reference_emitter.Emit("property-ivar-binding",
                             property_record.owner_kind,
                             property_record.owner_name,
                             "ivar-binding-symbol",
                             property_record.ivar_binding_symbol,
                             property_record.line, property_record.column);
    }
  }
}

void AppendMethodRuntimeMetadataReferencesJson(
    RuntimeMetadataReferenceJsonEmitter &reference_emitter,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  for (const auto &method_record :
       runtime_metadata_source_records.methods_lexicographic) {
    reference_emitter.Emit("method-selector", method_record.owner_kind,
                           method_record.owner_name, "selector",
                           method_record.selector, method_record.line,
                           method_record.column);
  }
}
