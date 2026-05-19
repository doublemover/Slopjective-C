struct Objc3ProtocolCategoryCompositionSummary {
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const {
    return protocol_composition_sites + category_composition_sites;
  }
};

struct Objc3ClassProtocolCategoryLinkingSummary {
  // completeness anchor: these deterministic counts validate the
  // first-class executable metadata graph packet.
  // completion anchor: protocol/category composition counts stay
  // stable so protocol/category/property/ivar export graph closure can fail
  // closed on deterministic sema inputs.
  // freeze anchor: these counts remain the canonical semantic
  // consistency inputs for executable metadata graph admission.
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const {
    return protocol_composition_sites + category_composition_sites;
  }
};

struct Objc3SelectorNormalizationSummary {
  std::size_t methods_total = 0;
  std::size_t normalized_methods = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_parameter_piece_entries = 0;
  std::size_t selector_pieceless_methods = 0;
  std::size_t selector_spelling_mismatches = 0;
  std::size_t selector_arity_mismatches = 0;
  std::size_t selector_parameter_linkage_mismatches = 0;
  std::size_t selector_normalization_flag_mismatches = 0;
  std::size_t selector_missing_keyword_pieces = 0;
  bool deterministic = true;

  std::size_t contract_violations() const {
    return selector_pieceless_methods + selector_spelling_mismatches +
           selector_arity_mismatches + selector_parameter_linkage_mismatches +
           selector_normalization_flag_mismatches +
           selector_missing_keyword_pieces;
  }
};

struct Objc3PropertyAttributeSummary {
  std::size_t properties_total = 0;
  std::size_t attribute_entries = 0;
  std::size_t readonly_modifiers = 0;
  std::size_t readwrite_modifiers = 0;
  std::size_t atomic_modifiers = 0;
  std::size_t nonatomic_modifiers = 0;
  std::size_t copy_modifiers = 0;
  std::size_t strong_modifiers = 0;
  std::size_t weak_modifiers = 0;
  std::size_t assign_modifiers = 0;
  std::size_t getter_modifiers = 0;
  std::size_t setter_modifiers = 0;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool deterministic = true;

  std::size_t ownership_modifiers() const {
    return copy_modifiers + strong_modifiers + weak_modifiers + assign_modifiers;
  }

  std::size_t contract_violations() const {
    return invalid_attribute_entries + property_contract_violations;
  }
};
