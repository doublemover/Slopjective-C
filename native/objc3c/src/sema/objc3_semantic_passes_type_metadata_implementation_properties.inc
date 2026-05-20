    std::vector<std::string> property_names;
    property_names.reserve(implementation_it->second.properties.size());
    for (const auto &property_entry : implementation_it->second.properties) {
      property_names.push_back(property_entry.first);
    }
    std::sort(property_names.begin(), property_names.end());

    metadata.properties_lexicographic.reserve(property_names.size());
    for (const std::string &property_name : property_names) {
      const auto property_it = implementation_it->second.properties.find(property_name);
      if (property_it == implementation_it->second.properties.end()) {
        continue;
      }
      const Objc3PropertyInfo &source = property_it->second;
      Objc3SemanticPropertyTypeMetadata property_metadata;
      property_metadata.name = property_name;
      property_metadata.type = source.type;
      property_metadata.canonical_type = source.canonical_type;
      property_metadata.is_vector = source.is_vector;
      property_metadata.vector_base_spelling = source.vector_base_spelling;
      property_metadata.vector_lane_count = source.vector_lane_count;
      property_metadata.id_spelling = source.id_spelling;
      property_metadata.class_spelling = source.class_spelling;
      property_metadata.instancetype_spelling = source.instancetype_spelling;
      property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;
      property_metadata.has_generic_suffix = source.has_generic_suffix;
      property_metadata.has_pointer_declarator = source.has_pointer_declarator;
      property_metadata.has_nullability_suffix = source.has_nullability_suffix;
      property_metadata.has_ownership_qualifier = source.has_ownership_qualifier;
      property_metadata.has_invalid_generic_suffix = source.has_invalid_generic_suffix;
      property_metadata.has_invalid_pointer_declarator = source.has_invalid_pointer_declarator;
      property_metadata.has_invalid_nullability_suffix = source.has_invalid_nullability_suffix;
      property_metadata.has_invalid_ownership_qualifier = source.has_invalid_ownership_qualifier;
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.ownership_insert_retain = source.ownership_insert_retain;
      property_metadata.ownership_insert_release = source.ownership_insert_release;
      property_metadata.ownership_insert_autorelease = source.ownership_insert_autorelease;
      property_metadata.ownership_is_weak_reference = source.ownership_is_weak_reference;
      property_metadata.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
      property_metadata.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
      property_metadata.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
      property_metadata.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
      property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
      property_metadata.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
      property_metadata.ownership_lifetime_profile = source.ownership_lifetime_profile;
      property_metadata.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_retain = source.is_retain;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
      property_metadata.is_unowned = source.is_unowned;
      property_metadata.is_unsafe_unretained = source.is_unsafe_unretained;
      property_metadata.is_assign = source.is_assign;
      property_metadata.is_nullable = source.is_nullable;
      property_metadata.is_nonnull = source.is_nonnull;
      property_metadata.is_null_resettable = source.is_null_resettable;
      property_metadata.is_class = source.is_class;
      property_metadata.is_direct = source.is_direct;
      property_metadata.has_getter = source.has_getter;
      property_metadata.has_setter = source.has_setter;
      property_metadata.getter_selector = source.getter_selector;
      property_metadata.setter_selector = source.setter_selector;
      property_metadata.ivar_binding_symbol = source.ivar_binding_symbol;
      property_metadata.executable_synthesized_binding_kind =
          source.executable_synthesized_binding_kind;
      property_metadata.executable_synthesized_binding_symbol =
          source.executable_synthesized_binding_symbol;
      // runtime-backed ownership attribute surface anchor: the
      // implementation-side property handoff preserves the same ownership
      // profiles so emitted runtime-backed descriptors can stay synchronized
      // across interface/implementation pairs without reinterpreting source.
      property_metadata.property_attribute_profile =
          source.property_attribute_profile;
      property_metadata.property_behavior_declared =
          source.property_behavior_declared;
      property_metadata.property_behavior_name =
          source.property_behavior_name;
      property_metadata.effective_getter_selector =
          source.effective_getter_selector;
      property_metadata.effective_setter_available =
          source.effective_setter_available;
      property_metadata.effective_setter_selector =
          source.effective_setter_selector;
      property_metadata.accessor_ownership_profile =
          source.accessor_ownership_profile;
      apply_arc_property_interaction_metadata(property_metadata);
      property_metadata.executable_ivar_layout_symbol =
          source.executable_ivar_layout_symbol;
      property_metadata.executable_ivar_layout_slot_index =
          source.executable_ivar_layout_slot_index;
      property_metadata.executable_ivar_layout_size_bytes =
          source.executable_ivar_layout_size_bytes;
      property_metadata.executable_ivar_layout_alignment_bytes =
          source.executable_ivar_layout_alignment_bytes;
      property_metadata.executable_ivar_layout_offset_bytes =
          source.executable_ivar_layout_offset_bytes;
      property_metadata.executable_ivar_layout_padding_bytes =
          source.executable_ivar_layout_padding_bytes;
      property_metadata.executable_ivar_layout_inherited_slot_count =
          source.executable_ivar_layout_inherited_slot_count;
      property_metadata.executable_ivar_layout_inherited_size_bytes =
          source.executable_ivar_layout_inherited_size_bytes;
      property_metadata.executable_ivar_layout_owner_size_bytes =
          source.executable_ivar_layout_owner_size_bytes;
      property_metadata.executable_ivar_init_order_index =
          source.executable_ivar_init_order_index;
      property_metadata.executable_ivar_destroy_order_index =
          source.executable_ivar_destroy_order_index;
      property_metadata.executable_ivar_layout_valid =
          source.executable_ivar_layout_valid;
      property_metadata.executable_ivar_layout_replay_key =
          source.executable_ivar_layout_replay_key;
      property_metadata.invalid_attribute_entries = source.invalid_attribute_entries;
      property_metadata.property_contract_violations = source.property_contract_violations;
      property_metadata.has_unknown_attribute = source.has_unknown_attribute;
      property_metadata.has_duplicate_attribute = source.has_duplicate_attribute;
      property_metadata.has_readwrite_conflict = source.has_readwrite_conflict;
      property_metadata.has_atomicity_conflict = source.has_atomicity_conflict;
      property_metadata.has_ownership_conflict = source.has_ownership_conflict;
      property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;
      property_metadata.has_accessor_selector_contract_violation = source.has_accessor_selector_contract_violation;
      property_metadata.has_invalid_attribute_contract = source.has_invalid_attribute_contract;
      metadata.properties_lexicographic.push_back(std::move(property_metadata));
    }
